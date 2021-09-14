import matplotlib.pyplot as plt
import csv, sys
from datetime import datetime
import matplotlib.dates as mdates
import numpy as np
from matplotlib.cm import get_cmap
from cycler import cycler

filenames = ""
if len(sys.argv) > 1:
    filenames = sys.argv[1:]

# reading without sample - of analogRead in Arduino (max 1023)
a_laser = 900

x = []
y = []
data = None
labels = None
  
for filename in filenames:
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

experiment_data = data[:]

def getOD(a_ldr, a_laser):
    # resistance in series with LDR - in Ohm
    r_series = 10000.0

    a_ldr[a_ldr == 0] = 1

    # OD conversion
    #   get resistance
    r_ldr   = r_series * (1023 - a_ldr) / a_ldr
    r_laser = r_series * (1023 - a_laser) / a_laser

    #   get logIntensity
    # logR = k*logI + C
    k = -0.87
    C = 8
    logI = (np.log10(r_ldr) - C ) / k
    logI_laser = (np.log10(r_laser) - C ) / k

    #   get OD
    OD = logI_laser - logI

    return OD


def calculatePlot(plot_title, experiment_data, a_laser):

    OD = getOD(experiment_data[:,1],a_laser)

    # time in seconds
    time = experiment_data[:,0] - experiment_data[0,0]
    print("Total time : " + str(time[-1]) + " seconds")


    name = "Dark2"#"Accent"
    cmap = get_cmap(name)  ##type: matplotlib.colors.ListedColormap
    colors = cmap.colors  # type: list
    plt.rc('axes', prop_cycle=(cycler('color', colors)))

    real_time = np.vectorize(datetime.fromtimestamp)(experiment_data[:, 0])
    # plt.xlabel('time')
    # plt.yscale('log')
    plt.gcf().autofmt_xdate()
    myFmt = mdates.DateFormatter('%H:%M')
    plt.gca().xaxis.set_major_formatter(myFmt)
    # plt.title('Plot')
    # plt.ylabel('data')

    #plt.subplot(1, 3, subplot_index)
    raw = experiment_data[:,1]

    plt.plot(real_time, OD, '.', label = plot_title)
    plt.xlabel("Time")

        
    if 1: #If averaging
        #Processing - averaging/low pass filtering
        numReadings = 100
        queue = [0]*numReadings
        yvar = np.zeros(shape=(OD.shape[0]))
        for j in range(OD.shape[0]):
            queue.pop(0)
            queue.append(OD[j])
            yvar[j] = sum(queue)/numReadings
        plt.plot(real_time, yvar, '.', label = "Averaged (100 values)")

    if 0: #Inflow times
        inflow_time = []
        for j in range(experiment_data.shape[0]):
            if experiment_data[j,2] < 700:
                inflow_time.append(j)
        plt.plot(real_time[inflow_time], OD[inflow_time], 'g.', label = "Inflow timings")

    plt.ylabel("OD")
    # plt.yscale("log")
    # plt.title("OD Variation on Dilution (V_laser = 4V)")
    plt.legend(loc = "upper left")
    plt.grid(True)

# time processing:
for i in range(data.shape[0]):
    if data[i,0] < 1630426739:
        data[i,0] = data[i,0] + (1630426739 - 1630394045)

plot_str = "OD " + datetime.utcfromtimestamp(experiment_data[0,0]).strftime("%d%b")
calculatePlot(plot_str, data[:], a_laser)

plt.show()
