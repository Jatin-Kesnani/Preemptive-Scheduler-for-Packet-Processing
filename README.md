# Preemptive Scheduler for Packet Processing

A multi-threaded C++ implementation of various preemptive CPU scheduling algorithms for packet processing, featuring SRTF, Priority, LRTF, and Round Robin scheduling methods.

## Table of Contents
- [Features](#features)
- [Algorithms Implemented](#algorithms-implemented)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)

## Features

- **Four Scheduling Algorithms**:
  - Shortest Remaining Time First (SRTF)
  - Priority Scheduling
  - Longest Remaining Time First (LRTF)
  - Round Robin (RR)
  
- **Interactive Menu System** with ASCII art introduction
- **Detailed Output** including:
  - Process/Packet information tables
  - Gantt charts for visualization
  - Performance metrics (waiting time, turnaround time)
- **Multi-threaded Implementation** using pthreads
- **Synchronization** with semaphores
- **Color-coded** terminal output

## Algorithms Implemented

1. **Shortest Remaining Time First (SRTF)**
   - Preempts the currently running process if a new process arrives with a shorter burst time
   - Minimizes average waiting time

2. **Priority Scheduling**
   - Executes processes based on priority (lower number = higher priority)
   - Preempts when a higher priority process arrives

3. **Longest Remaining Time First (LRTF)**
   - Opposite of SRTF, prioritizes processes with longest remaining time
   - Demonstrates worst-case scheduling scenario

4. **Round Robin (RR)**
   - Uses time quantum for fair allocation
   - Preempts after fixed time slices

## Requirements

- Linux/Unix environment (for pthreads and unistd.h)
- g++ compiler
- Basic C++ libraries (iostream, vector, etc.)
- pthread library
- semaphore.h (part of POSIX)

## Installation

1. Clone the repository:
```bash
git clone https://github.com/Jatin-Kesnani/Preemptive-Scheduler-for-Packet-Processing.git
cd preemptive-scheduler
```

## Usage

### Compile the Program
Use the following command to compile the program:

```bash
g++ -pthread -o scheduler scheduler.cpp
```
