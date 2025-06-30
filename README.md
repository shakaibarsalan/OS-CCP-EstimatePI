# Pi Estimation Using Maclaurin Series — Parallel & Sequential Approaches

## Overview

This project estimates the value of **π (Pi)** using the Maclaurin series for arctangent, implementing and comparing four different approaches:
- **Sequential computation**
- **Multiprocessing with shared memory**
- **Multiprocessing with pipes**
- **Multithreading**

The goal is to demonstrate how parallel programming techniques can accelerate computationally intensive tasks, and to analyze the trade-offs in synchronization and resource utilization.

## Mathematical Background

The Maclaurin series for arctan(1) is:

$$
\arctan(1) = \frac{\pi}{4} = \sum_{n=0}^{\infty} \frac{(-1)^n}{2n+1}
$$

Thus,

$$
\pi = 4 \times \sum_{n=0}^{N} \frac{(-1)^n}{2n+1}
$$

## Project Structure

| File Name            | Description                                              |
|----------------------|---------------------------------------------------------|
| `sequential.cpp`     | Single-threaded, sequential calculation of π            |
| `shared-memory.cpp`  | Multiprocessing with shared memory and locks            |
| `pipes.cpp`          | Multiprocessing with inter-process communication (pipes) |
| `thread.cpp`         | Multithreaded computation using mutex for synchronization|
| `Document.pdf`       | Project report, algorithm discussion, and performance analysis |

## How to Build

All files are standard C++ and require a POSIX-compliant system for multiprocessing features.

**Compile each file using:**
```bash
g++ -o sequential sequential.cpp -std=c++11
g++ -o shared-memory shared-memory.cpp -std=c++11 -pthread
g++ -o pipes pipes.cpp -std=c++11
g++ -o thread thread.cpp -std=c++11 -pthread
```

## How to Run

### 1. Sequential Version

```bash
./sequential 
```
- Example: `./sequential 1000000`

### 2. Multiprocessing (Shared Memory)

```bash
./shared-memory  
```
- Example: `./shared-memory 4 1000000`

### 3. Multiprocessing (Pipes)

```bash
./pipes  
```
- Example: `./pipes 4 1000000`

### 4. Multithreading

```bash
./thread  
```
- Example: `./thread 4 1000000`

## Implementation Details

### Sequential
- Computes the sum in a single loop.
- No parallelism; slowest for large N[1][2].

### Multiprocessing (Shared Memory)
- Forks multiple child processes.
- Each process computes a partial sum and writes to a shared variable using locks for synchronization[3][2].
- Parent waits for all children, then multiplies the result by 4.

### Multiprocessing (Pipes)
- Forks processes, each computes a partial sum.
- Parent collects results via pipes and aggregates them[4][2].

### Multithreading
- Spawns multiple threads, each computes a partial sum.
- Uses mutexes to protect updates to the shared π variable[5][2].
- Fastest due to lower context-switching overhead.

## Performance Analysis

| Metric              | Sequential | Multiprocessing | Multithreading |
|---------------------|------------|-----------------|---------------|
| Execution Time      | Slow       | Moderate        | Fast          |
| Sync. Overhead      | None       | High            | Low           |
| Resource Utilization| Low        | High            | High          |
| Scalability         | Poor       | Moderate        | Excellent     |

- **Multithreading** outperforms other methods for large N due to efficient resource utilization and minimal synchronization overhead[2].
- **Multiprocessing** introduces more overhead due to process creation and inter-process communication.
- **Sequential** is simplest but least efficient for large computations.

## Synchronization

- **Mutex locks** (threads) and **atomic flags** (shared memory) ensure correctness when updating the shared π variable[5][3][2].
- Proper lock placement is critical to avoid race conditions and performance bottlenecks.

## Authors

- Muhammad Shakaib Arsalan (F2022266626)

## References

- See `Document.pdf` for detailed discussion, flowcharts, and further performance analysis[2].

## Notes

- All code assumes a POSIX system (Linux, macOS).
- For large N, multithreading is recommended for best performance.
- Output is written to `thread_results.txt` in the multithreaded version[5].

## Example Output

```
PI is: 3.1415926535
error: 0.0000000000
```
