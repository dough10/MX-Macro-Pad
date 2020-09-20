
const {app, BrowserWindow} = require('electron');
const SerialPort = require('serialport');
const ipc = require('electron').ipcMain;

/*--
Create the browser window sto reneder that app
 --*/
function createWindow () {
  // properties of the browser window.
  const win = new BrowserWindow({
    width: 568,
    height: 320,
    resizable: false,
    webPreferences: {
      worldSafeExecuteJavaScript: true,
      nodeIntegration: true
    }
  })
  win.removeMenu();
  win.loadFile('src/index.html');
  let contents = win.webContents
  contents.openDevTools()
  /*-- gets a array of avaliable COM ports and sends to app.js--*/
  getPortsList().then((ports) => {
    setTimeout(() => {
      contents.send('data', ports)
    }, 1000)
  });
  /*-- selecting port for serial --*/
  ipc.on('selectPort', (event, portNum) => {
    const port = new SerialPort(portNum, { baudRate: 9600 });
    const Readline = require('@serialport/parser-readline');
    const parser = port.pipe(new Readline({ delimiter: '\n' }));
    port.on('close', _ => {
      app.quit();
    });
    port.on("open", _ => {
      event.sender.send('portOpen', portNum);
      ipc.on('selectButton', (e, button) => {
        port.write(button);
      });
      ipc.on('conError', (e, error) => {
        app.quit();
      });
    });
    // forward data to UI (app.js)
    parser.on('data', data =>{
      contents.send('data', data);
    });
  });

}

app.whenReady().then(createWindow);
app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit()
  }
});
app.on('activate', () => {
  if (BrowserWindow.getAllWindows().length === 0) {
    createWindow()
  }
});


function getPortsList () {
  return new Promise((resolve, reject) => {
    var portsList = [];
    SerialPort.list().then((ports) => {
      ports.forEach((port) => {
        portsList.push(port.path);
      });
      resolve(portsList);
    });
  });
}
