import subprocess
import matplotlib.pyplot as plt
import math
import numpy as np
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

    result = subprocess.run(command, capture_output=True, text=True)

    # Process the result (you may need to adjust this based on your program's output)
    #if len(result.stdout)<10000:
    if result.stderr:
        print(result.stderr)
    x,y,_=result.stdout.split("\n")
    
    
    x = list(map(int, x.strip().split(" ")))
    y = list(map(int, y.strip().split(" ")))

    return x,y



fig = plt.figure()
ax = fig.add_subplot(projection='3d')
ax.set_xlabel('insertsintervall')
ax.set_ylabel('log10(density)')
ax.set_zlabel('microseconds/edge')
randseed = 42
nodenum = 2500
maxedgenum=nodenum*(nodenum-1)//2
rs=5*maxedgenum*0.01
#rs=maxedgenum*0.01
def mesuretime( randseed, nodenum, insertsintervall,density):
    edgenum=math.ceil(maxedgenum*density)
    repeats = math.ceil(rs/edgenum) #this makes more repeats for smaller tests to make them more accurate
    samplesintervall = edgenum+1
    timings = run_cpp_program(nodenum, randseed, insertsintervall, repeats, edgenum, samplesintervall,2)
        #print("Timings:", timings)
    x,y=timings
    dy=y[-1]-y[0]#this is here to ignore setup time dy=y[-1] would be with setup time
    msperedge=dy/edgenum
    print(insertsintervall, x[-1],msperedge)
    return msperedge


insertsintervalls=[1]
while insertsintervalls[-1]<500:
    insertsintervalls.append(math.ceil(insertsintervalls[-1]*1.3))
densitys=10**np.arange(-4,-1,0.25)

X, Y = np.meshgrid(insertsintervalls, np.log10(densitys))
X, Y = np.meshgrid(insertsintervalls, densitys)
#X, Y = np.meshgrid(np.log10(insertsintervalls), np.log10(densitys))
Z=np.zeros(X.shape)


surf=ax.plot_surface(X, Y, Z, cmap='viridis', alpha=0.5)
print(surf)

for i,density in enumerate(densitys):
    for j,insertsintervall in enumerate(insertsintervalls):
        # Calculate msperedge
        msperedge = mesuretime(randseed, nodenum, insertsintervall, density)
        Z[i,j]=msperedge
        # Plot scatter point
        #ax.scatter(X[i,j], Y[i,j], msperedge, 'o')
        

        surf.remove()  # Remove the existing surface plot
        surf = ax.plot_surface(X, Y, Z, cmap='viridis')
        #ax.plot_surface(X, Y, Z, cmap='viridis', alpha=0.5)
        
        plt.pause(0.01)

plt.show()
plt.show()