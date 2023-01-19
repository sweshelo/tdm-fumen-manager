#include <iostream>
#include <stdlib.h>
#include <string>
#include <inttypes.h>
#include <jansson.h>
#include <3ds.h>

#include "fs.hpp"

Result http_download(const char*, string);
void load_songlist(void);
void quitwait(void);
