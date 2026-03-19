#include "EDFScheduler.h"
#include <algorithm>
#include <limits>

void EDFScheduler::addTask(const Task& t) {
    tasks_.push_back(t);
}

bool EDFScheduler::isFeasible() const {
    return cpuUtilization() <= 1.0;
}

double EDFScheduler::cpuUtilization() const {
    double u = 0;
    for (auto& t : tasks_) u += t.utilization();
    return u;
}

void EDFScheduler::run(int duration_ms) {
    log_.clear();

    for (auto& t : tasks_) {
        t.remaining_ms      = t.execution_ms;
        t.next_release_ms   = 0;
        t.absolute_deadline = t.deadline_ms;
        t.deadline_missed   = false;
    }

    for (int tick = 0; tick < duration_ms; tick++) {

        // Release tasks
        for (auto& t : tasks_) {
            if (tick == t.next_release_ms) {
                t.remaining_ms      = t.execution_ms;
                t.absolute_deadline = tick + t.deadline_ms;
                t.deadline_missed   = false;
            }
        }

        // Check deadline misses
        for (auto& t : tasks_) {
            if (tick == t.absolute_deadline && t.remaining_ms > 0)
                t.deadline_missed = true;
        }

        // Pick task with earliest absolute deadline (EDF rule)
        Task* selected = nullptr;
        for (auto& t : tasks_) {
            if (t.remaining_ms > 0) {
                if (!selected ||
                    t.absolute_deadline < selected->absolute_deadline)
                    selected = &t;
            }
        }

        if (selected) {
            selected->remaining_ms--;
            log_.push_back({tick, selected->id, selected->name,
                            selected->deadline_missed, false});

            if (selected->remaining_ms == 0)
                selected->next_release_ms = tick + 1 +
                    (selected->period_ms - selected->execution_ms);
        } else {
            log_.push_back({tick, -1, "IDLE", false, true});
        }
    }
}

const std::vector<EDFEvent>& EDFScheduler::getLog() const {
    return log_;
}
