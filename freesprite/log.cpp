#include "globals.h"
#include "mathops.h"

FILE* logFile = NULL;
PlatformNativePathString logPath;

void log_init()
{
    if (logFile == NULL) {
        logPath = platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("/voidsprite_log.txt");
        logFile = platformOpenFile(logPath, convertStringOnWin32("w"));
        if (logFile == NULL) {
            printf("Failed to open log file\n");
        }
        else {
            loginfo("Log file initialized");
        }
    }
    else {
        logwarn("Attempted to initialize logging when already initialized.");
    }
}

void log_duplicateLast() {
    bool logExists = std::filesystem::exists(logPath);
    if (!logPath.empty() && logExists) {
        time_t t = time(NULL);
        std::tm tmn = getLocalTime();
        std::string date = frmt("{:04d}-{:02d}-{:02d}--{:02d}-{:02d}-{:02d}", tmn.tm_year + 1900, tmn.tm_mon + 1, tmn.tm_mday, tmn.tm_hour, tmn.tm_min, tmn.tm_sec);
        PlatformNativePathString newLogPath = platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32(frmt("/voidsprite_log-{}.txt", date));
        platformCopyFile(logPath, newLogPath);
    }
}

void log_close() {
    if (logFile != NULL) {
        fclose(logFile);
        logFile = NULL;
    }
}

void loginfo_sync(std::string a)
{
    loginfo(a);
    if (logFile != NULL) {
        fflush(logFile);
    }
}

void loginfo(std::string a)
{
    if (logFile != NULL) {
        fwrite("[I] ", 1, 4, logFile);
        fwrite(a.c_str(), sizeof(char), a.size(), logFile);
        fputc('\n', logFile);
    }
    printf("[I] %s\n", a.c_str());
}

void logwarn(std::string a)
{
    if (logFile != NULL) {
        fwrite("[W] ", 1, 4, logFile);
        fwrite(a.c_str(), sizeof(char), a.size(), logFile);
        fputc('\n', logFile);
    }
    printf("[W] %s\n", a.c_str());
}

void logerr(std::string a)
{
    if (logFile != NULL) {
        fwrite("[E] ", 1, 4, logFile);
        fwrite(a.c_str(), sizeof(char), a.size(), logFile);
        fputc('\n', logFile);
    }
    printf("[E] %s\n", a.c_str());
}

void logprintf(const char* format, ...) 
{
    if (logFile != NULL) {
        va_list args;
        va_start(args, format);
        fflush(logFile);
        vfprintf(logFile, format, args);
        vprintf(format, args);
        va_end(args);
    }
}

void logprintf(char* format, ...)
{
    if (logFile != NULL) {
        va_list args;
        va_start(args, format);
        fflush(logFile);
        vfprintf(logFile, format, args);
        vprintf(format, args);
        va_end(args);
    }
}

void loghexdump(void* data, int bytesInLine, int lines)
{
    u8* d = (u8*)data;
    for (int i = 0; i < lines; i++) {
        std::string line = "";
        for (int j = 0; j < bytesInLine; j++) {
            line += frmt("{:02X} ", d[i * bytesInLine + j]);
        }
        logprintf("%s\n", line.c_str());
    }
}
