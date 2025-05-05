#pragma once
#include <string>

void log_init();
void log_close();
void log_duplicateLast();

void loginfo(std::string a);
void logwarn(std::string a);
void logerr(std::string a);
void logprintf(const char* format, ...);
void logprintf(char* format, ...);