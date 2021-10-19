import csv, sys
import numpy as np
import utilsOD

filenames = []
if len(sys.argv) > 1:
    filenames = sys.argv[1:]


for filename in filenames:
    x = []
    avg = []
    inputData = None
    outputData = None
    with open(filename,'r') as csvfile:
        reader = csv.reader(csvfile, delimiter = ',')
        x = list(reader)
        inputData = np.array(x).astype("float")
    
    intensity = utilsOD.getIntensity(inputData[:,1])
    if 1: #If averaging
        #Processing - averaging/low pass filtering
        numReadings = 100
        queue = [intensity[0]]*numReadings
        avg = np.ones(shape=(inputData.shape[0]))
        for j in range(inputData.shape[0]):
            queue.pop(0)
            queue.append(intensity[j])
            avg[j] = sum(queue)/numReadings

    outputData = np.stack((inputData[:,0], inputData[:,1], intensity, avg), -1)
    np.savetxt(filename.replace('sensor','intensity'), outputData, fmt='%.3f', delimiter=",")