package com.rtos.scheduler.service;

import com.rtos.scheduler.model.TaskRequest;
import org.springframework.stereotype.Service;
import java.io.*;
import java.net.Socket;
import java.util.stream.Collectors;

@Service
public class SchedulerClient {
    private static final String HOST = "localhost";
    private static final int    PORT = 9090;

    public String runSchedule(TaskRequest req) throws IOException {
        String tasks = req.getTasks().stream()
            .map(t -> t.getName() + ":" + t.getPeriod() + ":" + t.getExecution())
            .collect(Collectors.joining(","));

        String message = "algo=" + req.getAlgo()
                       + "&duration=" + req.getDuration()
                       + "&tasks=" + tasks + "\n";

        try (Socket socket = new Socket(HOST, PORT);
             PrintWriter out   = new PrintWriter(socket.getOutputStream(), true);
             BufferedReader in = new BufferedReader(
                                   new InputStreamReader(socket.getInputStream()))) {
            out.print(message);
            out.flush();
            socket.shutdownOutput();
            return in.lines().collect(Collectors.joining("\n"));
        }
    }
}
