#pragma once
#include "globals.h"
#include "mathops.h"

//todo: move this to platform?
#if _WIN32

#include <windows.h>
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

inline std::string fetchTextFile(std::string url) {

	std::wstring wurl = convertStringOnWin32(url);

    URL_COMPONENTS urlComp{};
    wchar_t host[256], path[1024];
    urlComp.dwStructSize = sizeof(urlComp);
    urlComp.lpszHostName = host;
    urlComp.dwHostNameLength = _countof(host);
    urlComp.lpszUrlPath = path;
    urlComp.dwUrlPathLength = _countof(path);

    if (!WinHttpCrackUrl(wurl.c_str(), 0, 0, &urlComp)) {
        throw std::runtime_error("Invalid URL");
    }

    HINTERNET hSession = WinHttpOpen(L"voidsprite/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) throw std::runtime_error("WinHttpOpen failed");
	DoOnReturn a1([&]() {
		WinHttpCloseHandle(hSession);
	});

    HINTERNET hConnect = WinHttpConnect(hSession, urlComp.lpszHostName,
        urlComp.nPort, 0);
    if (!hConnect) {
        throw std::runtime_error("WinHttpConnect failed");
    }
	DoOnReturn a2([&]() {
		WinHttpCloseHandle(hConnect);
	});

    HINTERNET hRequest = WinHttpOpenRequest(hConnect,
        L"GET",
        urlComp.lpszUrlPath,
        NULL, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        (urlComp.nScheme == INTERNET_SCHEME_HTTPS)
        ? WINHTTP_FLAG_SECURE
        : 0);
    if (!hRequest) {
        throw std::runtime_error("WinHttpOpenRequest failed");
    }
    DoOnReturn a3([&]() {
        WinHttpCloseHandle(hRequest);
    });

    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        throw std::runtime_error("WinHttpSendRequest failed");
    }

    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        throw std::runtime_error("WinHttpReceiveResponse failed");
    }

    DWORD statusCode = 0, size = sizeof(statusCode);
    WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        NULL, &statusCode, &size, NULL);
    if (statusCode != 200) {
        throw std::out_of_range("HTTP status code: " + std::to_string(statusCode));
    }

    std::stringstream responseStream;
    DWORD bytesAvailable = 0;
    do {
        WinHttpQueryDataAvailable(hRequest, &bytesAvailable);
        if (bytesAvailable == 0) break;

        std::string buffer(bytesAvailable, '\0');
        DWORD bytesRead = 0;
        WinHttpReadData(hRequest, &buffer[0], bytesAvailable, &bytesRead);
        responseStream.write(buffer.data(), bytesRead);
    } while (bytesAvailable > 0);

    return responseStream.str();
}

#else


//todo: implement this with libcurl
inline std::string fetchTextFile(std::string url) {
	throw std::runtime_error("Network operations are not supported on this platform.");
}

#endif