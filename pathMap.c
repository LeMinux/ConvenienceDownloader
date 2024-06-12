#include "./includes/pathMap.h"

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

static void sortMap(Map_t map){

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

void printPathMap(Map_t* pathMap){
	int p = 0;
	for(; p < pathMap->length; ++p) printf("%d> %s\n", p + 1, pathMap->map[p]);
}
