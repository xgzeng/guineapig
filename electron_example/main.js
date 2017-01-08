var app = require('app');
var BrowserWindow = require('browser-window');

var mainWindow = null;
app.on('ready', function() {
  mainWindow = new BrowserWindow({width: 800, height: 600});
  var url = 'file://' + __dirname + '/index.html';
  console.log(url);
  mainWindow.loadUrl(url);
  mainWindow.on('closed', function() {
    mainWindow = null;
  })
});


