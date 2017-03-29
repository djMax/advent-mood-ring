import path from 'path';
import WebSocket from 'ws';
import pgp from 'pg-promise';
import express from 'express';

const PORT = process.env.PORT || 3000;
const PGHOST = process.env.PGHOST || 'localhost';
const PGUSER = process.env.PGUSER || 'docker';
const PGPASSWORD = process.env.PGPASSWORD || 'postgis';
const PGDB = process.env.PGDB || 'moodring';

const db = pgp({})({
  host: PGHOST,
  user: PGUSER,
  password: PGPASSWORD,
  database: PGDB,
});

const server = express()
  .use(express.static(path.join(__dirname, '..', 'static')))
  .listen(PORT, () => console.log(`Listening on ${PORT}`));

const wss = new WebSocket.Server({ server });

wss.on('connection', function connection(ws) {
  console.log('Client connected');

  ws.on('message', function incoming(message) {
    console.log('received: %s', message);
  });

  pgp.manyOrNone(`SELECT
    pixel_id, instruction
  FROM
    user JOIN instruction ON user.user_id = instruction.user_id`)
    .then((users) => {
      const parts = [];
      user.forEach((i) => {
        parts.push(user.pixel_id);
        const bits = [];
        user.instruction.forEach((i) => {
          bits.push(...i.color, i.ticks);
        });
        parts.push(bits.join(','));
      });
      ws.send(parts.join('\n'));
    });
});