#include <iostream>
#include <string>
#include <random>
#include "MiniRTOS.h"

// Define a message structure for our queue
struct SensorData {
    int id;
    float value;
    uint32_t timestamp;
};

// Global handles (simulating RTOS handles)
MiniRTOS::Queue<SensorData> sensorQueue;

// --- Task 1: Sensor Task (Producer) ---
void vSensorTask() {
    int id = 0;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(20.0, 30.0);

    while (true) {
        // Simulate reading a sensor
        float temp = dis(gen);
        SensorData data = { ++id, temp, MiniRTOS::Kernel::getTickCount() };

        std::cout << "[SensorTask] Read: " << temp << " C. Sending to queue..." << std::endl;
        sensorQueue.send(data);

        // Run at 1Hz (1000ms delay)
        MiniRTOS::Kernel::delay(1000);
    }
}

// --- Task 2: Processing Task (Consumer) ---
void vProcessingTask() {
    SensorData receivedData;
    while (true) {
        // Wait for data from the queue (Blocking wait)
        if (sensorQueue.receive(receivedData)) {
            std::cout << "  [ProcessingTask] Received ID: " << receivedData.id 
                      << " Val: " << receivedData.value 
                      << " Time: " << receivedData.timestamp << std::endl;
            
            // Simulate processing time
            MiniRTOS::Kernel::delay(200);
        }
    }
}

// --- Task 3: Monitor Task (High Priority) ---
void vMonitorTask() {
    while (true) {
        std::cout << "    [MonitorTask] System Heartbeat OK." << std::endl;
        MiniRTOS::Kernel::delay(5000); // Run every 5 seconds
    }
}

// --- Task 4: Jitter Analysis Task ---
void vJitterTask() {
    using namespace std::chrono;
    auto next_wake_time = steady_clock::now();
    const auto period = milliseconds(10); // 100Hz

    long long max_jitter = 0;
    long long total_jitter = 0;
    long count = 0;

    std::cout << "[JitterTask] Starting 100Hz loop..." << std::endl;

    while (true) {
        next_wake_time += period;
        std::this_thread::sleep_until(next_wake_time);

        auto actual_time = steady_clock::now();
        // Jitter = Actual - Expected
        auto jitter = duration_cast<microseconds>(actual_time - next_wake_time).count();

        // Only track positive jitter (lateness). Early wakeups (negative) are rare with sleep_until but possible.
        if (jitter < 0) jitter = 0; 

        if (jitter > max_jitter) max_jitter = jitter;
        total_jitter += jitter;
        count++;

        if (count >= 100) { // Every 1 second (100 * 10ms)
            std::cout << "      >>> [JitterTask] Max Jitter: " << max_jitter << " us, Avg: " << (total_jitter / count) << " us" << std::endl;
            max_jitter = 0; // Reset max for next window
            total_jitter = 0;
            count = 0;
        }
    }
}

int main() {
    std::cout << "--- MiniRTOS Simulation Starting ---" << std::endl;

    // Create Tasks
    // Priorities: Higher number = Higher priority (Conceptual in this sim)
    MiniRTOS::Task task1("Sensor", 1, vSensorTask);
    MiniRTOS::Task task2("Process", 2, vProcessingTask);
    MiniRTOS::Task task3("Monitor", 3, vMonitorTask);
    MiniRTOS::Task task4("Jitter", 4, vJitterTask);

    // Start Tasks
    task1.start();
    task2.start();
    task3.start();
    task4.start();

    // Start Scheduler (Blocking)
    // In this simulation, main thread effectively becomes the idle task or scheduler loop
    // But since our startScheduler is infinite loop, we can call it.
    // Alternatively, we can just join the threads here to keep main alive.
    
    task1.join();
    task2.join();
    task3.join();
    task4.join();

    return 0;
}
