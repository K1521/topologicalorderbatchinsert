import subprocess
import matplotlib.pyplot as plt
import math
import numpy as np
import time
def run_cpp_program(nodenum, randseed, insertsintervall, repeats, edgenum, samplesintervall,insertnodes):
    command = [
        #"./fullinserttimingtestargswithoutaddition.exe",  # Adjust the executable name if needed
        "./fullinserttimingtestargs.exe",
        str(nodenum),
        str(randseed),
        str(insertsintervall),
        str(repeats),
        str(edgenum),
        str(samplesintervall),
        str(int(insertnodes))
    ]
    print("running",command)

    result = subprocess.run(command, capture_output=True, text=True)

    # Process the result (you may need to adjust this based on your program's output)
    #if len(result.stdout)<10000:
    if result.stderr:
        print(result.stderr)
    x,y,_=result.stdout.split("\n")
    
    
    x = list(map(float, x.strip().split(" ")))
    y = list(map(float, y.strip().split(" ")))

    return x,y








logdensity=True
logbatchsize=True
randseed = 42
nodenum = 2500

batchsizeminmaxstep=[1,50000,1.3]
densetyminmaxstep=[0.0001,0.1,2]
#rs=5*maxedgenum*0.01
#rs=maxedgenum*0.01
rs=40000#this means the mesurement is repeated for a given density and insertsintervall untill a minimum of 40000 edges are inserted
def mesuretime( randseed, nodenum, insertsintervall,density):
    maxedgenum=nodenum*(nodenum-1)//2#Full graph
    edgenum=math.ceil(maxedgenum*density)
    repeats = math.ceil(rs/edgenum) #this makes more repeats for smaller tests to make them more accurate
    samplesintervall = edgenum+1
    timings = run_cpp_program(nodenum, randseed, insertsintervall, repeats, edgenum, samplesintervall,1)
        #print("Timings:", timings)
    x,y=timings
    dy=y[-1]-y[0]#this is here to ignore setup time dy=y[-1] would be with setup time
    msperedge=dy/edgenum
    print(insertsintervall, x[-1],msperedge)
    return msperedge


insertsintervalls=[batchsizeminmaxstep[0]]
while insertsintervalls[-1]<batchsizeminmaxstep[1]:
    insertsintervalls.append(math.ceil(insertsintervalls[-1]*batchsizeminmaxstep[2]))
densitys=10**np.arange(np.log10(densetyminmaxstep[0]),np.log10(densetyminmaxstep[1]),np.log10(densetyminmaxstep[2]))

X, Y = np.meshgrid(np.log10(insertsintervalls)if logbatchsize else insertsintervalls, np.log10(densitys)if logdensity else densitys)

Z=np.zeros(X.shape)

fig = plt.figure()

ax = fig.add_subplot(projection='3d')
if logbatchsize:
    ax.set_xlabel('log(batchsize)')
else:
    ax.set_xlabel('batchsize')

if logbatchsize:
    ax.set_ylabel('log(density)')
else:
    ax.set_ylabel('density')

ax.set_zlabel('microseconds/edge')
surf=ax.plot_surface(X, Y, Z, cmap='viridis')
print(surf)


update_interval = 2  # Update interval in seconds
last_update_time = time.time()
for i,density in enumerate(densitys):
    for j,insertsintervall in enumerate(insertsintervalls):
        # Calculate msperedge
        msperedge = mesuretime(randseed, nodenum, insertsintervall, density)
        Z[i,j]=(msperedge)
        # Plot scatter point
        #ax.scatter(X[i,j], Y[i,j], msperedge, 'o')
        

        if time.time() - last_update_time >= update_interval:
            surf.remove()  # Remove the existing surface plot
            surf = ax.plot_surface(X, Y, Z, cmap='viridis')
            plt.pause(0.1)
            last_update_time = time.time()


surf.remove()  # Remove the existing surface plot
surf = ax.plot_surface(X, Y, Z, cmap='viridis')
ax.axes.set_zlim3d(bottom=0,top=np.max(Z))#this is here to set the minimum to zero
print(np.average(Z))
plt.show()
