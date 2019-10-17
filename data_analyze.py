import csv
import matplotlib.pyplot as plt
import numpy as np

context1_dataset = []
sample1_dataset = []

def read_csv_to_array(filename):
    re = []
    with open(filename, newline='') as csvfile:
        rows = csv.reader(csvfile)
        # print(type(rows))
        for row in rows:
            re.append(row)
    return re

def get_target(data, index = 'last'):
    data_np = np.array(data)
    # data_len = len(data[0])
    # print(data_len)
    # print(data)
    targets = []
    if index == 'last':
        targets = data_np[:, -1]
        return targets
    else:
        targets = data_np[:, index]
        return targets

def get_train_data(data, target_index = 'last'):
    # data_len = len(data[0])
    data_np = np.array(data)
    if target_index == 'last':
        train_data = data_np[:, :-1]
        return train_data
    else:
        
        train_data = np.delete(data, target_index, axis = 1)
        return train_data

context1_dataset = read_csv_to_array('context1.data')
sample1_dataset = read_csv_to_array('sample1.data')

context1_target = get_target(context1_dataset)
context1_data = get_train_data(context1_dataset)
# print(type(context1_dataset[0][-1]))
# print(type(context1_dataset[0]))
# print(type(context1_dataset))
print(context1_data)
print(context1_target)