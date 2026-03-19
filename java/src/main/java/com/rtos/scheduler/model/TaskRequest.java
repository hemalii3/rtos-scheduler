package com.rtos.scheduler.model;
import java.util.List;

public class TaskRequest {
    private String algo = "RMS";
    private int duration = 100;
    private List<TaskDef> tasks;

    public static class TaskDef {
        private String name;
        private int period;
        private int execution;
        public String getName()           { return name; }
        public void   setName(String n)   { this.name = n; }
        public int    getPeriod()         { return period; }
        public void   setPeriod(int p)    { this.period = p; }
        public int    getExecution()      { return execution; }
        public void   setExecution(int e) { this.execution = e; }
    }

    public String        getAlgo()                { return algo; }
    public void          setAlgo(String a)         { this.algo = a; }
    public int           getDuration()             { return duration; }
    public void          setDuration(int d)        { this.duration = d; }
    public List<TaskDef> getTasks()                { return tasks; }
    public void          setTasks(List<TaskDef> t) { this.tasks = t; }
}
