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

  const p2 = pix.thresh(p1, 128);
  pix.save(p2, 'img/s-030-01-thresh.png', f.png);

  const p3 = pix.morph(p2, 'c150.5');
  pix.save(p3, 'img/s-030-02-morph.png', f.png);

  const b1 = pix.connComp(p3);
  console.log(b1.length, b1.ptr, b1[0]);
  const b2 = box.aget(b1, 2);
  console.log(b2);
  box.destroy(b2);
  console.log(b2);

  box.destroy(b1);
  console.log(b1.length, b1.ptr, b1[0]);

  pix.destroy(p1);
  console.log('s-030.ppm', p1);
  pix.destroy(p2);
  pix.destroy(p3);
  console.log('end');
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
