#include <chrono>
#include <gtest/gtest.h>
#include <thread>

#include "timed_set.f"

TEST(TimedSetTest, AddAndContains) {
    TimedSet timedSet;

    // Add an item with a duration of 2 seconds
    timedSet.add("item1", std::chrono::seconds(2));

    // Check if the item is in the set immediately after adding
    EXPECT_TRUE(timedSet.contains("item1"));

    // Wait for 3 seconds to ensure the item expires
    std::this_thread::sleep_for(std::chrono::seconds(3));

    timedSet.cleanupExpiredItems();
    // Check if the item is no longer in the set after expiration
    EXPECT_FALSE(timedSet.contains("item1"));
}

TEST(TimedSetTest, CleanupExpiredItems) {
    TimedSet timedSet;

    // Add two items with different durations
    timedSet.add("item1", std::chrono::seconds(2));
    timedSet.add("item2", std::chrono::seconds(3));

    // Wait for 4 seconds to ensure both items expire
    std::this_thread::sleep_for(std::chrono::seconds(4));

    // Perform cleanup of expired items
    timedSet.cleanupExpiredItems();

    // Check if the set is empty after cleanup
    EXPECT_TRUE(timedSet.empty());
}

TEST(TimedSetTest, ContainsNonExistingItem) {
    TimedSet timedSet;

    // Add an item to the set
    timedSet.add("item1", std::chrono::seconds(2));

    // Check if a non-existing item is not in the set
    EXPECT_FALSE(timedSet.contains("non_existing_item"));
}
