import subprocess
import time
import os

# Define base directory and executable path
base_dir = "/home/smd89/C2_coursework/C2_coursework"
executable_path = os.path.join(base_dir, "build/release/src/game")

# Grid size and number of updates for the game simulation
grid_size = "5000"
updates = "100"

# Total number of cores available on the node
total_cores = 76

# Possible MPI and OpenMP configurations to explore
mpi_processes = [1, 2, 4, 19, 38, 76]
openmp_threads = [76, 38, 19, 4, 2, 1]

# Ensure the list lengths match and the configurations are complementary
assert len(mpi_processes) == len(openmp_threads)

# Open a file to record execution times
with open('mpi_openmp_tradeoff_execution_times.txt', 'w') as f:
    for mpi, omp in zip(mpi_processes, openmp_threads):
        print(f"Running with {mpi} MPI processes and {omp} OpenMP threads")
        start_time = time.time()

        # Set the OpenMP environment variable for the number of threads
        os.environ['OMP_NUM_THREADS'] = str(omp)

        # Execute the program with mpirun, specifying MPI processes and passing the grid size and updates
        result = subprocess.run(["mpirun", "-np", str(mpi), executable_path, grid_size, updates, "save"], capture_output=True, text=True)

        execution_time = time.time() - start_time
        print(f"Finished in {execution_time} seconds")

        # Record the configuration and execution time to the file
        f.write(f"MPI Processes: {mpi}, OpenMP Threads: {omp}, Time: {execution_time} seconds\n")

        if result.stderr:
            # If there's error output, write it to the file as well
            f.write(f"Errors with {mpi} MPI processes and {omp} OpenMP threads:\n{result.stderr}\n")

        # Optional: Print MPI and OpenMP output to console
        print(result.stdout)
        print(result.stderr)
        print("Configuration done\n")
