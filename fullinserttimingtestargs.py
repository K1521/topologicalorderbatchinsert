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

    result = subprocess.run(command, capture_output=True, text=True)

    # Process the result (you may need to adjust this based on your program's output)
    #if len(result.stdout)<10000:
    if result.stderr:
        print(result.stderr)
    x,y,_=result.stdout.split("\n")
    
    
    x = list(map(int, x.strip().split(" ")))
    y = list(map(int, y.strip().split(" ")))

    return x,y

# Example usage:
repeats = 5
randseed = 42
nodenum = 100000
edgenum = 100000
samplesintervall = edgenum//1000
#insertsintervall = 10000
plt.subplot(1, 3, 1) 
for insertsintervall in [100,1000,10000,100000]:
    timings = run_cpp_program(nodenum, randseed, insertsintervall, repeats, edgenum, samplesintervall,False)
    #print("Timings:", timings)
    x,y=timings
    plt.plot(x,y, marker='o',label=f'{insertsintervall=}')

plt.title('Timing vs Iteration')
plt.xlabel('Iteration')
plt.ylabel('Time (microseconds)')
plt.legend()
#plt.show()


repeats = 5
randseed = 42
#nodenum = 100000
edgenum = 100000
samplesintervall = edgenum//1000
insertsintervall = 10000
plt.subplot(1, 3, 2) 
for nodenum in [1000,10000,100000,1000000,10000000]:
    timings = run_cpp_program(nodenum, randseed, insertsintervall, repeats, edgenum, samplesintervall,1)
    #print("Timings:", timings)
    x,y=timings
    plt.plot(x,y, marker='o',label=f'{nodenum=}')


repeats = 5
randseed = 42
#nodenum = 100000
edgenum = 100000
samplesintervall = edgenum//1000
insertsintervall = 10000
plt.subplot(1, 3, 3) 
for nodenum in [1000,10000,100000,1000000,10000000]:
    timings = run_cpp_program(nodenum, randseed, insertsintervall, repeats, edgenum, samplesintervall,2)
    #print("Timings:", timings)
    x,y=timings
    plt.plot(x,y, marker='o',label=f'{nodenum=}')


plt.title('Timing vs Iteration')
plt.xlabel('Iteration')
plt.ylabel('Time (microseconds)')
plt.legend()
plt.show()