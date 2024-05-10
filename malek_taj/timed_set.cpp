#include <chrono>
#include <iostream>

#include "timed_set.f"

void TimedSet::add(const std::string& item, std::chrono::seconds duration) {
    auto expiration_time = std::chrono::steady_clock::now() + duration;
    items.emplace(expiration_time, item);
}

void TimedSet::cleanupExpiredItems() {
    auto now = std::chrono::steady_clock::now();
    while (!items.empty() && items.begin()->first < now) {
        items.erase(items.begin());
    }
}

bool TimedSet::contains(const std::string& item) const {
    for (const auto& pair : items) {
        if (pair.second == item) {
            return true;
        }
    }
    return false;
}

bool TimedSet::empty() const {
    if (items.empty())
        return true;
    return false;
}
