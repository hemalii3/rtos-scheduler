package com.rtos.scheduler.controller;

import com.rtos.scheduler.model.TaskRequest;
import com.rtos.scheduler.service.SchedulerClient;
import org.springframework.messaging.simp.SimpMessagingTemplate;
import org.springframework.web.bind.annotation.*;

@RestController
@RequestMapping("/api")
@CrossOrigin(origins = "*")
public class SchedulerController {
    private final SchedulerClient       client;
    private final SimpMessagingTemplate broker;

    public SchedulerController(SchedulerClient client,
                                SimpMessagingTemplate broker) {
        this.client = client;
        this.broker = broker;
    }

    @PostMapping("/schedule")
    public String schedule(@RequestBody TaskRequest req) throws Exception {
        String result = client.runSchedule(req);
        broker.convertAndSend("/topic/schedule", result);
        return result;
    }

    @GetMapping("/health")
    public String health() {
        return "{\"status\":\"ok\"}";
    }
}
