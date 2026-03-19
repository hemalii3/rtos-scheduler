#pragma once
#include <string>
#include <chrono>

struct Task {
    int id;
    std::string name;
    int period_ms;        // how often task repeats
    int execution_ms;     // how long it runs
    int deadline_ms;      // relative deadline (usually = period)
    int priority;         // lower = higher priority (RMS)

    // Runtime state
    int remaining_ms      = 0;
    int next_release_ms   = 0;
    int absolute_deadline = 0;
    bool deadline_missed  = false;

    // Utilization = execution / period
    double utilization() const {
        return (double)execution_ms / (double)period_ms;
    }
};
