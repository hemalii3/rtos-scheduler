#include "RMSScheduler.h"
#include <algorithm>
#include <cmath>
#include <limits>

void RMSScheduler::addTask(const Task& t) {
    tasks_.push_back(t);
}

// Liu & Layland: U <= n*(2^(1/n) - 1)
bool RMSScheduler::isFeasible() const {
    int n = tasks_.size();
    if (n == 0) return true;
    double bound = n * (std::pow(2.0, 1.0 / n) - 1.0);
    return cpuUtilization() <= bound;
}

double RMSScheduler::cpuUtilization() const {
    double u = 0;
    for (auto& t : tasks_) u += t.utilization();
    return u;
}

void RMSScheduler::assignPriorities() {
    // Shorter period = higher priority (lower number)
    auto sorted = tasks_;
    std::sort(sorted.begin(), sorted.end(),
        [](const Task& a, const Task& b){
            return a.period_ms < b.period_ms;
        });
    for (int i = 0; i < (int)sorted.size(); i++)
        sorted[i].priority = i + 1;
    tasks_ = sorted;
}

void RMSScheduler::run(int duration_ms) {
    assignPriorities();
    log_.clear();

    // Init runtime state
    for (auto& t : tasks_) {
        t.remaining_ms      = t.execution_ms;
        t.next_release_ms   = 0;
        t.absolute_deadline = t.deadline_ms;
        t.deadline_missed   = false;
    }

    for (int tick = 0; tick < duration_ms; tick++) {

        // Release tasks whose period has come
        for (auto& t : tasks_) {
            if (tick == t.next_release_ms) {
                t.remaining_ms      = t.execution_ms;
                t.absolute_deadline = tick + t.deadline_ms;
                t.deadline_missed   = false;
            }
        }

        // Check deadline misses
        for (auto& t : tasks_) {
            if (tick == t.absolute_deadline && t.remaining_ms > 0) {
                t.deadline_missed = true;
            }
        }

        // Pick highest-priority ready task (lowest priority number)
        Task* selected = nullptr;
        for (auto& t : tasks_) {
            if (t.remaining_ms > 0) {
                if (!selected || t.priority < selected->priority)
                    selected = &t;
            }
        }

        if (selected) {
            selected->remaining_ms--;
            log_.push_back({tick, selected->id, selected->name,
                            selected->deadline_missed, false});

            // Schedule next release
            if (selected->remaining_ms == 0)
                selected->next_release_ms = tick + 1 +
                    (selected->period_ms - selected->execution_ms);
        } else {
            log_.push_back({tick, -1, "IDLE", false, true});
        }
    }
}

const std::vector<ScheduleEvent>& RMSScheduler::getLog() const {
    return log_;
}
