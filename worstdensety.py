import subprocess
import matplotlib.pyplot as plt
def run_cpp_program(nodenum, randseed, insertsintervall, repeats, edgenum, samplesintervall,insertnodes):
    command = [
        "./fullinserttimingtestargs.exe",  # Adjust the executable name if needed
        str(nodenum),
        str(randseed),
        str(insertsintervall),
        str(repeats),
        str(edgenum),
        str(samplesintervall),
        str(int(insertnodes))
    ]
    print("running",command)

    result = subprocess.run(command, capture_output=True, text=True,timeout=500)

    # Process the result (you may need to adjust this based on your program's output)
    #if len(result.stdout)<10000:
    if result.stderr:
        print(result.stderr)
    #print(repr(result.stdout))
    x,y,_=result.stdout.split("\n")
    
    
    x = list(map(float, x.strip().split(" ")))
    y = list(map(float, y.strip().split(" ")))

    return x,y

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import math
import statistics
# Initialize lists for data
px, py = [], []
px2, py2 = [], []
py3 = []
# Create a figure and ax[0]is object
fig,ax = plt.subplots(2)

line, = ax[0].plot([], [])
line2, = ax[0].plot([], [],"o")
line3, = ax[1].plot([], [])
#line2, = ax[0].plot([], [])
# Set x-ax[0]is scale to logarithmic
#ax[0].set_xscale('log')
ax[0].set_xlabel("nodes")
ax[1].set_xlabel("nodes")
ax[0].set_ylabel("edges")
ax[1].set_ylabel("microsecconds/edge in worst case")
fig.tight_layout(pad=1.0)
# Function to update the plot
n=200
def update(frame):
    global px, py,n

    # Update data
    n = math.ceil(n*1.1)
    mnodes = n * (n + 1) // 2

    mes=[]
    mys=[]
    for i in range(7):
        x, y = run_cpp_program(n, 42, 100 , 1, n * 30, 100, 1)

        # Check if data is available
        if len(x) < 2:
            return line,
        y0=y[0]
        y = np.array(y[1:])
        x = np.array(x[1:])
        ynormal = y - y0  # remove setuptime
        yperedge = ynormal / x  # time per edge
        index = np.argmax(yperedge)
        me = x[index]  # edge where time per edge is max[0]imal
        #print(n, me, me / n, y[0], y[-1] - y[0])
        mes.append(me)
        mys.append(yperedge[index])
    for me in mes:
        px2.append(n)
        py2.append(me)
    px.append(n)
    py.append(statistics.median(mes))
    py3.append(statistics.median(mys))
    print(mes)

    # Update plot
    line.set_data(px, py)
    line2.set_data(px2, py2)
    line3.set_data(px, py3)
    ax[0].relim()
    ax[0].autoscale_view()
    ax[1].relim()
    ax[1].autoscale_view()





    return line,line2

plt.pause(10)
# Create animation
ani = FuncAnimation(fig, update, frames=200)

plt.show()

