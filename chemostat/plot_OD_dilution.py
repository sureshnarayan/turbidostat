import matplotlib.pyplot as plt
import csv, sys
from datetime import datetime
import matplotlib.dates as mdates
import numpy as np
from matplotlib.cm import get_cmap
from cycler import cycler

filename = 'logged_dilution_24_8.csv'
x_axis = "dilution"
if len(sys.argv) > 1:
    filename = sys.argv[1]
    if len(sys.argv) > 2:
        x_axis = "time"


x = []
y = []
data = None
data1 = None
labels = None
  
with open(filename,'r') as csvfile:
#with open('logged_sensor_plot.csv','r') as csvfile:
    reader = csv.reader(csvfile, delimiter = ',')
    labels = next(reader)
    x = list(reader)
    data = np.array(x).astype("float")

with open('logged_dilution_24_8_second.csv','r') as csvfile:
#with open('logged_sensor_plot.csv','r') as csvfile:
    reader = csv.reader(csvfile, delimiter = ',')
    labels = next(reader)
    x = list(reader)
    data1 = np.array(x).astype("float")

plot_title = "OD Variation on Dilution (V_laser = 4V)"

# reading without sample - of analogRead in Arduino (max 1023)
# a_laser = 1004
a_laser = 404

# what readings - beginning and end - in the data
# begin = 1092
# end = 1830 #data.shape[0]-1
# begin = 1853
begin = 112
end = data.shape[0]-1

v_initial = 6

if not end:
    end = data.shape[0]-1
experiment_data = data[begin:end]

def calculatePlot(plot_title, experiment_data, a_laser, v_initial, subplot_index, x_axis):

    # resistance in series with LDR - in Ohm
    r_series = 10000.0

    # OD conversion
    #   get resistance
    r_ldr = r_series * (1023 - experiment_data[:,1]) / experiment_data[:,1]
    r_laser = r_series * (1023 - a_laser) / a_laser

    #   get logIntensity
    # logR = k*logI + C
    k = -0.87
    C = 8
    logI = (np.log10(r_ldr) - C ) / k
    logI_laser = (np.log10(r_laser) - C ) / k

    #   get OD
    OD = logI_laser - logI

    # time in seconds
    time = experiment_data[:,0] - experiment_data[0,0]
    print("Total time : " + str(time[-1]) + " seconds")

    # Dilution conversion
    #   
    # v_inflow = 1/126
    v_inflow = 1/210
    # v_initial = 6
    # v_initial = 11.85 + 23/126

    # v_YPD = np.arange(experiment_data.shape[0]) * v_inflow
    v_YPD = time * v_inflow

    print("Final volume : " + str(v_initial + v_YPD[-1]) + " ml")
    dilution = (1 + v_YPD/v_initial)
    # dilution_factor = 1/(1 + v_YPD/6)
    # normalized_OD = OD * dilution_factor



    name = "Dark2"#"Accent"
    cmap = get_cmap(name)  # type: matplotlib.colors.ListedColormap
    colors = cmap.colors  # type: list
    plt.rc('axes', prop_cycle=(cycler('color', colors)))

    real_time = np.vectorize(datetime.fromtimestamp)(experiment_data[:, 0])
    # plt.xlabel('time')
    plt.gcf().autofmt_xdate()
    myFmt = mdates.DateFormatter('%H:%M')
    plt.gca().xaxis.set_major_formatter(myFmt)
    # plt.title('Plot')
    # plt.ylabel('data')

    #plt.subplot(1, 3, subplot_index)
    raw = experiment_data[:,1]

    if x_axis == "time":
        plt.plot(real_time, OD, '.', label = plot_title)
        plt.xlabel("Time")
    else:
        plt.plot(dilution, OD, '.', label = plot_title)
        plt.xlabel("Dilution Factor")

        
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

    plt.ylabel("OD")
    # plt.title("OD Variation on Dilution (V_laser = 4V)")
    plt.legend(loc = "upper right")

#calculatePlot("Plot label",  data[begin:end],   a_laser, v_initial, 1, x_axis)
# calculatePlot("4V laser, 6ml init volume",  data[1092:1830],   1000, 6, 1, x_axis)
# calculatePlot("3V laser, 12ml init volume", data[1853:],        740, (11.85 + 23/126), 2, x_axis)
# calculatePlot("3V laser, 6ml init volume",  data1[112:],        740, 6, 3, x_axis)

# calculatePlot("3V laser, 6ml init volume", data[:], 736, 6, 1, x_axis)


# time processing:
for i in range(data.shape[0]):
    if data[i,0] < 1629982730:
        data[i,0] = data[i,0] + 117881


calculatePlot("Yeast growth 27Aug", data[:], 900, 6, 1, x_axis)


# for i in range(data.shape[1] - 1):
#     y = invert * data[:,i+1]
#     plt.plot(x, y, linestyle = 'solid')
#     #plt.plot(x, y, '.')
#     if 0: #If averaging
#         #Processing - averaging/low pass filtering
#         numReadings = 100
#         queue = [0]*numReadings
#         yvar = np.zeros(shape=(data.shape[0]))
#         for j in range(data.shape[0]):
#             queue.pop(0)
#             queue.append(data[j,i+1])
#             yvar[j] = sum(queue)/numReadings
#         plt.plot(x, yvar, linestyle = 'solid')




#plt.bar(x, y, color = 'g', width = 0.72, label = "Age")
plt.show()
