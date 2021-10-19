#!/usr/bin/env python3

import paho.mqtt.client as mqtt
import time
import datetime
import csv, sys
import matplotlib
matplotlib.use("tkAgg")
import matplotlib.pyplot as plt
import numpy as np
import json
#from pandas import date_range
import matplotlib.dates as mdates
import utilsOD
import os.path

a_laser = 950

# This is the Subscriber
def on_connect(client, userdata, flags, rc):
    if rc==0:
        client.connected_flag=True #set flag
        print("Connected with return code=",rc)
        client.subscribe("turbidostat/log/sensor")
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

from matplotlib.animation import FuncAnimation

fig, ax = plt.subplots()
xdata, ydata = [], []
ln, = plt.plot([], [], 'ro')

def init():
    ax.set_xlim(0, 2*np.pi)
    ax.set_ylim(-1, 1)
    return ln,

def update(frame):
    if frame:
        timestamps = list(map(datetime.datetime.fromtimestamp,frame[:, 0]))
        xdata.append(timestamps)
        ydata.append(utilsOD.getOD(frame[:,1],a_laser))
        ln.set_data(xdata, ydata)
    return ln,

def frames():
    while True:
        frame = []
        if len(client.buffer):
            frame = client.buffer
        client.buffer = []
        yield frame


ani = FuncAnimation(fig, update, frames ,
                    init_func=init, blit=True)
plt.show()



"""
plot_window = 1
#x_var = date_range(start=yesterday, end=today, periods=plot_window).to_pydatetime().tolist()
x_var = datetime.datetime.now() 
y_var = np.zeros(1)
y_var2 = np.zeros(1)
data = None

numReadings = 100
queue = [0]*numReadings

dateTimeObj = datetime.datetime.now()
filenames = ["log/"+ dateTimeObj.strftime("%d%b") + "_sensor.csv",""]
if len(sys.argv) > 1:
    filenames = sys.argv[1:]

for filename in filenames:
    print(filename)
    if os.path.isfile(filename):
        with open(filename,'r') as csvfile:
            reader = csv.reader(csvfile, delimiter = ',')
            labels = next(reader)
            x = list(reader)
            numColumns = len(x[0])
            for i in x:
                if len(i) - numColumns:
                    x.remove(i)
            if data is None:
                data = np.array(x).astype("float")
            else:
                data = np.vstack((data, (np.array(x).astype("float"))))
        x_var = np.vectorize(datetime.datetime.fromtimestamp)(data[:, 0])
        y_var = utilsOD.getOD(data[:,1],a_laser)
        
        y_var2 = np.zeros(shape=(y_var.shape[0]))
        for j in range(y_var.shape[0]):
            queue.pop(0)
            queue.append(y_var[j])
            y_var2[j] = sum(queue)/numReadings


# x_var = np.vectorize(datetime.datetime.fromtimestamp)(x_var)
plt.xlabel('time')
plt.gcf().autofmt_xdate()
plt.ylabel("OD")
# myFmt = mdates.DateFormatter('%H:%M')
# plt.gca().xaxis.set_major_formatter(myFmt)


plt.ion()
fig, ax = plt.subplots()
line, = ax.plot(x_var, y_var, '.')
line2, =ax.plot(x_var, y_var2, '.')
#ax.set_ylim([0,5])
#ax.set_xlim([x_var[0],(datetime.datetime.today()+datetime.timedelta(days=1))])
plt.grid()
# plt.yscale("log")

initial_time = time.time()
while True:
    try:
        if len(client.buffer):
            newData = client.buffer
            # print(newData)
            client.buffer = []
        else:
            continue

        for data in newData:
            plotData = [float(i) for i in data]
            #If averaging
            #Processing - averaging/low pass filtering
            plotData = np.around(utilsOD.getOD(data[1],a_laser),5)
            #plotData = data[1]
            print(plotData)
            queue.pop(0)
            queue.append(plotData)
            # x_var = np.append(x_var, (time.time() - initial_time))
            x_var = np.append(x_var, datetime.datetime.now())
            y_var = np.append(y_var, plotData)
            y_var2 = np.append(y_var2, sum(queue)/numReadings)
            # x_var = x_var[1:plot_window+1]
            # y_var = y_var[1:plot_window+1]
            # y_var2 = y_var2[1:plot_window+1]
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

"""