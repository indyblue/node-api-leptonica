const bind = require('bindings');
const { pix, box } = bind('lept');
const f = pix.formats;

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

// const pix1 = pix.open('img/mps.jpg');
// console.log(1, pix1);
// const pix2 = pix.rotate(pix1, 1);
// console.log(2, pix2);
// const pix3 = pix.thresh(pix1, 128);
// console.log(3, pix3);

// console.log(f);
function main() {
  const p1 = pix.open('img/s-030.ppm');
  console.log('s-030.ppm', p1);

  const p2 = pix.thresh(p1, 150);
  pix.save(p2, 'img/s-030-01-thresh.png', f.png);
  pix.destroy(p1);

  const pline = pix.morph(p2, 'c150.5');
  pix.save(pline, 'img/s-030-02-morph-line.png', f.png);

  const bline = pix.connComp(pline);
  pix.destroy(pline);

  const pchar = pix.morph(p2, 'c1.10');
  pix.save(pchar, 'img/s-030-03-morph-char.png', f.png);

  const bchar = pix.connComp(pchar);
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
  pix.save(p4, 'img/s-030-04-boxa.png', f.png);
  console.log('done');
}
main();

// let i = 0;
// const id = setInterval(() => {
//   console.log('after a while...');
//   if (i++ > 10) clearInterval(id);
// }, 1000);

// lept.pix.destroy(pix);
// lept.pix.destroy(pix2);

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
