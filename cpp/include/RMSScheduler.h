#pragma once
#include "Task.h"
#include <vector>
#include <string>

struct ScheduleEvent {
    int  tick;
    int  task_id;
    std::string task_name;
    bool deadline_missed;
    bool idle;
};

class RMSScheduler {
public:
    void addTask(const Task& t);
    void run(int duration_ms);
    const std::vector<ScheduleEvent>& getLog() const;
    double cpuUtilization() const;
    bool isFeasible() const;         // Liu & Layland bound

private:
    std::vector<Task>          tasks_;
    std::vector<ScheduleEvent> log_;
    void assignPriorities();         // shortest period = highest priority
};
