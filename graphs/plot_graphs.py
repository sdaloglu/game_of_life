import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import numpy as np
import re

# Read the execution_times.txt and make a plot of ranks vs time

def extract_execution_times(filename):
    data = []
    with open(filename, 'r') as file:
        for line in file:
            # Regular expression to match the pattern "Rank: [number] Time: [number] ms"
            match = re.search(r'Ranks: (\d+), Time: ([\d.]+) seconds', line)
            if match:
                # Extract rank and time, convert them to appropriate types
                rank = int(match.group(1))
                time = float(match.group(2))
                data.append((rank, time))
    return data


# Plot also the Amdhal's law prediction
def amdhal_law(n, p):
    return 1 / ((1 - p) + p/n)



# Extract the run with 10,000 x 10,000 grid size over 100 updates
filename_10k = 'execution_times_10000.txt'
extracted_data_10k = extract_execution_times(filename_10k)

# Extract the run with 20,000 x 20,000 grid size over 100 updates
filename_20k = 'execution_times_20000.txt'
extracted_data_20k = extract_execution_times(filename_20k)

# Extract the run with 5,000 x 5,000 grid size over 100 updates
filename_5k = 'execution_times_5000.txt'
extracted_data_5k = extract_execution_times(filename_5k)



plt.figure(figsize=(14, 10))
fig, ax = plt.subplots()
x_5k, y_5k = zip(*extracted_data_5k)
ax.plot(x_5k, y_5k, label='5,000x5,000 Observed', color='red')

x_10k, y_10k = zip(*extracted_data_10k)
ax.plot(x_10k,y_10k, label='10,000x10,000 Observed', color='blue')

x_20k, y_20k = zip(*extracted_data_20k)
ax.plot(x_20k, y_20k, label='20,000x20,000 Observed', color='green')

# Calculate the base time for each grid size
# The base time is the time taken by the simulation with 1 rank
base_time_5k = y_5k[0]
base_time_10k = y_10k[0]
base_time_20k = y_20k[0]

# Plot the theoretical predictions:

theoretical_time_5k_amdhal = [base_time_5k/amdhal_law(rank, 0.95) for rank in x_5k]
ax.plot(x_5k, theoretical_time_5k_amdhal, label='5,000x5,000 Theoretical P = 0.95', linestyle='-.', color='red')

theoretical_time_10k_amdhal = [base_time_10k/amdhal_law(rank, 0.95) for rank in x_10k]
ax.plot(x_10k, theoretical_time_10k_amdhal, label='10,000x10,000 Theoretical P = 0.95', linestyle='-.', color='blue')

theoretical_time_20k_amdhal = [base_time_20k/amdhal_law(rank, 0.95) for rank in x_20k]
ax.plot(x_20k, theoretical_time_20k_amdhal, label='20,000x20,000 Theoretical P = 0.95', linestyle='-.', color='green')



ax.legend()
ax.set_xlabel('Ranks')
ax.set_ylabel('Run Time (s)')
# Set y-axis to log scale
ax.set_yscale('log')
ax.set_title('Runtime of Game of Life simulations - 100 updates as a function of MPI ranks')

# Save the plot to a file
plt.savefig('ranks_vs_time.png')
plt.show()
    