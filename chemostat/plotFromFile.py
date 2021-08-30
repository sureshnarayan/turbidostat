import matplotlib.pyplot as plt
import csv, sys
from datetime import datetime
import matplotlib.dates as mdates
import numpy as np
from matplotlib.cm import get_cmap
from cycler import cycler

filename = 'log_20210820_225727.csv'
invert = 1
if len(sys.argv) > 1:
    filename = sys.argv[1]
    if len(sys.argv) > 2:
        invert = -1


name = "Dark2"#"Accent"
cmap = get_cmap(name)  # type: matplotlib.colors.ListedColormap
colors = cmap.colors  # type: list
plt.rc('axes', prop_cycle=(cycler('color', colors)))
  
x = []
y = []
data = None
labels = None
  
with open(filename,'r') as csvfile:
#with open('logged_sensor_plot.csv','r') as csvfile:
    reader = csv.reader(csvfile, delimiter = ',')
    labels = next(reader)
    x = list(reader)
    data = np.array(x).astype("float")


# time processing:
for i in range(data.shape[0]):
    if data[i,0] < 1629982730:
        data[i,0] = data[i,0] + 117881

x = np.vectorize(datetime.fromtimestamp)(data[:, 0])
plt.xlabel('time')
plt.gcf().autofmt_xdate()
myFmt = mdates.DateFormatter('%H:%M')
plt.gca().xaxis.set_major_formatter(myFmt)
plt.title('Plot - Raw data')
plt.ylabel('raw data')

for i in range(data.shape[1] - 1):
    y = invert * data[:,i+1]
    plt.plot(x, y, linestyle = 'solid')
    #plt.plot(x, y, '.')

    if 0: #If averaging
        #Processing - averaging/low pass filtering
        numReadings = 20
        queue = [0]*numReadings
        yvar = np.zeros(shape=(data.shape[0]))
        for j in range(data.shape[0]):
            queue.pop(0)
            queue.append(data[j,i+1])
            yvar[j] = sum(queue)/numReadings
        plt.plot(x, yvar, linestyle = 'solid')




#plt.bar(x, y, color = 'g', width = 0.72, label = "Age")
plt.show()
