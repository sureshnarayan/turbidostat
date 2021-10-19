import matplotlib.pyplot as plt
import csv, sys
from datetime import datetime,timedelta
import matplotlib.dates as mdates
import numpy as np
from matplotlib.cm import get_cmap
from cycler import cycler

filename = 'log_20210820_225727.csv'
withtime = True
if len(sys.argv) > 1:
    filename = sys.argv[1]
    if len(sys.argv) > 2:
        withtime = False


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

dataAxisStart = 0

if withtime:
    dataAxisStart = 1

    time = data[-1,0] - data[0,0]
    print("Total time : " + str(timedelta(seconds=time)))

    x = np.vectorize(datetime.fromtimestamp)(data[:, 0])
    plt.xlabel('time')
    plt.gcf().autofmt_xdate()
    myFmt = mdates.DateFormatter('%H:%M')
    plt.gca().xaxis.set_major_formatter(myFmt)
    plt.title('Plot - Raw data vs time')
    plt.ylabel('raw data')


for i in range(data.shape[dataAxisStart] - 1):
    # y = np.log10(900 / (data[:,i+1]))
    y = data[:,i+1]
    #plt.plot(x, y, '.')
    # plt.yscale("log")
    plt.grid(True)
    #plt.plot(x, y, '.')

    if 0: #If averaging
        #Processing - averaging/low pass filtering
        numReadings = 100
        queue = [0]*numReadings
        yvar = np.zeros(shape=(data.shape[0]))
        for j in range(data.shape[0]):
            queue.pop(0)
            queue.append(data[j,i+1])
            yvar[j] = sum(queue)/numReadings
        y = np.stack((y, yvar), axis=-1)
    plt.plot(x, y, '.')



plt.show()
