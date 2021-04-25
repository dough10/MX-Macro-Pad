const fs = require( 'fs' );
const uglifyJS = require('uglify-es');
const uglifycss = require('uglifycss');
const minify = require('html-minifier').minify;
const colors = require('colors');


const files = [
  'keys.json',
  'modifiers.json',
  'css/paper-ripple.min.css',
  'js/paper-ripple.min.js',
  'fonts/roboto-v15-latin-regular.woff2',
  'fonts/roboto-v15-latin-regular.woff',
  'fonts/roboto-v15-latin-regular.ttf'
];


function copyFile(filePath) {
  console.log(`moving ${filePath.green} to HTML folder`);
  fs.createReadStream(`./src/${filePath}`).pipe(fs.createWriteStream(`./html/${filePath}`));
}


function minifyHTML() {
  return new Promise((resolve, reject) => {
    console.log('Concatinating/Minifying HTML'.yellow);
    fs.readFile('./src/index.html', 'utf8', (err, html) => {
      if (err) {
        reject(err);
        return;
      }
      const smallHTML = minify(html, {
        removeAttributeQuotes: true,
        collapseWhitespace: true,
        minifyCSS: true,
        minifyURLs: true,
        removeComments: true
      });
      console.log('/html/index.html'.green, 'has been saved');
      fs.writeFile( './html/index.html', smallHTML, resolve);
    });
  });
}


function uglifyJavaScript() {
  return new Promise((resolve, reject) => {
    console.log('Concatinating/Minifying Javascript'.yellow);
    fs.readFile('./src/js/app.js', 'utf8', (err, js) => {
      if (err) {
        reject(err);
        return;
      }
      const uglyCode = uglifyJS.minify(js);
      if (uglyCode.error) {
        console.log(uglyCode.error);
        return;
      }
      console.log('/html/js/app.js'.green, 'has been saved');
      fs.writeFile( './html/js/app.js', uglyCode.code, resolve);
    });
  });
}


function uglyCSS() {
  return new Promise(resolve => {
    console.log('Concatinating/Minifying Stylesheets'.yellow);
    const uglified = uglifycss.processFiles(
        [
          './src/css/base.css'
        ],
        {
          maxLineLen: 500,
          expandconsts: true
        }
    );
    console.log('/html/css/base.css'.green, 'has been saved');
    fs.writeFile( './html/css/base.css', uglified, resolve);
  });
}


uglifyJavaScript()
.then(uglyCSS)
.then(minifyHTML)
.then(_ => files.forEach(copyFile));
