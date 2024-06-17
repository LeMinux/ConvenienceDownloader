#include "../includes/pathMap.h"
#include <ctype.h>

//credit to this post https://stackoverflow.com/questions/1723002/how-to-list-all-subdirectories-in-a-given-directory-in-c#1723583
static void getSubdirectories(Map_t* map, const char* basePath){
	/* Check input parameters. */
	if(basePath == NULL) return;

	struct dirent* direntp = NULL;
	DIR* dirp = NULL;
	size_t pathLength = 0;
	pathLength = strlen(basePath);
	if(pathLength == 0 || pathLength > _POSIX_PATH_MAX) return;

	dirp = opendir(basePath);
	if(dirp == NULL) return;

	while((direntp = readdir(dirp)) != NULL){
		/* Ignore special directories. */
		if((strcmp(direntp->d_name, ".") == 0) ||
			(strcmp(direntp->d_name, "..") == 0))
			continue;

		char fullPath [_POSIX_PATH_MAX] = "";
		size_t fullLength = pathLength + strlen(direntp->d_name);

		//Calculate full name, check we are in file length limts
		if(basePath[pathLength - 1] != '/'){
			//+1 for nul byte +1 for '/'
			if(fullLength + 2 > _POSIX_PATH_MAX){
				(void)fprintf(stderr, "File path exceeds %d: %s/%s\n", _POSIX_PATH_MAX, basePath, direntp->d_name);
				continue;
			}
			(void)strcpy(fullPath, basePath);
			(void)strcat(fullPath, "/");
			++fullLength;
		}else{
			//+1 for nul byte
			if(fullLength + 1 > _POSIX_PATH_MAX){
				(void)fprintf(stderr, "File path exceeds %d: %s%s\n", _POSIX_PATH_MAX, basePath, direntp->d_name);
				continue;
			}
			(void)strcpy(fullPath, basePath);
		}
		(void)strcat(fullPath, direntp->d_name);
		fullPath[fullLength] = '\0';

		struct stat fstat = {0};
		if(stat(fullPath, &fstat) < 0)
			continue;

		if(S_ISDIR(fstat.st_mode))
			getSubdirectories(map, fullPath);
	}//end while

	//this does make the ordering reversed, but getting directories is
	//not in order anyway

	//increase size of header to point to another string
	map->map = realloc(map->map, sizeof(*map->map) * (map->length + 1));
	if(map->map == NULL){
		PRINT_ERROR(FAILED_MALLOC_MSG);
		exit(EXIT_FAILURE);
	}

	//allocate space for string
	map->map[map->length] = malloc(pathLength + 1);

	if(map->map[map->length] == NULL){
		PRINT_ERROR(FAILED_MALLOC_MSG);
		exit(EXIT_FAILURE);
	}
	(void)strcpy(map->map[map->length], basePath);
	map->map[map->length][pathLength] = '\0';
	++map->length;
	(void)closedir(dirp);
}

char* getSelection(MapArray_t* mapsOfSubDirs, const char* prompt, int isSkipping){
	int maxInput = 0;
	int c = 0;
	for(; c < mapsOfSubDirs->length; ++c)
		maxInput += mapsOfSubDirs->mapArray[c].length;

	//will accept 4 digits
	//of course there is always the possibility of someone having
	//9999 + 1 directories from just one mapArray
	char buffer [5] = "";
	char* endInput = NULL;
	int valid = 0;
	int selection = 0;

	printMapArray(mapsOfSubDirs);
	do{
		(void)puts(prompt);
		(void)exactInput(stdin, buffer, 5);
		selection = strtol(buffer, &endInput, 0);
		//check if it converted the whole thing
		if((*endInput == '\0') && errno != ERANGE){
			if(selection <= maxInput && selection > 0)
				valid = 1;
			else
				(void)printf(PNT_RED"Invalid input! Enter a number between 1 and %d\n"PNT_RESET, maxInput);
		//could be a string
		}else{
			int l = 0;
			for(; l < 4; ++l) buffer[l] = tolower(buffer[l]);
			if(isSkipping && strcmp(buffer, "skip") == 0)
				return "SKIP";
			else if(strcmp(buffer, "exit") == 0)
				return "EXIT";
			else
				(void)printf(PNT_RED"Invalid input! Enter a number between 1 and %d\n"PNT_RESET, maxInput);

		}
	}while(!valid);

	c = 0;
	while(selection > mapsOfSubDirs->mapArray[c].length){
		selection -= mapsOfSubDirs->mapArray[c].length;
		++c;
	}
	return mapsOfSubDirs->mapArray[c].map[selection - 1];
}

Map_t* obtainPathMap(const char* initialDirPath){
	if(initialDirPath == NULL) return NULL;

	Map_t* retMap = calloc(1, sizeof(*retMap));
	if(retMap == NULL){
		PRINT_ERROR(FAILED_MALLOC_MSG);
		exit(EXIT_FAILURE);
	}
	getSubdirectories(retMap, initialDirPath);
	return retMap;
}

void freePathMap(Map_t* pathMap){
	int f = 0;
	for(; f < pathMap->length; ++f){
		free(pathMap->map[f]);
		pathMap->map[f] = NULL;
	}
	free(pathMap);
	pathMap = NULL;
}

void printMapArray(MapArray_t* arrayOfMaps){
	int offset = 0;
	int m = 0;
	for(; m < arrayOfMaps->length; ++m){
		Map_t map = arrayOfMaps->mapArray[m];
		int p = 0;
		for(; p < map.length; ++p){
			(void)printf("%d> %s\n", p + 1 + offset, map.map[p]);
		}
		offset += map.length;
	}
}

void printPathMap(Map_t* pathMap){
	int p = 0;
	for(; p < pathMap->length; ++p) (void)printf("%d> %s\n", p + 1, pathMap->map[p]);
}
