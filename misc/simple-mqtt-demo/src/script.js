/*
   Eclipse Paho MQTT-JS Utility
   by Elliot Williams for Hackaday article, modified by Drew Harwell
*/

// Global variables
var client;

// These are configs	
var hostname       = "test.mosquitto.org";
var port           = "8081";
var led_topic      = "led1";
var clientId;

//create client ID or load from storage
if ( window.sessionStorage.clientId){
  clientId = window.sessionStorage.clientId;
}else{
  clientId = "mqtt_js_" + parseInt(Math.random() * 100000, 10);
  window.sessionStorage.clientId = clientId;
}

// This is called after the webpage is completely loaded
// It is the main entry point into the JS code
function connect(){
	// Set up the client
	client = new Paho.MQTT.Client(hostname, Number(port), clientId);
	console.info('Connecting to Server: Hostname: ', hostname, '. Port: ', port, '. Client ID: ', clientId);

	// set callback handlers
	client.onConnectionLost = onConnectionLost;
	client.onMessageArrived = onMessageArrived;

	// see client class docs for all the options
	var options = {
		onSuccess: onConnect, // after connected, subscribes
		onFailure: onFail,    // useful for logging / debugging
    useSSL:true
	};
	// connect the client
	client.connect(options);
	console.info('Connecting...');
}


function onConnect(context) {
	console.log("Client Connected");
  document.getElementById('connect_button').innerHTML = "Connected!";
  document.getElementById('connect_button').disabled = true;
  document.getElementById('on_button').disabled = false;
  document.getElementById('off_button').disabled = false;

  
    // And subscribe to our topics	-- all with the same callback function
	options = {qos:0, onSuccess:function(context){ console.log("subscribed"); } }
	client.subscribe(led_topic, options);

}

function onFail(context) {
	console.log("Failed to connect");
}

function onConnectionLost(responseObject) {
	if (responseObject.errorCode !== 0) {
		console.log("Connection Lost: " + responseObject.errorMessage);
		window.alert("Someone else took my websocket!\nRefresh to take it back.");
	}
}

// Here are the two main actions that drive the webpage:
//  handling incoming messages and the toggle button.

// Updates the webpage elements with new data, and 
//  tracks the display LED status as well, 
//  in case multiple clients are toggling it.
function onMessageArrived(message) {
	console.log('got message to topic:',message.destinationName, 'message:', message.payloadString);
  if (message.destinationName == led_topic){
    console.log('got LED message');
  }
}

// Provides the button logic that toggles our display LED on and off
// Triggered by pressing the HTML button "status_button"
function led_on(){
  var payload = "1";
  led_is_on = true;

	// Send messgae
	message = new Paho.MQTT.Message(payload);
	message.destinationName = led_topic;
	message.retained = true;
	client.send(message);
	console.info('sending: ', payload);
}

// Provides the button logic that toggles our display LED on and off
// Triggered by pressing the HTML button "status_button"
function led_off(){
  var payload = "0";
  led_is_on = false;

	// Send messgae
	message = new Paho.MQTT.Message(payload);
	message.destinationName = led_topic;
	message.retained = true;
	client.send(message);
	console.info('sending: ', payload);
}