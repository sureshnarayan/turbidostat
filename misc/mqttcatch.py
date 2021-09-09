#!/usr/bin/env python3

import paho.mqtt.client as mqtt
import time
import datetime
import csv
import matplotlib
matplotlib.use("tkAgg")
import matplotlib.pyplot as plt
import numpy as np
import json
from pandas import date_range
import matplotlib.dates as mdates
import utilsOD

a_laser = 900

# This is the Subscriber
def on_connect(client, userdata, flags, rc):
    if rc==0:
        client.connected_flag=True #set flag
        print("Connected with return code=",rc)
        client.subscribe("turbidostat/log")
    else:
        print("Bad connection Returned code= ",rc)

def on_message(client, userdata, message):
    decoded_message=str(message.payload.decode("utf-8"))
    msg=json.loads(decoded_message)
    # print(msg)
    client.buffer.append(msg)


def on_disconnect(client, userdata, rc):
    print("disconnecting reason  "  +str(rc))
    client.connected_flag=False
    client.disconnect_flag=True

client = mqtt.Client()#(transport="websockets")
client.connected_flag=False
client.bad_connection_flag=False
client.buffer = []
# broker = "172.16.100.68"
broker = "localhost"
port = 1883
client.on_connect = on_connect
client.on_message = on_message
client.loop_start()
print("Connecting to broker ",broker)
try:
    client.connect(broker,port) #connect to broker
except:
    print("Could not connect")

while not client.connected_flag and not client.bad_connection_flag: #wait in loop
    print("In wait loop")
    time.sleep(1)
if client.bad_connection_flag:
    client.loop_stop()    #Stop loop


today = datetime.datetime.now() 
yesterday = today - datetime.timedelta(days = 1)

plot_window = 10000
# x_var = np.arange(plot_window, dtype=float)
x_var = date_range(start=yesterday, end=today, periods=plot_window).to_pydatetime().tolist()
y_var = np.zeros(plot_window)
y_var2 = np.zeros(plot_window)
index = 1


numReadings = 100
queue = [0]*numReadings

# x_var = np.vectorize(datetime.datetime.fromtimestamp)(x_var)
plt.xlabel('time')
plt.gcf().autofmt_xdate()
myFmt = mdates.DateFormatter('%H:%M')
plt.gca().xaxis.set_major_formatter(myFmt)


plt.ion()
fig, ax = plt.subplots()
line, = ax.plot(x_var, y_var, '.')
line2, =ax.plot(x_var, y_var2, '.')
ax.set_ylim([0,5])
plt.grid()
# plt.yscale("log")

initial_time = time.time()
while True:
    try:
        if len(client.buffer):
            newData = client.buffer
            print(newData)
            client.buffer = []
        else:
            continue

        for data in newData:
            plotData = [float(i) for i in data]
            #If averaging
            #Processing - averaging/low pass filtering
            plotData = utilsOD.getOD(data[1],a_laser)
            queue.pop(0)
            queue.append(plotData)
            # x_var = np.append(x_var, (time.time() - initial_time))
            x_var = np.append(x_var, datetime.datetime.now())
            y_var = np.append(y_var, plotData)
            y_var2 = np.append(y_var2, sum(queue)/numReadings)
            x_var = x_var[1:plot_window+1]
            y_var = y_var[1:plot_window+1]
            y_var2 = y_var2[1:plot_window+1]
        line.set_xdata(x_var)
        line.set_ydata(y_var)
        line2.set_xdata(x_var)
        line2.set_ydata(y_var2)
        ax.relim()
        # ax.set_ylim([0,1023])
        # ax.autoscale_view()
        fig.canvas.draw()
        fig.canvas.flush_events()

    except Exception as e:
        client.disconnect()
        client.loop_stop()
        print(e)
        raise
        break
        #continue

