import serial
import time
import datetime
import csv
import matplotlib
matplotlib.use("tkAgg")
import matplotlib.pyplot as plt
import numpy as np
import utilsOD
import sys

ser = serial.Serial('/dev/ttyACM0',9600,timeout=30)
ser.flushInput()

dateTimeObj = datetime.datetime.now()
# timestampStr = dateTimeObj.strftime("%d-%b-%Y (%H:%M:%S.%f)")
filename = "log/log_"+ dateTimeObj.strftime("%d%b_%H%M%S") +".csv"
# a_laser = 900
a_laser = 900
if len(sys.argv) > 1:
    filename = sys.argv[1]
    if len(sys.argv) > 2:
        a_laser = float(sys.argv[2])

plot_window = 10000
x_var = np.arange(plot_window, dtype=float)
y_var = np.zeros(plot_window)
y_var2 = np.zeros(plot_window)
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
    #while (ser.inWaiting() == 0):   
        #pass 
    try:
        index = index + 1
        ser_bytes = ser.readline()
        try:
            decoded_bytes = ser_bytes[0:len(ser_bytes)-2].decode("utf-8")
            data = [float(i) for i in (decoded_bytes.split(","))]
            print(data)
        except:
            continue
        with open(filename,"a") as f:
            writer = csv.writer(f,delimiter=",")

            #dateTimeObj = datetime.now()
            #timestampStr = dateTimeObj.strftime("%d-%b-%Y (%H:%M:%S.%f)")
            writer.writerow([time.time()] + data)
            # writer.writerow([timestampStr,decoded_bytes])


        #If averaging
        #Processing - averaging/low pass filtering
        plotData = utilsOD.getOD(data[0], a_laser)
        queue.pop(0)
        queue.append(plotData)

        x_var = np.append(x_var, (time.time() - initial_time))
        y_var = np.append(y_var, np.log2(plotData))
        y_var2 = np.append(y_var2, np.log2(sum(queue)/numReadings))
        x_var = x_var[1:plot_window+1]
        y_var = y_var[1:plot_window+1]
        y_var2 = y_var2[1:plot_window+1]
        #line.set_xdata(x_var)
        line.set_ydata(y_var)
        line2.set_ydata(y_var2)
        ax.relim()
        # ax.set_ylim([0,1023])
        # ax.autoscale_view()
        fig.canvas.draw()
        fig.canvas.flush_events()

    except Exception as e:
        #print("Keyboard Interrupt")
        ser.close()
        print(e)
        raise
        break
        #continue