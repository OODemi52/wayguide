# WayGuide IPC Node

A Node.js bridge application that enables communication between web applications and serial devices (like Arduino) via WebSockets.

## Features

- Serial port communication with hardware devices
- WebSocket server for real-time bidirectional communication
- Automatic serial port detection and listing
- JSON-based message passing
- Error handling and reporting

## Installation

### Prerequisites

- Node.js (v12 or later)
- npm or yarn
- Arduino or other serial-compatible device

### Setup

1. Clone the repository:
   ```
   git clone https://github.com/yourusername/wayguide.git
   cd wayguide/ipc-node
   ```

2. Install dependencies:
   ```
   npm install
   ```

## Configuration

### Serial Port

By default, the application tries to connect to `/dev/tty.usbmodem1101`. You can modify this in `index.js`:

```javascript
port = new SerialPort({ 
  path: '/dev/tty.usbmodem1101', // Change this to your device's path
  baudRate: 112500 
});
```

> **Note:** The application lists all available serial ports when starting up to help you identify the correct port.

### Server Settings

The WebSocket server runs on port 8080 by default. You can change this by setting the `PORT` environment variable.

## Usage

### Starting the Server

```
node index.js
```

The server will:
1. List all available serial ports
2. Try to connect to the configured serial port
3. Start the WebSocket server when the serial connection is established

### Connecting via WebSocket

Connect to the WebSocket server at:
```
ws://your-server-ip:8080/ws
```

### Sending Messages to the Serial Device

Send a message via WebSocket. The message will be forwarded to the serial device.

Example with JavaScript:
```javascript
const ws = new WebSocket('ws://your-server-ip:8080/ws');

ws.onopen = () => {
  ws.send('YOUR_COMMAND');
};

ws.onmessage = (event) => {
  const response = JSON.parse(event.data);
  console.log(response);
};
```

### Response Format

Successful operations return:
```json
{
  "success": true,
  "message": "Message sent to device"
}
```

Error responses:
```json
{
  "error": "Error message"
}
```

## Troubleshooting

### Serial Port Issues

- Make sure your device is properly connected
- Check that you have the correct port path in the configuration
- Verify the baud rate matches your device's configuration
- You may need to run the server with elevated permissions to access certain serial ports

### WebSocket Connection Issues

- Verify the server is running and the correct IP/port is being used
- Check for firewall settings that might block WebSocket connections
- Ensure the client is connecting with the correct path (`/ws`)

## License

MIT
