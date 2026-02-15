#ifndef COVERART_H
#define COVERART_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "execOthers.h"

#define TEMP_FILE "/tmp/newly_added_art.mp3"

enum ERROR writeCover(const char* file_with_path, const char* cover_path);

#endif
