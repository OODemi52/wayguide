const express = require('express');
const { SerialPort } = require('serialport');
const WebSocket = require('ws');

const app = express();
let port;

// List available ports first
SerialPort.list().then(ports => {
  console.log('Available ports:');
  ports.forEach(port => {
    console.log(` - ${port.path}`);
  });

  // Try to open the serial port
  try {
    port = new SerialPort({ 
      path: '/dev/tty.usbmodem1101', // Changed to the available port
      baudRate: 112500 
    });

    port.on('open', () => {
      console.log('Serial port opened successfully');
      
      // Only start WebSocket server after port is successfully opened
      const wss = new WebSocket.Server({ port: 8080 });

      wss.on('connection', ws => {
        console.log('New WebSocket connection');
        ws.on('message', message => {
          console.log('received: %s', message);
          port.write(message, (err) => {
            if (err) {
              console.log('Error on write: ', err.message);
              ws.send('Error: Failed to send message to device');
            } else {
              console.log('message written to Arduino');
              ws.send('Message sent to device');
            }
          });
        });
      });

      console.log('WebSocket server started at ws://localhost:8080');
    });

    port.on('error', (err) => {
      console.error('Serial port error:', err.message);
    });

  } catch (err) {
    console.error('Failed to open serial port:', err.message);
  }
}).catch(err => {
  console.error('Error listing ports:', err.message);
});
