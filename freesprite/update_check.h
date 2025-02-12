#pragma once

#ifndef GIT_COMMIT
#define GIT_COMMIT "unknown"
#endif

inline void checkUpdates() {
#if _WIN32
    FILE* f = platformOpenFile(L"current-ver", L"w");
    fprintf(f, "%s", GIT_COMMIT);
    fclose(f);
    std::string updaterPath = g_programDirectory + "\\vsp-updater.exe";
	int ec = platformRunProgramAndGetExitCode(convertStringOnWin32(updaterPath), L"");
	switch (ec) {
        case 0:
            g_addNotification(SuccessNotification("Update", "No updates available."));
            break;
        case 1:
            g_addNotification(SuccessNotification("Update", "Update successful."));
            break;
        case -1:
            g_addNotification(ErrorNotification("Update", "Update failed."));
            break;
        case -2:
            g_addNotification(ErrorNotification("Update", "Update failed."));
            break;
	}
#endif
}