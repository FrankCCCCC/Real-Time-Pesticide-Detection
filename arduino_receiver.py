import serial
import numpy as np
import matplotlib.pyplot as plt
import csv

COM_PORT = 'COM7'
BAUD_PATES = 9600
# ser = serial.Serial(COM_PORT, BAUD_PATES)

# serial_record = 

# get_serial_data -> add_tag -> store_data
#                 -> convert_data -> show

def convert_data(temp_arr):
    array_convert = np.array(temp_arr)
    array_convert = array_convert.astype(np.float)
    # array_convert_T = array_convert.transpose()
    return array_convert

def add_tag(tag, data_array):
    # data_array_np = np.array(data_array)
    data_array = [x + [tag] for x in data_array]
    return data_array

def show_all_data(data_array_T):
    shape = data_array_T.shape()
    x_axis = np.arange(0, shape[1], 1)
    plt.plot(x_axis, data_array_T[0], label='CO2 ppm')
    plt.legend()
    plt.show()
    plt.plot(x_axis, data_array_T[1], label='tVOC ppb')
    plt.legend()
    plt.show()
    plt.plot(x_axis, data_array_T[2], label='alc_raw')
    # plt.plot(x_axis, data_array_T[3], label='lpg_raw')
    # plt.plot(x_axis, data_array_T[4], label='Temperature C')
    # plt.plot(x_axis, data_array_T[5], label='Pressure Pa')
    # plt.plot(x_axis, data_array_T[6], label='Altitude m')
    # plt.plot(x_axis, data_array_T[7], label='Humidity rh')
    plt.legend()
    plt.show()

def store_data(file_name, data_array):
    class_np_array = type(np.zeros(2))
    class_list = type([1, 2])

    with open(file_name, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        #Check is Numpy array or not
        # if type(data_array) == class_list:
            # for in range():
                # print()

def get_serial_data(ser):
    try:
        while True:
            # i = 0
            temp_arr = []
            while ser.in_waiting:
                data_raw = ser.readline()
                data = data_raw.decode()
                print('Raw Data ', data_raw)
                print('Data ', data)
                
                str_arr = data.split(',')
                if len(str_arr) == 8:
                    temp_arr.append(str_arr)
                # i = i + 1

    except KeyboardInterrupt:
        ser.close()
        print('Interrupt Exit')
    return temp_arr



# print(float_array_T)
# length = len(float_array_T[0])
# param_num = float_array_T.shape()
# param_num = param_num[0]

# for i in range(param_num):

# setting an array element with a sequence.