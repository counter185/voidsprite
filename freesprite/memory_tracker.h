#pragma once


inline std::map<std::string, u64> g_named_memmap;
inline std::map<void*, std::pair<u64, std::string>> allocated_mems;

inline void* tracked_malloc(u64 bytes, std::string name = "unk") {
	void* ret = malloc(bytes);
#if _DEBUG
	if (ret != NULL) {
        allocated_mems[ret] = { bytes, name };
        g_named_memmap[name] += bytes;
	}
#endif
	return ret;
}

inline void tracked_free(void *ptr) {
    if (ptr != NULL) {
#if _DEBUG
        g_named_memmap[allocated_mems[ptr].second] -= allocated_mems[ptr].first;
        allocated_mems.erase(ptr);
#endif
        free(ptr);
    }
}

inline std::string bytesToFriendlyString(u64 bytes) {
    std::string suffixes[] = { "B", "KB", "MB", "GB", "TB" };
    double bytesf = (double)bytes;
    int suffixIndex = 0;
    while (bytesf >= 1024 && suffixIndex < 4) {
        bytesf /= 1024;
        suffixIndex++;
    }
    return std::format("{:.4f}{}", bytesf, suffixes[suffixIndex]);
}