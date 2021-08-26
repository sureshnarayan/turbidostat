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
filename = "logged_"+ dateTimeObj.strftime("%d%b_%H%M%S") +".csv"
a_laser = 736
if len(sys.argv) > 1:
    filename = sys.argv[1]
    if len(sys.argv) > 2:
        a_laser = float(sys.argv[2])

plot_window = 1000
x_var = np.arange(plot_window, dtype=float)
y_var = np.zeros(plot_window)
index = 1

plt.ion()
fig, ax = plt.subplots()
line, = ax.plot(x_var, y_var)

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

        x_var = np.append(x_var, (time.time() - initial_time))
        y_var = np.append(y_var, utilsOD.getOD(data[0], a_laser))
        x_var = x_var[1:plot_window+1]
        y_var = y_var[1:plot_window+1]
        #line.set_xdata(x_var)
        line.set_ydata(y_var)
        ax.relim()
        # ax.set_ylim([0,1023])
        ax.autoscale_view()
        fig.canvas.draw()
        fig.canvas.flush_events()

    except Exception as e:
        #print("Keyboard Interrupt")
        ser.close()
        print(e)
        raise
        break
        #continue