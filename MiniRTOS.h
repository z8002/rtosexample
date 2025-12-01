#pragma once

#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <chrono>
#include <atomic>
#include <string>

// Namespace to encapsulate our "RTOS" primitives
namespace MiniRTOS {

    // --- Queue Implementation ---
    // A thread-safe queue similar to xQueueCreate/xQueueSend/xQueueReceive
    template <typename T>
    class Queue {
    public:
        void send(const T& item) {
            std::unique_lock<std::mutex> lock(mutex_);
            queue_.push(item);
            cond_var_.notify_one();
        }

        // Returns true if item received, false if timeout
        bool receive(T& item, int timeout_ms = -1) {
            std::unique_lock<std::mutex> lock(mutex_);
            if (timeout_ms < 0) {
                // Wait indefinitely
                cond_var_.wait(lock, [this] { return !queue_.empty(); });
                item = queue_.front();
                queue_.pop();
                return true;
            } else {
                // Wait with timeout
                if (cond_var_.wait_for(lock, std::chrono::milliseconds(timeout_ms), [this] { return !queue_.empty(); })) {
                    item = queue_.front();
                    queue_.pop();
                    return true;
                }
                return false;
            }
        }

    private:
        std::queue<T> queue_;
        std::mutex mutex_;
        std::condition_variable cond_var_;
    };

    // --- Task Implementation ---
    // A wrapper around std::thread to simulate an RTOS task
    class Task {
    public:
        using TaskFunction = std::function<void()>;

        Task(const std::string& name, int priority, TaskFunction func)
            : name_(name), priority_(priority), func_(func), running_(false) {}

        void start() {
            running_ = true;
            thread_ = std::thread([this]() {
                // Simulate setting priority (best effort on Linux without root)
                // In a real RTOS, this would be strict.
                // On Linux, we can try to set nice value or pthread priority if permitted.
                std::cout << "[System] Task Started: " << name_ << " (Prio: " << priority_ << ")" << std::endl;
                func_();
                std::cout << "[System] Task Ended: " << name_ << std::endl;
            });
        }

        void join() {
            if (thread_.joinable()) {
                thread_.join();
            }
        }

        const std::string& getName() const { return name_; }

    private:
        std::string name_;
        int priority_;
        TaskFunction func_;
        std::thread thread_;
        std::atomic<bool> running_;
    };

    // --- Scheduler / Kernel ---
    class Kernel {
    public:
        static void startScheduler() {
            std::cout << "[System] Scheduler Started. (Blocking main thread)" << std::endl;
            // In a real RTOS, this never returns.
            // Here, we just block until all tasks are done (which might be never).
            // For simulation, we'll just sleep or join tasks if we had a list of them.
            // Since we don't track all tasks globally in this simple sim, 
            // the user is responsible for joining them or keeping main alive.
            
            // To simulate "Scheduler taking over", we can just sleep forever.
            while (true) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        
        static void delay(int ms) {
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        }
        
        static uint32_t getTickCount() {
             using namespace std::chrono;
             return (uint32_t)duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
        }
    };

} // namespace MiniRTOS
