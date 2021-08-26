import matplotlib.pyplot as plt
import csv
from datetime import datetime
import matplotlib.dates as mdates
  
x = []
y = []
  
with open('logged_sensor_plot.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter = ',')
      
    for row in plots:
        x.append(float(row[1]))
        ldr_raw = float(row[2])
        ldr = 10000 * (1023-ldr_raw)/ldr_raw
        y.append(ldr)
  
#plt.bar(x, y, color = 'g', width = 0.72, label = "Age")
plt.subplot(1, 2, 1)
plt.plot(x, y, 'b.')
plt.xlabel('Intensity (lux)')
#plt.gcf().autofmt_xdate()
#myFmt = mdates.DateFormatter('%H:%M')
#plt.gca().xaxis.set_major_formatter(myFmt)

#plt.xticks([]) 
plt.ylabel('LDR Resistance (Ohm)')
plt.title('LDR characteristics')

plt.subplot(1, 2, 2)
plt.plot(x, y, 'b.')
plt.yscale("log")
plt.xscale("log")
plt.xlabel('log Intensity (lux)')
#plt.gcf().autofmt_xdate()
#myFmt = mdates.DateFormatter('%H:%M')
#plt.gca().xaxis.set_major_formatter(myFmt)

#plt.xticks([]) 
plt.ylabel('log LDR Resistance (Ohm)')
plt.title('LDR characteristics (log-log)')

plt.show()
