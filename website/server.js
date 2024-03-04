const path = require('path');
const express = require('express');
const WebSocket = require('ws');
const app = express();
const bodyParser = require('body-parser');
const ejs = require('ejs');
const date = require('date-and-time');
const { stat } = require('fs');

app.use(bodyParser.json());
// Function to serve all static files
// inside public directory.
app.use(express.static(__dirname+'/public'));
app.set('view engine', 'ejs');

const now = new Date();
const formattedDate = now.toLocaleString('en-US', {
    month: 'numeric',
    day: 'numeric',
    year: 'numeric',
    hour: 'numeric',
    minute: 'numeric',
    second: 'numeric',
    hour12: true,
});
app.locals.values = formattedDate;
let times = ''; //current time.
let holdtime = times; // time passed 10 seconds ago.
let current_message = ''; //for store message as received from esp32-devkitv1 in current
let store_message_past = current_message; // for store last message from current_message
const WS_PORT  = 8888;
const esp32_port = 8887;
const HTTP_PORT = 8000;
const wsServer = new WebSocket.Server({port: WS_PORT}, ()=> console.log(`WS Server is listening at ${WS_PORT}`)); //for create new websocket for connect what ever you want by in this code this lines use for esp32-cam(camera)
const esp32 = new WebSocket.Server({port: esp32_port}, ()=> console.log(`esp32_port Server is listening at ${esp32_port}`)); //for create new websocket for connect wha ever you want by in this code this lines use for esp32 devkitv1(for control relay)

let connectedClients = [];//keep buffer as receive from ESP32-Cam via websocket in array.
//esp32-cam
wsServer.on('connection', (ws_cam, req)=>{
    connectedClients.push(ws_cam);
    ws_cam.on('message', data => {
        // Assuming data is in a format that OpenCV.js can handle
        // You might need to adjust this based on your data structure
        const imageBuffer = Buffer.from(data, 'base64');
        const mat = cv.imdecode(imageBuffer);
        // Perform OpenCV processing on the 'mat' object as needed
        // Convert the processed image back to base64
        const processedImageBuffer = cv.imencode('.jpg', mat).toString('base64');
        connectedClients.forEach((ws_cam,i)=>{
            if(ws_cam.readyState === ws_cam.OPEN){
                ws_cam.send(processedImageBuffer);//using for send frames picture from ESP32-CAM via websocket to client.ejs files.
            }else{
                connectedClients.splice(i ,1);//using for destroy frame when connection between esp32-cam and Node.JS not connect via websocket.
            }
        })
    });
});

//devkitv1 
esp32.on('connection', (ws) => {
    // Send a message to the ESP32 when it connects in the first time connection after 3-ways handshake(TCP).
    ws.send('Hello ESP32!');
    // Endpoint to receive data from ejs file for control type of light as wanna open.
    app.post('/showButton', async(req, res) => {
        const dataFromHTML = req.body;
        console.log('Data received from HTML:', dataFromHTML);
        // Forward the data to ESP32 via WebSocket when The Socket already.
        if (ws.readyState === ws.OPEN) {
            ws.send(JSON.stringify(dataFromHTML));
        }
    });
    //using for refreshtime and send message to end-device in this part is ESP32-DekitV1.
    app.post('/refreshtime', (req, res) => {
        const time = req.body.datetime;
        console.log('Time :', time);
        //time
        times = time;
        if (ws && ws.readyState === WebSocket.OPEN) {
            const message = 'Reconnect';
            ws.send(JSON.stringify({ data: message }));
            console.log(`Sent message to ESP32: ${message} ${times}`);
            app.locals.dataFromESP32 = "ESP32-DevkitV1 are Connected";
        } else {
            console.log('No ESP32 connection or server not connect same wifi ESP32-Devkitv1');
            app.locals.dataFromESP32 = "ESP32-DevkitV1 Not Connect Now"; 
        }
    });
    // Listen for messages from the client
    ws.on('message', (message) => {
        current_message += message;
        // Forward the data to HTML via HTTP endpoint
        if (message){
            console.log(`Received message from client: ${message} ${times}`);//using first time when start service so use time in node.js for change webpage
            app.locals.dataFromESP32 = "ESP32-DevkitV1 are Connected Now";
        }
        else{
            console.log('Data from ESP32_Devkitv1 can not send back to node.js');
            app.locals.dataFromESP32 = "ESP32-DevkitV1 Not Connect Now";
        }
    });
});
//app.get('/client',(req,res)=>res.sendFile(path.resolve(__dirname, './client.html')));
// Render the 'index.ejs' template with dynamic data 
app.get('/',(req,res)=>res.render('client', { dataFromESP32: app.locals.dataFromESP32, values: app.locals.values}));//using for dynamic template webpage via change data from Node.JS
app.listen(HTTP_PORT, ()=> console.log(`HTTP server listening at ${HTTP_PORT}`));
