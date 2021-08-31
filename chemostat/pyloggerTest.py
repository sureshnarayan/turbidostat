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
import random
from cycler import cycler

name = "Dark2"#"Accent"
cmap = matplotlib.cm.get_cmap(name)  # type: matplotlib.colors.ListedColormap
colors = cmap.colors  # type: list
plt.rc('axes', prop_cycle=(cycler('color', colors)))

dateTimeObj = datetime.datetime.now()

plot_window = 100
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
ax.set_ylim([0,10])

initial_time = time.time()

a_laser = 600

while True:
    try:
        index = index + 1
        try:
            data = random.randint(1, 500)
            time.sleep(0.05)
            print(data)
        except:
            continue

        #If averaging
        #Processing - averaging/low pass filtering
        plotData = utilsOD.getOD(data, a_laser)
        queue.pop(0)
        queue.append(plotData)

        x_var = np.append(x_var, (time.time() - initial_time))
        y_var = np.append(y_var, plotData)
        y_var2 = np.append(y_var2, sum(queue)/numReadings)
        
        x_var = x_var[1:plot_window+1]
        y_var = y_var[1:plot_window+1]
        y_var2 = y_var2[1:plot_window+1]
        #line.set_xdata(x_var)
        line.set_ydata(y_var)
        line2.set_ydata(y_var2)
        ax.relim()
        #ax.autoscale_view()
        fig.canvas.draw()
        fig.canvas.flush_events()

    except Exception as e:
        print(e)
        raise
        break
        #continue