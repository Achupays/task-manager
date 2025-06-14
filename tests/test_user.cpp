#include <gtest/gtest.h>
#include "../include/User.h"
#include "../include/Task.h"

TEST(UserTests, AddTaskIncreasesSize) {
    User user("test_user");
    Task task{"Title", "Desc", Priority::Medium, Status::Active, "2030-01-01 12:00", {"tag"}};
    size_t before = user.get_tasks().size();

    user.add_task(task);

    EXPECT_EQ(user.get_tasks().size(), before + 1);
    EXPECT_EQ(user.get_tasks().back().title, "Title");
}

TEST(UserTests, DeleteValidTask) {
    User user("test_user");
    user.add_task(Task{"A", "B", Priority::Low, Status::Done, "2030-01-01 10:00", {}});
    EXPECT_EQ(user.get_tasks().size(), 1);

    user.delete_task(0);
    EXPECT_TRUE(user.get_tasks().empty());
}

TEST(UserTests, DeleteInvalidTaskIndexDoesNothing) {
    User user("test_user");
    user.delete_task(999); // Should not crash
    EXPECT_TRUE(user.get_tasks().empty());
}

TEST(UserTests, SearchFindsMatchingTasks) {
    User user("test_user");
    user.add_task(Task{"Buy milk", "From store", Priority::High, Status::Active, "2030-01-01 12:00", {}});
    user.add_task(Task{"Do homework", "Math", Priority::Low, Status::Done, "2030-01-02 12:00", {}});

    auto results = user.search_tasks("milk");

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].title, "Buy milk");
}

TEST(UserTests, SearchFindsNothingIfNoMatch) {
    User user("test_user");
    user.add_task(Task{"Call mom", "Evening", Priority::Low, Status::Active, "2030-01-01 12:00", {}});
    
    auto results = user.search_tasks("nonexistent");

    EXPECT_TRUE(results.empty());
}
