#pragma once
#include <iostream>
#include <stdlib.h>
#include <string>
#include <inttypes.h>
#include <jansson.h>
#include <3ds.h>

#include <curl/curl.h>
#include <malloc.h>
#include <regex>
#include <string>
#include <unistd.h>
#include <dirent.h>

#include "fs.hpp"

Result downloadToFile(const std::string &url, const std::string &path);

Result http_download(const char*, string);
void load_songlist(void);
void quitwait(void);

std::string readFileIntoString(const string& path);
