#include "./includes/pathsMap.h"

//credit to this post https://stackoverflow.com/questions/1723002/how-to-list-all-subdirectories-in-a-given-directory-in-c#1723583
static void getSubdirectories(Map_t* map, const char* basePath){
	/* Check input parameters. */
	if(basePath == NULL) return;

	struct dirent* direntp = NULL;
	DIR* dirp = NULL;
	size_t pathLength = 0;

	//This determines if a slash is needed and fits
	//it is done here so that it doesn't create the large array
	//just for it to fail existing as a directory if that happens
	pathLength = strlen(basePath);
	if(pathLength == 0) return;

	int catMode = -1;
	if(basePath[pathLength - 1] != '/'){
		if(pathLength + 2 > _POSIX_PATH_MAX) return;
		++pathLength;
		catMode = ADD_SLASH;
	}else{
		if(pathLength + 1 > _POSIX_PATH_MAX) return;
		catMode = NO_SLASH;
	}

	//opens directory
	dirp = opendir(basePath);
	if(dirp == NULL) return;
	//Passes checks and can be added to map

	//POSIX_PATH_MAX contains null count in it
	char fullPath [_POSIX_PATH_MAX] = "";
	switch(catMode){
		case NO_SLASH:
				strcpy(fullPath, basePath);
		break;

		case ADD_SLASH:
				strcpy(fullPath, basePath);
				strcat(fullPath, "/");
		break;

		default:
				PRINT_ERROR("Program encountered courruption during execution");
				exit(EXIT_FAILURE);
		break;
	}

	++map->length;
	//increase size of header to point to another string
	if((map->map = realloc(map->map, sizeof(map->map) * map->length)) == NULL){
		PRINT_ERROR(FAILED_MALLOC_MSG);
		exit(EXIT_FAILURE);
	}

	//allocate space for string
	map->map[map->length - 1] = malloc(pathLength + 1);

	if(map->map[map->length - 1] == NULL){
		PRINT_ERROR(FAILED_MALLOC_MSG);
		exit(EXIT_FAILURE);
	}
	strcpy(map->map[map->length - 1], fullPath);
	map->map[map->length - 1][pathLength - 1] = '\0';

	while((direntp = readdir(dirp)) != NULL){
		/* Ignore special directories. */
		if((strcmp(direntp->d_name, ".") == 0) ||
			(strcmp(direntp->d_name, "..") == 0))
			continue;

		/* Calculate full name, check we are in file length limts */
		//+1 for nul byte
		if((pathLength + strlen(direntp->d_name) + 1) > _POSIX_PATH_MAX){
			fprintf(stderr, "File path exceeds %d: %s%s\n", _POSIX_PATH_MAX, basePath, direntp->d_name);
			continue;
		}else{
			strcat(fullPath, direntp->d_name);
		}

		struct stat fstat = {0};
		if(stat(fullPath, &fstat) < 0){
			continue;
			memset(fullPath + pathLength + 1, '\0', _POSIX_PATH_MAX - pathLength - 1);
		}

		if(S_ISDIR(fstat.st_mode))
			getSubdirectories(map, fullPath);

	}
	closedir(dirp);
}

Map_t* obtainPathMap(const char* initialDirPath){
	if(initialDirPath == NULL) return NULL;

	Map_t* retMap = malloc(sizeof(*retMap));
	if(retMap == NULL){
		PRINT_ERROR(FAILED_MALLOC_MSG);
		exit(EXIT_FAILURE);
	}
	retMap->length = 0;
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
	for(; p < pathMap->length; ++p) printf("%d> %s", p + 1, pathMap->map[p]);
	free(pathMap);
}
