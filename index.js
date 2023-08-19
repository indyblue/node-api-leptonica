const { exec } = require('child_process');
const { promisify } = require('util');
const pexec = promisify(exec);

const bind = require('bindings');
const { pix, box } = bind('lept');
const f = pix.formats;

async function main() {
  const d = 'img/';
  const td = d + 'rd/';
  await pexec(`cp ${d}*.ppm ${td}`);
  const p1 = pix.open(td + 's-030.ppm');
  console.log('s-030.ppm', p1);

  const p2 = pix.thresh(p1, 170);
  pix.save(p2, td + 's-030-01-thresh.png', f.png);
  pix.destroy(p1);

  const pline = pix.morph(p2, 'c150.5');
  pix.save(pline, td + 's-030-02-morph-line.png', f.png);

  const bline = pix.connComp(pline);
  pix.destroy(pline);

  const pchar = pix.morph(p2, 'c2.10');
  pix.save(pchar, td + 's-030-03-morph-char.png', f.png);

  const bchar = pix.connComp(p2);
  pix.destroy(pchar);

  console.log('mod lines');
  for (const l of bline) {
    bchar.filter(x => {
      const cy = x.y + x.h / 2;
      return l.y < cy && cy < l.y + l.h;
    }).forEach(x => {
      x.y = l.y;
      x.h = l.h;
    });
  }
  console.log('set geometry');
  box.aSetGeometry(bchar);
  console.log('pix render');

  const p4 = pix.copy(p2);
  box.pixRender(p4, bchar);
  console.log('save');
  // box.pixRender(p4, bline);
  pix.save(p4, td + 's-030-04-boxa.png', f.png);
  console.log('done');
}
main();

/*
const asdf = bind('asdf');
console.log(asdf.myArray);
const x = asdf.hello();
const y = asdf.add(4, 3);
asdf.runCallback(() => {
  console.log('this is my callback');
});
console.log(x, y);
*/

/*
const express = require('express');
const app = express();
const port = 3000;

app.use(express.static('public'));

app.get('/hw', (req, res) => {
  res.send('Hello World!');
});

app.listen(port, () => {
  console.log(`Example app listening on port ${port}`);
});
*/
