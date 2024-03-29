import subprocess
import matplotlib.pyplot as plt
import math
import numpy as np
import time
def run_cpp_program(path,nodenum, randseed, insertsintervall, repeats, edgenum, samplesintervall,insertnodes):
    command = [
        #"./fullinserttimingtestargswithoutaddition.exe",  # Adjust the executable name if needed
        path,
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







#path="./fullinserttimingtestargs.exe"
path="./fullinserttimingtestargswithoutaddition.exe"
logdensity=True
logbatchsize=True
randseed = 42
nodenum = 10000

batchsizeminmaxstep=[1,50000,1.3]
densetyminmaxstep=[0.0001,0.1,2]
#rs=5*maxedgenum*0.01
#rs=maxedgenum*0.01
rs=40000#this means the mesurement is repeated for a given density and insertsintervall untill a minimum of 40000 edges are inserted
#rs=159937*3+3
def mesuretime( randseed, nodenum, insertsintervall,density):
    maxedgenum=nodenum*(nodenum-1)//2#Full graph
    edgenum=math.ceil(maxedgenum*density)
    repeats = math.ceil(rs/edgenum) #this makes more repeats for smaller tests to make them more accurate
    samplesintervall = edgenum+1
    timings = run_cpp_program(path,nodenum, randseed, insertsintervall, repeats, edgenum, samplesintervall,1)
        #print("Timings:", timings)
    x,y=timings
    dy1=y[-1]-y[0]#ignore setup time 
    dy2=y[-1]#with setup time 
    msperedgeignore=dy1/edgenum
    msperedgewith=dy2/edgenum
    print(insertsintervall, x[-1],msperedgeignore,msperedgewith)
    return msperedgeignore,msperedgewith


insertsintervalls=[batchsizeminmaxstep[0]]
while insertsintervalls[-1]<batchsizeminmaxstep[1]:
    insertsintervalls.append(math.ceil(insertsintervalls[-1]*batchsizeminmaxstep[2]))
densitys=10**np.arange(np.log10(densetyminmaxstep[0]),np.log10(densetyminmaxstep[1]),np.log10(densetyminmaxstep[2]))

X, Y = np.meshgrid(np.log10(insertsintervalls)if logbatchsize else insertsintervalls, np.log10(densitys)if logdensity else densitys)

Z=np.zeros(X.shape)
Z2=np.zeros(X.shape)
fig = plt.figure()

ax1 = fig.add_subplot(1, 2, 1, projection='3d')
ax1.set_title("without setup time")
ax2 = fig.add_subplot(1, 2, 2, projection='3d')
ax2.set_title("with setup time")

for ax in(ax1,ax2):
    if logbatchsize:
        ax.set_xlabel('log(batchsize)')
    else:
        ax.set_xlabel('batchsize')

    if logbatchsize:
        ax.set_ylabel('log(density)')
    else:
        ax.set_ylabel('density')

    ax.set_zlabel('microseconds/edge')
surf1=ax1.plot_surface(X, Y, Z, cmap='viridis')
surf2=ax2.plot_surface(X, Y, Z, cmap='viridis')
print(surf1)


update_interval = 2  # Update interval in seconds
last_update_time = time.time()
for i,density in enumerate(densitys):
    for j,insertsintervall in enumerate(insertsintervalls):
        # Calculate msperedge
        msperedgeignore,msperedgewith = mesuretime(randseed, nodenum, insertsintervall, density)
        Z[i,j]=msperedgeignore
        Z2[i,j]=msperedgewith
        # Plot scatter point
        #ax.scatter(X[i,j], Y[i,j], msperedge, 'o')
        

        if time.time() - last_update_time >= update_interval:
            surf1.remove()  # Remove the existing surface plot
            surf1 = ax1.plot_surface(X, Y, Z, cmap='viridis')
            surf2.remove()  # Remove the existing surface plot
            surf2 = ax2.plot_surface(X, Y, Z2, cmap='viridis')
            plt.pause(0.1)
            last_update_time = time.time()


surf1.remove()  # Remove the existing surface plot
surf1 = ax1.plot_surface(X, Y, Z, cmap='viridis')
ax1.axes.set_zlim3d(bottom=0,top=np.max(Z))#this is here to set the minimum to zero
print(np.average(Z))

surf2.remove()  # Remove the existing surface plot
surf2 = ax2.plot_surface(X, Y, Z2, cmap='viridis')
ax2.axes.set_zlim3d(bottom=0,top=np.max(Z2))
print(np.average(Z2))
plt.show()


