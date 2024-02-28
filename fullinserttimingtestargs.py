import subprocess

def run_cpp_program(nodenum, randseed, insertsintervall, repeats, edgenum, samplesintervall):
    command = [
        "./my_cpp_program",  # Adjust the executable name if needed
        str(nodenum),
        str(randseed),
        str(insertsintervall),
        str(repeats),
        str(edgenum),
        str(samplesintervall)
    ]

    result = subprocess.run(command, capture_output=True, text=True)

    # Process the result (you may need to adjust this based on your program's output)
    timings = list(map(float, result.stdout.split()))

    return timings

# Example usage:
nodenum = 1000
randseed = 42
insertsintervall = 5
repeats = 3
edgenum = 5000
samplesintervall = 100

timings = run_cpp_program(nodenum, randseed, insertsintervall, repeats, edgenum, samplesintervall)
print("Timings:", timings)