#include <gtest/gtest.h>
#include "RMSScheduler.h"
#include "EDFScheduler.h"

// ── RMS Tests ─────────────────────────────────────────────

TEST(RMSTest, FeasibilityCheck) {
    RMSScheduler s;
    s.addTask({1, "T1", 20, 4, 20, 0});   // util = 0.20
    s.addTask({2, "T2", 50, 10, 50, 0});  // util = 0.20
    // total = 0.40, well under Liu & Layland bound ~0.828
    EXPECT_TRUE(s.isFeasible());
}

TEST(RMSTest, OverloadedNotFeasible) {
    RMSScheduler s;
    s.addTask({1, "T1", 10, 9, 10, 0});   // util = 0.90
    s.addTask({2, "T2", 20, 9, 20, 0});   // util = 0.45
    // total = 1.35 > 1.0
    EXPECT_FALSE(s.isFeasible());
}

TEST(RMSTest, LogLengthMatchesDuration) {
    RMSScheduler s;
    s.addTask({1, "T1", 10, 3, 10, 0});
    s.addTask({2, "T2", 20, 5, 20, 0});
    s.run(100);
    EXPECT_EQ((int)s.getLog().size(), 100);
}

TEST(RMSTest, NoDeadlineMissOnFeasibleSet) {
    RMSScheduler s;
    s.addTask({1, "T1", 20, 4, 20, 0});
    s.addTask({2, "T2", 50, 10, 50, 0});
    s.run(200);
    for (auto& e : s.getLog())
        EXPECT_FALSE(e.deadline_missed) << "Missed at tick " << e.tick;
}

// ── EDF Tests ─────────────────────────────────────────────

TEST(EDFTest, FeasibleUnderFullLoad) {
    EDFScheduler s;
    s.addTask({1, "T1", 10, 5, 10, 0});  // util = 0.5
    s.addTask({2, "T2", 20, 10, 20, 0}); // util = 0.5
    // total = 1.0 — exactly feasible for EDF
    EXPECT_TRUE(s.isFeasible());
}

TEST(EDFTest, LogLengthMatchesDuration) {
    EDFScheduler s;
    s.addTask({1, "T1", 10, 3, 10, 0});
    s.addTask({2, "T2", 20, 5, 20, 0});
    s.run(100);
    EXPECT_EQ((int)s.getLog().size(), 100);
}

TEST(EDFTest, NoDeadlineMissOnFeasibleSet) {
    EDFScheduler s;
    s.addTask({1, "T1", 20, 4, 20, 0});
    s.addTask({2, "T2", 50, 10, 50, 0});
    s.run(200);
    for (auto& e : s.getLog())
        EXPECT_FALSE(e.deadline_missed) << "Missed at tick " << e.tick;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
