#ifndef settings
#define settings
#include <unordered_map>
#include <iostream>

extern void read_env();
extern std::unordered_map<std::string, std::string> ENV_MAP;

#endif