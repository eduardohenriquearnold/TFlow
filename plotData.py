import re
import numpy as np
import matplotlib.pyplot as plt

def getDataFromString(s):
        res = re.match('(\d+\.*\d*)ms occ=(\d+\.*\d*) flow=(\d+\.*\d*)', s)
        
        if res:
                vec = res.group(1,2,3)        #tuple of results in string form
                vec = [float(v) for v in vec] #convert them to float
                return vec

plt.axis()
plt.ion()
plt.title("Occupation & Flow vs Time")
plt.legend(loc='upper left')
plt.show()

maxCountSpan = 100

count=0  
while True:
        ln = raw_input()
        data=getDataFromString(ln)

        count += 1
        if count is maxCountSpan:
                count = 0
                plt.cla()

        #Iteratively Plot results        
        plt.scatter(data[0], data[1], color='r', label='Occupancy')
        plt.scatter(data[0], data[2], color='b', label='Flow')
#        plt.draw()
        plt.pause(0.00001)


        
        
        
        
        

