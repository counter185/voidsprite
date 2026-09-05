#pragma once

#include <regex>

#include "globals.h"
#include "UILabel.h"

struct GitData {
	bool found = false;
	std::string repoName;
	std::string friendlyRepoName;
	std::string branch;
};

inline GitData tryFindGitDataFromFile(PlatformNativePathString f) {
	try {
		std::filesystem::path p = f;
		while (p.has_parent_path() && p != p.parent_path()) {
			p = p.parent_path();
			auto gitRoot = p / ".git";
			if (std::filesystem::is_directory(gitRoot)) {
				GitData g{true};
				g.repoName = p.filename().string();
				
				std::regex urlMatch("\\s*url\\s*=\\s*https?:\\/\\/(.+)");
				std::regex remoteUrlMatch("(github\\.com|codeberg\\.org|gitlab\\.com)\\/([a-zA-Z0-9_\\.\\-]+)\\/([a-zA-Z0-9_\\-]+)");
				std::regex refMatch("ref:\\s*refs\\/heads\\/(.+)");
				std::smatch m;

				auto gitConfig = gitRoot / "config";
				if (std::filesystem::exists(gitConfig)) {
					std::ifstream configFile(gitConfig);
					if (configFile.is_open()) {
						std::string line = "";
						while (std::getline(configFile, line)) {
							if (std::regex_match(line, m, urlMatch)) {
								std::smatch remoteMatch;
								std::string url = m[0].str();
								if (std::regex_search(url, remoteMatch, remoteUrlMatch)) {
									g.friendlyRepoName = frmt("{}/{}", remoteMatch[2].str(), remoteMatch[3].str());
								}
								break;
							}
						}
					}
				}
				auto gitHead = gitRoot / "HEAD";
				if (std::filesystem::exists(gitHead)) {
					std::ifstream headFile(gitHead);
					if (headFile.is_open()) {
						std::string line = "";
						std::getline(headFile, line);
						if (std::regex_search(line, m, refMatch)) {
							g.branch = m[1];
						}
					}
				}

				if (g.friendlyRepoName == "") {
					g.friendlyRepoName = g.repoName;
				}
				return g;
			}
		}
	}
	catch (std::exception&) {
		return {};
	}
	return {};
}