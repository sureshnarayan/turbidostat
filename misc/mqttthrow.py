#!/usr/bin/env python3

import paho.mqtt.client as mqtt
import json
import serial
import time
import datetime
import csv
import numpy as np

ser = serial.Serial('/dev/ttyACM0')
ser.flushInput()

def on_connect(client, userdata, flags, rc):
    if rc==0:
        client.connected_flag=True #set flag
        print("Connected with return code=",rc)
        client.subscribe("turbidostat/command")
    else:
        print("Bad connection Returned code= ",rc)

def on_message(client, userdata, msg):
  print(msg.payload.decode())

def on_disconnect(client, userdata, rc):
    print("disconnecting reason  "  +str(rc))
    client.connected_flag=False
    client.disconnect_flag=True

client = mqtt.Client()
client.connected_flag=False
client.bad_connection_flag=False
broker = "localhost"
# broker = "172.16.100.68"
port = 1883
client.on_connect = on_connect
client.on_message = on_message
client.loop_start()
print("Connecting to broker ",broker)
try:
    client.connect(broker,port) #connect to broker
except Exception as e:
    raise
    print("Could not connect")

while not client.connected_flag and not client.bad_connection_flag: #wait in loop
    print("In wait loop")
    time.sleep(1)
if client.bad_connection_flag:
    client.loop_stop()    #Stop loop



dateTimeObj = datetime.datetime.now()
filename = "log/log_"+ dateTimeObj.strftime("%d%b_%H%M%S") +".csv"

while True:
    try:
        ser_bytes = ser.readline()
        decoded_bytes = ser_bytes[0:len(ser_bytes)-2].decode("utf-8")
        try:
            data = [float(i) for i in (decoded_bytes.split(","))]
            logData = [round(time.time(),3)] + data
            print(logData)
            try:
                client.publish("turbidostat/log", json.dumps(logData))
            except:
                raise

            with open(filename,"a") as f:
                writer = csv.writer(f,delimiter=",")
                writer.writerow(logData)
        except:
            continue

    except:
        print("Keyboard Interrupt")
        ser.close()
        client.loop_stop()    #Stop loop 
        client.disconnect()
        raise
        break
