const path = require('path');
const express = require('express');
const WebSocket = require('ws');
const app = express();
const bodyParser = require('body-parser');
//const { spawnSync } = require('child_process');

app.use(bodyParser.json());
// Function to serve all static files
// inside public directory.
app.use(express.static(__dirname+'/public'));

const WS_PORT  = 8888;
const esp32_port = 8887;
const HTTP_PORT = 8000;

const wsServer = new WebSocket.Server({port: WS_PORT}, ()=> console.log(`WS Server is listening at ${WS_PORT}`)); //for create new websocket for connect wha ever you want by in this code this lines use for esp32-cam(camera)
const esp32 = new WebSocket.Server({port: esp32_port}, ()=> console.log(`esp32_port Server is listening at ${esp32_port}`)); //for create new websocket for connect wha ever you want by in this code this lines use for esp32 devkitv1(for control relay)

let connectedClients = [];
//esp32-cam
wsServer.on('connection', (ws, req)=>{
    connectedClients.push(ws);
    ws.on('message', data => {
        connectedClients.forEach((ws,i)=>{
            if(ws.readyState === ws.OPEN){
                console.log('ESP32-CAM can Connected');
                ws.send(data);
            }else{
                connectedClients.splice(i ,1);
            }
        })
    });
});
//devkitv1
esp32.on('connection', (ws) => {
    console.log('ESP32 Devkitv1 can connected');

    // Send a message to the ESP32 when it connects
    ws.send('Hello ESP32!');
    // Endpoint to receive data from HTML file
    app.post('/showButton', (req, res) => {
        const dataFromHTML = req.body;
        console.log('Data received from HTML:', dataFromHTML);
        // Forward the data to ESP32 via WebSocket
        if (ws.readyState === ws.OPEN) {
            ws.send(JSON.stringify(dataFromHTML));
        }
    });
    // Listen for messages from the client
    ws.on('message', (message) => {
        console.log(`Received message from client: ${message}`);
    });
});

app.get('/client',(req,res)=>res.sendFile(path.resolve(__dirname, './client.html')));
app.listen(HTTP_PORT, ()=> console.log(`HTTP server listening at ${HTTP_PORT}`));
