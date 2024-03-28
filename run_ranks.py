import subprocess
import time
import os

# List of ranks to run from 1 to 50
ranks = range(1, 51)


base_dir = "/home/smd89/C2_coursework/C2_coursework"
executable_path = os.path.join(base_dir, "build/release/src/game")

# Open a file to record execution times
with open('execution_times.txt', 'w') as f:
    for rank in ranks:
        print(f"Running with {rank} ranks")
        start_time = time.time()

        # Modify the command according to your actual executable and its arguments
        result = subprocess.run(["mpirun", "-np", str(rank), executable_path, "5000", "100", "save"], capture_output=True, text=True)

        execution_time = time.time() - start_time
        print(f"Finished running with {rank} ranks in {execution_time} seconds")
        # Record the execution time to the file
        f.write(f"Ranks: {rank}, Time: {execution_time} seconds\n")

        # If there's error output, write it to the file as well
        if result.stderr:
            f.write(f"Errors for {rank} ranks:\n{result.stderr}\n")
        # Optional: Print MPI output to console or log file
        print(result.stdout)
        print(result.stderr)
        print("Done")

