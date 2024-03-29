const path = require('path');
const express = require('express');
const WebSocket = require('ws');
const app = express();
const bodyParser = require('body-parser');
const ejs = require('ejs');

const axios = require('axios');
const cors = require('cors');

app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());
// Function to serve all static files
// inside public directory.
app.use(express.static(__dirname+'/public'));
app.set('view engine', 'ejs');
app.get('/',(req,res)=>res.render('index'));
app.use(cors());

const HTTP_PORT = 5000;
const esp32_port = 8887;
const esp32 = new WebSocket.Server({port: esp32_port}, ()=> console.log(`esp32_port Server is listening at ${esp32_port}`)); //for create new websocket for connect wha ever you want by in this code this lines use for esp32 devkitv1(for control relay)
let data_for_send_to_esp32 = "";
let state = '';
app.use(cors());
let current_message = [];
//video feed after detect
app.get('/get_color', async (req, res) => {
    try {
        const response = await axios.get('http://127.0.0.1:5001/api/get_color');
        // Add data to current_message array
        if (response.data.class_name_human){
            current_message.push(response.data.class_name_human);
            res.json(response.data);
        }
    } catch (error) {
        //console.error(error.message);
        //res.status(500).send('Internal Server Error');
    }
});

//state
app.post('/state_to_order', async(req, res) => {
    state = req.body.mode;
});

// Function to calculate the most frequent value in an array
function findMostFrequentValue(current_message) {
    // Check if arr is an array
    if (Array.isArray(current_message)) {
        const counts = {};
        current_message.forEach(value => {
            counts[value] = (counts[value] || 0) + 1;
        });
        let maxCount = 0;
        let mostFrequentValue = null;
        for (const value in counts) {
            if (counts[value] > maxCount) {
                maxCount = counts[value];
                mostFrequentValue = value;
            }
        }
        return mostFrequentValue;
    }
    else{
        console.error('Error: Input is not an array');
        return null; // Return null or handle the error as needed
    }
}

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
    // Timer to calculate most frequent value every 10 seconds
    setInterval(() => {
        console.log(state);
        if (current_message.length > 0) {
            // Calculate the most frequent classname
            const mostFrequentClassname = findMostFrequentValue(current_message);
            console.log(mostFrequentClassname);
            if (mostFrequentClassname) {
                // Send the most frequent classname to esp32-devkitv1
                data_for_send_to_esp32 = mostFrequentClassname;
                //Ensure data_for_send_to_esp32 is not empty or invalid before sending
                if (data_for_send_to_esp32 && data_for_send_to_esp32.length > 0) {
                    if (state == "auto"){
                        ws.send(JSON.stringify(data_for_send_to_esp32));
                        console.log('Most frequent classname:', data_for_send_to_esp32);
                    }
                    else{
                        console.log('Now mode is manual.');
                    }
                    data_for_send_to_esp32 = "";
                } else {
                    console.error('Error: data_for_send_to_esp32 is empty or invalid');
                }
            }
        } else {
            console.log('No data available to calculate most frequent classname');
        }
        // Clear the current_message array
        current_message = [];
    }, 10000); // 10 seconds interval
    // Listen for messages from the client
    ws.on('message', (message) => {
        // Forward the data to HTML via HTTP endpoint
        if (message){
            console.log(`Received message from client: ${message}`);//using first time when start service so use time in node.js for change webpage
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
//app.get('/',(req,res)=>res.render('index', { dataFromESP32: app.locals.dataFromESP32, values: app.locals.values}));
app.listen(HTTP_PORT, ()=> console.log(`HTTP server listening at ${HTTP_PORT}`));
