var fs = require( 'fs' );
var uglifyJS = require('uglify-es');
var uglifycss = require('uglifycss');
var minify = require('html-minifier').minify;


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
  fs.createReadStream(`./src/${filePath}`).pipe(fs.createWriteStream(`./html/${filePath}`));
}


function minifyHTML() {
  return new Promise((resolve, reject) => {
    fs.readFile('./src/index.html', 'utf8', (err, html) => {
      if (err) {
        reject(err);
        return;
      }
      var smallHTML = minify(html, {
        removeAttributeQuotes: true,
        collapseWhitespace: true,
        minifyCSS: true,
        minifyURLs: true,
        removeComments: true
      });
      fs.writeFile( './html/index.html', smallHTML, resolve);
    });
  });
}


function uglifyJavaScript() {
  return new Promise((resolve, reject) => {
    fs.readFile('./src/js/app.js', 'utf8', (err, js) => {
      if (err) {
        reject(err);
        return;
      }
      var uglyCode = uglifyJS.minify(js);
      if (uglyCode.error) {
        console.log(uglyCode.error);
        return;
      }
      fs.writeFile( './html/js/app.js', uglyCode.code, resolve);
    });
  });
}


function uglyCSS() {
  return new Promise(resolve => {
    var uglified = uglifycss.processFiles(
        [
          './src/css/base.css'
        ],
        {
          maxLineLen: 500,
          expandVars: true
        }
    );
    fs.writeFile( './html/css/base.css', uglified, _ => {
      resolve();
    });
  });
}


uglifyJavaScript()
.then(uglyCSS)
.then(minifyHTML)
.then(_ => files.forEach(copyFile));
