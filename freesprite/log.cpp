#include "globals.h"

FILE* logFile = NULL;

void log_init()
{
	PlatformNativePathString logPath = platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("/voidsprite_log.txt");
	logFile = platformOpenFile(logPath, convertStringOnWin32("w"));
	if (logFile == NULL) {
		printf("Failed to open log file\n");
	}
	else {
		loginfo("Log file initialized");
	}
}
void log_close() {
	if (logFile != NULL) {
		fclose(logFile);
		logFile = NULL;
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

void logprintf(const char* format, ...) {
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
		vfprintf(logFile, format, args);
		va_end(args);
	}
	printf(format);
}
