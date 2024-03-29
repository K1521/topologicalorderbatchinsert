import subprocess
import matplotlib.pyplot as plt
import math
def run_cpp_program(nodenum, randseed, insertsintervall, repeats, edgenum, samplesintervall,insertnodes):
    command = [
        "./fullinserttimingtestargs.exe",  # Adjust the executable name if needed
        #"./fullinserttimingtestargswithoutaddition.exe",
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




plt.xlabel('insertsintervall')
plt.ylabel('microseconds/edge')
randseed = 42
nodenum = 2500
maxedgenum=nodenum*(nodenum-1)//2
rs=25*maxedgenum*0.01
for density in [0.0001,0.0003,0.001,0.003,0.01]:
    #repeats = 50
    
    
    edgenum=max(int(maxedgenum*density),1)
    repeats = int(rs/edgenum)#this makes more repeats for smaller tests to make them more accurate
    pointsx=[];pointsy=[]
    p,=plt.plot(pointsx,pointsy,label=f"{density=} edges={edgenum}/{maxedgenum} {repeats=}")
    plt.legend()

    insertsintervall=1
    while insertsintervall<1000:

        samplesintervall = edgenum+1
        timings = run_cpp_program(nodenum, randseed, insertsintervall, repeats, edgenum, samplesintervall,1)
        #print("Timings:", timings)
        x,y=timings
        dy=y[-1]-y[0]#this is here to ignore setup time dy=y[-1] would be with setup time
        print(insertsintervall, x[-1],dy/edgenum)

        pointsx.append(insertsintervall)
        pointsy.append(dy/edgenum)
        p.set_data(pointsx,pointsy)
        plt.gca().relim()
        plt.gca().autoscale_view()
        plt.pause(0.1)

        insertsintervall=math.ceil(insertsintervall*1.5)

plt.show()