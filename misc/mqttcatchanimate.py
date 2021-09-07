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
from matplotlib.animation import FuncAnimation
import collections
import psutil

# This is the Subscriber
def on_connect(client, userdata, flags, rc):
    if rc==0:
        client.connected_flag=True #set flag
        print("Connected with return code ",rc)
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

client = mqtt.Client()
client.connected_flag=False
client.bad_connection_flag=False
client.buffer = []
broker = "172.16.100.68"
# broker = "localhost"
port = 1883
client.connect(broker,port,60)
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

"""
# function to update the data
def my_function(i):
    # get data
    cpu.popleft()
    cpu.append(psutil.cpu_percent())
    ram.popleft()
    ram.append(psutil.virtual_memory().percent)
    # clear axis
    ax.cla()
    ax1.cla()
    # plot cpu
    ax.plot(cpu)
    ax.scatter(len(cpu)-1, cpu[-1])
    ax.text(len(cpu)-1, cpu[-1]+2, "{}%".format(cpu[-1]))
    #ax.set_ylim(0,100)
    # plot memory
    ax1.plot(ram)
    ax1.scatter(len(ram)-1, ram[-1])
    ax1.text(len(ram)-1, ram[-1]+2, "{}%".format(ram[-1]))
    #ax1.set_ylim(0,100)
# start collections with zeros
plot_window = 10000
cpu = collections.deque(np.zeros(plot_window))
ram = collections.deque(np.zeros(plot_window))
# define and adjust figure
fig = plt.figure(figsize=(12,6), facecolor='#DEDEDE')
ax = plt.subplot(121)
ax1 = plt.subplot(122)
ax.set_facecolor('#DEDEDE')
ax1.set_facecolor('#DEDEDE')
try:
    # animate
    ani = FuncAnimation(fig, my_function)
    plt.show()
except Exception as e:
    client.disconnect()
    client.loop_stop()
    print(e)
    raise
    #continue

"""

plot_window = 10000
x_var = collections.deque(np.arange(plot_window, dtype=float))
y_var = collections.deque(np.zeros(plot_window))
y_var2 = collections.deque(np.zeros(plot_window))
index = 1


numReadings = 100
queue = [0]*numReadings

plt.ion()
fig, ax = plt.subplots()
line, = ax.plot(x_var, y_var, '.')
line2, =ax.plot(x_var, y_var2, '.')
ax.set_ylim([-10,10])
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
            plotData = data[1]
            queue.pop(0)
            queue.append(plotData)
            x_var.popleft()
            y_var.popleft()
            y_var2.popleft()
            x_var.append((time.time() - initial_time))
            y_var.append(np.log2(plotData))
            y_var2.append(np.log2(sum(queue)/numReadings))
        #line.set_xdata(x_var)
        line.set_ydata(y_var)
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

