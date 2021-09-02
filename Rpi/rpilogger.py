import serial
import time
import datetime
import csv
#import matplotlib
#matplotlib.use("tkAgg")
#import matplotlib.pyplot as plt
import numpy as np

ser = serial.Serial('/dev/ttyACM0')
ser.flushInput()

"""
plot_window = 200
y_var = np.array(np.zeros([plot_window]))

plt.ion()
fig, ax = plt.subplots()
line, = ax.plot(y_var)
"""

timestr = time.strftime("%Y%m%d_%H%M%S")

while True:
    try:
        ser_bytes = ser.readline()
        try:
            decoded_bytes = ser_bytes[0:len(ser_bytes)-2].decode("utf-8")
            data = [float(i) for i in (decoded_bytes.split(","))]
            print(data)
        except:
            continue
        with open("log/log_"+timestr+".csv","a") as f:
            writer = csv.writer(f,delimiter=",")
            #dateTimeObj = datetime.now()
            #timestampStr = dateTimeObj.strftime("%d-%b-%Y (%H:%M:%S.%f)")
            writer.writerow([time.time()] + data)
            # writer.writerow([timestampStr,decoded_bytes])
        """
        y_var = np.append(y_var,decoded_bytes)
        y_var = y_var[1:plot_window+1]
        line.set_ydata(y_var)
        ax.relim()
        ax.set_ylim([940,1000])
        #ax.autoscale_view()
        fig.canvas.draw()
        fig.canvas.flush_events()
        """
    except:
        print("Keyboard Interrupt")
        ser.close()
        break
