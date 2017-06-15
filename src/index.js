import path from 'path';
import WebSocket from 'ws';
import pgp from 'pg-promise';
import express from 'express';
import bodyParser from 'body-parser';
import { packedColors, getRgbCsv, getRgbHex } from './colors';

const PORT = process.env.PORT || 3000;
const PGURL = process.env.DATABASE_URL || 'postgres://docker:postgis@localhost:5432/moodring'

const db = pgp({})(PGURL);

const app = express();
const server = app
  .use(express.static(path.join(__dirname, '..', 'static')))
  .use(bodyParser.json())
  .listen(PORT, () => console.log(`Listening on ${PORT}`));

function csv(flow) {
  const bits = [];
  flow.forEach((i) => {
    bits.push(...i.color, i.ticks);
  });
  return bits.join(',');
}

app.post('/update', async (req, res) => {
  await db.result(`INSERT INTO instruction (user_id, instruction)
    SELECT user_id, $1 FROM "user" WHERE identifier = $2
    ON CONFLICT (user_id) DO UPDATE SET instruction = $1`, [
      req.body.instruction,
      req.body.id,
    ]);
  const u = await db.oneOrNone('SELECT pixel_id FROM "user" WHERE identifier = $1', [req.body.id]);
  if (u) {
    const msg = `${u.pixel_id}\n${csv(req.body.instruction.flow)}`;
    wss.clients.forEach((client) => {
      client.send(msg);
    });
  }
  res.sendStatus(200);
});

function getAll() {
  try {
    return db.manyOrNone(`SELECT
    pixel_id, instruction
  FROM
    "user" U JOIN instruction I ON U.user_id = I.user_id`);
  } catch (error) {
    console.error(error);
    return [];
  }
}

app.get('/display', (req, res) => {
  getAll()
    .then(i => res.json(i))
    .catch((e) => {
      console.error(e);
      res.sendStatus(500);
    });
});

const wss = new WebSocket.Server({ server });
const displays = new Set();
const knobs = new Set();
const colors = {};

function findSpot() {
  for (let c = 0; c < 1000000; c += 1) {
    const i = Math.floor(Math.random() * 25);
    let alreadyThere = false;
    knobs.forEach(c => {
      if (c.index === i) {
        alreadyThere = true;
      }
    });
    if (!alreadyThere) {
      return i;
    }
  }
}

wss.on('connection', function connection(client) {
  console.log('New socket connection');

  client.on('message', function incoming(message) {
    if (knobs.has(client)) {
      console.log(`Knob ${client.index} ${message}`);
    }
    if (message.startsWith('KNOB')) {
      client.knobId = message.substring(5);
      client.index = findSpot();
      client.send(`colorTable\n${packedColors()}`);
      console.log('Assigning', client.knobId, client.index);
      knobs.add(client);
    } else if (message.startsWith('DISPLAY')) {
      console.log('New display connection');
      displays.add(client);
      let parts = []
      db.manyOrNone('SELECT spot, reading FROM knob_reading')
        .then((spots) => {
          for (const p of spots) {
            parts.push(p.spot);
            const rgb = getRgbCsv(p.reading);
            parts.push(`0,0,0,150,${rgb},150`);
          }
          console.error('SENDING>>>', parts.join('\n'), '<<<');
          client.send(parts.join('\n'));
        });
    } else if (message.startsWith('S ') && knobs.has(client)) {
      // Got a new value from a knob
      const setting = message.substring(2);
      colors[client.index] = setting;
      const rgb = getRgbCsv(setting);
      const instr = `${client.index}\n0,0,0,150,${rgb},150\n`;
      console.log('Sending', instr);
      db.result(`INSERT INTO knob_reading (spot, reading)
    VALUES ($1, $2)
    ON CONFLICT (spot) DO UPDATE SET reading = $2`, [
          client.index,
          setting,
        ])
        .catch(console.error);
      displays.forEach(s => s.send(instr));
      knobs.forEach((k) => {
        if (k !== client) {
          k.send(`!${getRgbHex(setting)}`);
          setTimeout(() => k.send('!'), 2000);
        }
      })
    }
  });

  client.on('close', () => {
    knobs.delete(client);
    displays.delete(client);
  });
});