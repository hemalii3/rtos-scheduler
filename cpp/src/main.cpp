#include "RMSScheduler.h"
#include "EDFScheduler.h"
#include "SocketServer.h"
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

// Parse a simple key=value request line
// Format: "algo=RMS&duration=100&tasks=T1:10:3,T2:20:5\n"
std::string handleRequest(const std::string& raw) {
    std::string algo     = "RMS";
    int         duration = 100;
    std::string tasks_str;

    std::istringstream ss(raw);
    std::string token;
    while (std::getline(ss, token, '&')) {
        auto eq = token.find('=');
        if (eq == std::string::npos) continue;
        std::string key = token.substr(0, eq);
        std::string val = token.substr(eq + 1);
        val.erase(std::remove(val.begin(), val.end(), '\n'), val.end());
        val.erase(std::remove(val.begin(), val.end(), '\r'), val.end());

        if (key == "algo")     algo      = val;
        if (key == "duration") duration  = std::stoi(val);
        if (key == "tasks")    tasks_str = val;
    }

    // Parse tasks — format: "name:period:exec,name:period:exec"
    std::vector<Task> tasks;
    int id = 1;
    std::istringstream ts(tasks_str);
    std::string taskToken;
    while (std::getline(ts, taskToken, ',')) {
        std::istringstream fs(taskToken);
        std::string name, period_s, exec_s;
        std::getline(fs, name,    ':');
        std::getline(fs, period_s,':');
        std::getline(fs, exec_s,  ':');
        if (name.empty() || period_s.empty() || exec_s.empty()) continue;
        Task t;
        t.id           = id++;
        t.name         = name;
        t.period_ms    = std::stoi(period_s);
        t.execution_ms = std::stoi(exec_s);
        t.deadline_ms  = t.period_ms;
        t.priority     = 0;
        tasks.push_back(t);
    }

    if (tasks.empty())
        return "{\"error\":\"no tasks provided\"}\n";

    std::vector<ScheduleEvent> rms_log;
    std::vector<EDFEvent>      edf_log;
    double util     = 0;
    bool   feasible = false;

    if (algo == "EDF") {
        EDFScheduler sched;
        for (auto& t : tasks) sched.addTask(t);
        sched.run(duration);
        edf_log  = sched.getLog();
        util     = sched.cpuUtilization();
        feasible = sched.isFeasible();
    } else {
        RMSScheduler sched;
        for (auto& t : tasks) sched.addTask(t);
        sched.run(duration);
        rms_log  = sched.getLog();
        util     = sched.cpuUtilization();
        feasible = sched.isFeasible();
    }

    std::ostringstream json;
    json << "{";
    json << "\"algo\":\"" << algo << "\",";
    json << "\"utilization\":" << util << ",";
    json << "\"feasible\":"    << (feasible ? "true" : "false") << ",";
    json << "\"events\":[";

    bool first = true;
    auto writeEvent = [&](int tick, int tid, const std::string& tname,
                          bool missed, bool idle) {
        if (!first) json << ",";
        first = false;
        json << "{\"tick\":"        << tick
             << ",\"task_id\":"     << tid
             << ",\"task_name\":\"" << tname << "\""
             << ",\"deadline_missed\":" << (missed ? "true" : "false")
             << ",\"idle\":"        << (idle ? "true" : "false")
             << "}";
    };

    if (algo == "EDF")
        for (auto& e : edf_log)
            writeEvent(e.tick, e.task_id, e.task_name,
                       e.deadline_missed, e.idle);
    else
        for (auto& e : rms_log)
            writeEvent(e.tick, e.task_id, e.task_name,
                       e.deadline_missed, e.idle);

    json << "]}";
    return json.str() + "\n";
}

int main() {
    std::cout << "=== RTOS Scheduler Server ===" << std::endl;
    SocketServer server(9090);
    server.start(handleRequest);
    return 0;
}
