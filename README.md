# MiniRTOS Simulation

A lightweight C++ simulation of an RTOS (Real-Time Operating System) environment running on Linux.

## Overview
This project demonstrates how to simulate RTOS primitives like **Tasks** and **Queues** using standard C++ threading and synchronization mechanisms. It allows you to write code that looks and behaves like an embedded RTOS application but runs on a desktop environment.

## Features
- **MiniRTOS.h**: A header-only abstraction layer.
    - `Task`: Wrapper around `std::thread` with priority simulation.
    - `Queue`: Thread-safe queue for inter-task communication.
    - `Kernel`: Utilities for delays and system ticks.
- **Jitter Analysis**: A built-in task to measure timing accuracy and demonstrate the "Soft Real-Time" nature of the simulation.

## Building and Running

### Prerequisites
- GCC with C++11 support (or later)
- Linux environment (for `pthread`)

### Compile
```bash
g++ -pthread -o rtos_app rtos_app.cpp
```

### Run
```bash
./rtos_app
```

## Example Output
```
[System] Task Started: Sensor (Prio: 1)
[System] Task Started: Process (Prio: 2)
[SensorTask] Read: 24.5 C. Sending to queue...
  [ProcessingTask] Received ID: 1 Val: 24.5 Time: 100
      >>> [JitterTask] Max Jitter: 335 us, Avg: 50 us
```

## Real-Time Characteristics
This simulation runs on a standard Linux kernel, which is a General Purpose OS. Therefore, it provides **Soft Real-Time** guarantees. The `JitterTask` included in the application measures the deviation from expected wake-up times. You may observe jitter in the range of milliseconds, whereas a true Hard RTOS would have jitter in the microseconds range.
