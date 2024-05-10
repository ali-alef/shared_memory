#ifndef timed_set
#define timed_set

#include <set>
#include <chrono>

class TimedSet {
public:
    void add(const std::string& item, std::chrono::seconds duration);
    void cleanupExpiredItems();
    bool contains(const std::string& item) const;
    bool empty() const;

private:
    std::set<std::pair<std::chrono::steady_clock::time_point, std::string>> items;
};

#endif