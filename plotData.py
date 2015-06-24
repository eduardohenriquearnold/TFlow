import re
import numpy as np

import matplotlib
matplotlib.use('TkAgg')

import matplotlib.pyplot as plt
#from mpltools import style



def getDataFromString(s):
        res = re.match('(\d+\.*\d*)ms occ=(\d+\.*\d*) flow=(\d+\.*\d*)', s)
        
        if res:
                vec = res.group(1,2,3)        #tuple of results in string form
                vec = [float(v) for v in vec] #convert them to float
                return vec

plt.ion()
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)

ax.set_title("Occupation & Flow vs Time")
ax.legend(loc='upper right')
ax.hold(True)

maxCountSpan = 100
data = np.zeros((maxCountSpan, 3))
pOcc, = plt.plot(data[:,0], data[:,1], color='r', label='Occupancy')
pFlw, = plt.plot(data[:,0], data[:,2], color='b', label='Flow')

fig.show(False)
fig.canvas.draw()

#Cache bgnd
background = fig.canvas.copy_from_bbox(ax.bbox)

while True:
        ln = raw_input()
        newData=getDataFromString(ln)
        
        #roll array
        data = np.roll(data, -1, 0)
                
        #copy newData to end of data
        data[-1,:] = newData
        
        #Update plot data
        pOcc.set_data(data[:,0], data[:,1])
        pFlw.set_data(data[:,0], data[:,2])
        
        #Adjust axis to new time position
        plt.axis([data[0,0], data[-1,0], 0, 1])
        
        #Reset bgnd
        fig.canvas.restore_region(background)
        
        #Redraw just the lines
        ax.draw_artist(pOcc)
        ax.draw_artist(pFlw)
        
        #Fill axes
        fig.canvas.blit(ax.bbox)
        
#        plt.draw()

        #Iteratively Plot results        
#        plt.scatter(data[0], data[1], color='r', label='Occupancy')
#        plt.scatter(data[0], data[2], color='b', label='Flow')
#        plt.draw()
#        plt.pause(0.00001)


        
        
        
        
        

