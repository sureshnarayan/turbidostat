#!/usr/bin/env python3

import paho.mqtt.client as mqtt
import json
import serial
import time
import datetime
import csv
import numpy as np
import utilsOD

a_laser = 950

ser = serial.Serial('/dev/ttyACM0')
ser.flushInput()

def on_connect(client, userdata, flags, rc):
    if rc==0:
        client.connected_flag=True #set flag
        print("Connected with return code=",rc)
        client.subscribe("turbidostat/command")
    else:
        print("Bad connection Returned code= ",rc)

def on_message(client, userdata, message):
    # msg=json.loads(str(message.payload.decode("utf-8")))
    msg=str(message.payload.decode("utf-8"))
    ser.writeline(msg.encode("utf-8"))
    print(msg)


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
#filename = "log/"+ dateTimeObj.strftime("%d%b_%H%M%S")

while True:
    filename = "log/"+ dateTimeObj.strftime("%d%b")
    try:
        ser_bytes = ser.readline()
        decoded_bytes = ser_bytes[0:len(ser_bytes)-2].decode("utf-8")
        strings = decoded_bytes.split(",")
        if strings[0] == "LOG":
            currentTime = round(time.time(),3)
            data = [float(i) for i in strings[1:]]
            logData = [currentTime] + data
            ODData = np.around(utilsOD.getOD(np.array(data),a_laser),3)
            logODData = [currentTime] + ODData.tolist()
            print(logData)

            with open(filename + "_sensor.csv","a") as f:
                writer = csv.writer(f,delimiter=",")
                writer.writerow(logData)
            with open(filename + "_OD.csv","a") as f:
                writer = csv.writer(f,delimiter=",")
                writer.writerow(logODData)
            
            try:
                client.publish("turbidostat/log/sensor", json.dumps(logData))
            except:
                continue
        
        elif strings[0] == "LOOPLOG":
            logtype = strings[0]
            #data = [float(i) for i in strings[1:]]
            logData = [logtype] + [str(round(time.time(),3))] + strings[1:]
            print(logData)
            with open(filename + "_log.csv","a") as f:
                writer = csv.writer(f,delimiter=",")
                writer.writerow(logData)

            try:
                client.publish("turbidostat/log/loopinfo", json.dumps(logData))
            except:
                continue
        
        else:
            print(strings)
            try:
                client.publish("turbidostat/status", json.dumps(strings))
            except:
                continue
        

    except:
        print("Keyboard Interrupt")
        ser.close()
        client.loop_stop()    #Stop loop 
        client.disconnect()
        raise
        break
