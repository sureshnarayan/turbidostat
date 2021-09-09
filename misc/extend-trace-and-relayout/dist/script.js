const clientId = 'mqttjs_' + Math.random().toString(16).substr(2, 8);
const host = 'ws://172.16.100.68:8080';
const topic = 'turbidostat/log';
var incomingData = [];
var newMessageFlag = false
const options = {
  keepalive: 60,
  clientId: clientId,
  protocolId: 'MQTT',
  protocolVersion: 4,
  clean: true,
  reconnectPeriod: 1000,
  connectTimeout: 30 * 1000,
  will: {
    topic: 'WillMsg',
    payload: 'Connection Closed abnormally..!',
    qos: 0,
    retain: false
  },
}
console.log('Connecting mqtt client')
const client = mqtt.connect(host, options)
client.on('error', (err) => {
  console.log('Connection error: ', err)
  client.end()
})
client.on('reconnect', () => {
  console.log('Reconnecting...')
})
client.on('connect', () => {
  console.log('Client connected:' + clientId)
  // Subscribe
  client.subscribe( topic, { qos: 0 })
})
// Unsubscribe
// client.unubscribe(topic, () => {
//   console.log('Unsubscribed')
// })
// Publish
client.publish(topic, 'ws connection demo...!', { qos: 0, retain: false })
// Received
client.on('message', (topic, message, packet) => {
  console.log('Re:'  + topic + ': ' + message.toString())
  if(topic == 'turbidostat/log'){
    incomingData.push(JSON.parse(message))
    newMessageFlag = true
  }
})





function rand() {
  return 5 * Math.random();
}

function getCol(matrix, col){
  var column = [];
  for(var i=0; i<matrix.length; i++){
     column.push(matrix[i][col]);
  }
  return column; // return column data..
}

var itime = new Date();

var data = [{
  x: [itime], 
  y: [rand],
  type: 'scattergl',
  mode:'marker'
}]

Plotly.newPlot('graph', data);  

var cnt = 0;

var interval = setInterval(function() {
  
  if(newMessageFlag){ 
    var time = getCol(incomingData,0).map(unix => new Date(unix * 1000));
    var ydata = getCol(incomingData,1)
    
    var update = {
    x: [time],
    y: [ydata]
    }

    var updateData = [{
      x: time,
      y: ydata,
      type: 'scatter',
      mode:'marker'
    }]
    
    //var olderTime = new Date(time[0]).setMinutes(time[0].getMinutes() - 1);
    //var futureTime = new Date(time[time.length - 1]).setMinutes(time[time.length - 1].getMinutes() + 1);
    
    // var minuteView = {
    //       xaxis: {
    //         type: 'date',
    //         range: [olderTime,futureTime]
    //       }
    //     };
    
    console.log(update);
    Plotly.extendTraces('graph', update, [0], time.length);
    // incomingData = [];
    newMessageFlag = false;
    //Plotly.relayout('graph', minuteView);
    //Plotly.react('graph', updateData);
    
    // incomingData.splice(0,1);
  }
  
  // if(++cnt === 100) clearInterval(interval);
}, 100);