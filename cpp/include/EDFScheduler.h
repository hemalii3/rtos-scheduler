#pragma once
#include "Task.h"
#include <vector>

struct EDFEvent {
    int  tick;
    int  task_id;
    std::string task_name;
    bool deadline_missed;
    bool idle;
};

class EDFScheduler {
public:
    void addTask(const Task& t);
    void run(int duration_ms);
    const std::vector<EDFEvent>& getLog() const;
    double cpuUtilization() const;
    bool isFeasible() const;

private:
    std::vector<Task>     tasks_;
    std::vector<EDFEvent> log_;
};
