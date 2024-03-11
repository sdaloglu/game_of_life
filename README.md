# C2: Research Computing - Coursework Assignment

## Description

This project contains the code ..


## Problem Setting
Clone the remote GitLab on local

## Running the main algorithm

First build to project by
```bash
make build
```
Then compile the project with a chosen flag (release, debug, docs, run_tests)
```bash
make release
```

It is important to note that if one wants to access the documentation for the project, the compiling command should be
```bash
make docs
```
Similarly, if one wants to test the functionality of the libraries used for the project, the compiling command to use is:
```bash
make run_tests
```

After compiling the project, an executable for the game will be created and should from the root directory as:
```bash
./build/release/src/game {Grid size} {time steps}
```