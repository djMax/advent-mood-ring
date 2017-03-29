import path from 'path';
import WebSocket from 'ws';
import pgp from 'pg-promise';
import express from 'express';
import bodyParser from 'body-parser';

const PORT = process.env.PORT || 3000;
const PGURL = process.env.DATABASE_URL || 'postgres://docker:postgis@localhost:5432/moodring'

const db = pgp({})(PGURL);

const app = express();
const server = app
  .use(express.static(path.join(__dirname, '..', 'static')))
  .use(bodyParser.json())
  .listen(PORT, () => console.log(`Listening on ${PORT}`));

app.post('/update', async (req, res) => {
  await db.result(`INSERT INTO instruction (user_id, instruction)
    SELECT user_id, $1 FROM "user" WHERE identifier = $2
    ON CONFLICT (user_id) DO UPDATE SET instruction = $1`, [
      req.body.instruction,
      req.body.id,
    ]);
    res.sendStatus(200);
});

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
      users.forEach((i) => {
        parts.push(user.pixel_id);
        const bits = [];
        user.instruction.flow.forEach((i) => {
          bits.push(...i.color, i.ticks);
        });
        parts.push(bits.join(','));
      });
      ws.send(parts.join('\n'));
    });
});