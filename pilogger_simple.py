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

dateTimeObj = datetime.datetime.now()
filename = "log/log_"+ dateTimeObj.strftime("%d%b_%H%M%S") +".csv"

while True:
    try:
        ser_bytes = ser.readline()
        try:
            decoded_bytes = ser_bytes[0:len(ser_bytes)-2].decode("utf-8")
            data = [float(i) for i in (decoded_bytes.split(","))]
            print(data)
        except:
            continue
        with open(filename,"a") as f:
            writer = csv.writer(f,delimiter=",")
            writer.writerow([time.time()] + data)

    except:
        print("Keyboard Interrupt")
        ser.close()
        break
