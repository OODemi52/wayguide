const express = require('express');
const { SerialPort } = require('serialport');
const WebSocket = require('ws');
const http = require('http');

const app = express();
const server = http.createServer(app);
let port;

// Create WebSocket server attached to HTTP server
const wss = new WebSocket.Server({ 
  server,
  path: '/ws' // Explicit WebSocket path
});

// List available ports first
SerialPort.list().then(ports => {
  console.log('Available ports:');
  ports.forEach(port => {
    console.log(` - ${port.path}`);
  });

  // Try to open the serial port
  try {
    port = new SerialPort({ 
      path: '/dev/tty.usbmodem1101',
      baudRate: 112500 
    });

    port.on('open', () => {
      console.log('Serial port opened successfully');
      
      wss.on('connection', (ws, req) => {
        console.log(`New WebSocket connection from ${req.socket.remoteAddress}`);
        
        ws.on('message', message => {
          try {
            console.log('received: %s', message);
            port.write(message, (err) => {
              if (err) {
                console.log('Error on write: ', err.message);
                ws.send(JSON.stringify({ error: 'Failed to send message to device' }));
              } else {
                console.log('Message written to Arduino');
                ws.send(JSON.stringify({ success: true, message: 'Message sent to device' }));
              }
            });
          } catch (error) {
            console.error('WebSocket message handling error:', error);
            ws.send(JSON.stringify({ error: 'Internal server error' }));
          }
        });

        ws.on('error', error => {
          console.error('WebSocket error:', error);
        });

        ws.on('close', () => {
          console.log('Client disconnected');
        });
      });

      // Start the server
      const PORT = process.env.PORT || 8080;
      server.listen(PORT, () => {
        console.log(`Server is running on port ${PORT}`);
        console.log(`WebSocket server available at ws://172.20.10.3:${PORT}/ws`);
      });
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