import serial
import numpy as np
import matplotlib as plt
import csv

COM_PORT = 'COM6'
BAUD_PATES = 9600
ser = serial.Serial(COM_PORT, BAUD_PATES)

try:
    while True:
        while ser.in_waiting:
            data_raw = ser.readline()
            data = data_raw.decode()
            print('Raw Data ', data_raw)
            print('Data ', data)
            
            str_arr = data.split(',')
            length = len(str_arr)

except KeyboardInterrupt:
    ser.close()
    print('Interrupt Exit')