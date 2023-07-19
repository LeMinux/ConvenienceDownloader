#include "helpers.h"

//printing an Error is more dynamic and more compact
//this also means there is no table to shift
void printError(int code, const char* message){
	printf(PNT_RED"%s\n"PNT_RESET,message);
	exit(code);
}

//helper method to get input and clears stdin
//also nul terminates the string
void exactUserInput(char* input, int buffer){
	int index = 0;
	int data = 0;
	switch(buffer){
		case 1:
			*input = getchar();
		break;
		default:
			while(index < buffer - 1 && (data = getchar()) != '\n')
				*(input + index++) = data;

			*(input + index) = '\0';
		break;
	}

	//clears stdin
	if(data != '\n')
		while(getchar() != '\n'){}

}

//method for obtaining input from file streams
//returns how many characters read for any error checking needed
int exactFileInput(FILE* stream, char* dest, int buffer){
	int index = 0;
	int data = 0;
	switch(buffer){
		case 1:
			*dest = fgetc(stream);
		break;
		default:
			while(index < buffer - 1 && (data = fgetc(stream)) != EOF && data != '\n')
				*(dest + index++) = data;

			*(dest + index) = '\0';
		break;
     }

	//clears to next line
	if(data != '\n' && data != EOF)
		while((data = fgetc(stream)) != '\n' && data != EOF){}

	return index;
}

int unkownFileRead(FILE* stream, char** dest){
	int MAX_READ = 4095;
	int reallocSize = 50;
	int data = '\0';
	int length = 0;

	*dest = realloc(*dest, reallocSize);
	if(*dest == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);

	for(data = fgetc(stream); length < MAX_READ && data != '\n' && data != EOF; data = fgetc(stream)){
		*(*dest + length++) = data;
		if(length == reallocSize){
			reallocSize += 25;
			if((*dest = realloc(*dest, reallocSize)) == NULL)
				printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
		}
	}

	//if the length is zero free the memory to avoid leaks
	switch(length){
		case 0: free(*dest); break;
		default: *(*dest + length) = '\0'; break;
	}

	return length;
}

//checks if file exists
int checkIfExists(const char* check, char mode){
	const char FIND [] = "find ";
	const char FILE_OPTIONS [] = " -maxdepth 1 -type f";
	const char DIR_OPTIONS [] = " -maxdepth 1 -type d";
	char* findCommand = NULL;
	int length = 0;
	switch(mode){
		case 'f':
			length = strlen(FIND) + strlen(check) + strlen(FILE_OPTIONS);
			findCommand = malloc(length + 1);
			if(findCommand == NULL) printError(FAILED_MALLOC_CODE,FAILED_MALLOC_MSG);
			snprintf(findCommand, length + 1, "%s%s%s", FIND, check, FILE_OPTIONS);
		break;
		case 'd':
			length = strlen(FIND) + strlen(check) + strlen(DIR_OPTIONS);
			findCommand = malloc(length + 1);
			if(findCommand == NULL) printError(FAILED_MALLOC_CODE,FAILED_MALLOC_MSG);
			snprintf(findCommand, length + 1, "%s%s%s", FIND, check, DIR_OPTIONS);
		break;
		default: printf(PNT_RED"Passed in an invalid mode for finding"PNT_RESET);break;
	}
	puts(findCommand);
	int result = system(findCommand);
	free(findCommand);
	return result == 0;
}

//surrounds a string in quotes
//simplifies needing to do this\ is\ a\ test
//single quotes could be used, but they can not escape characters
char* surroundInQuotes(const char* surround){
	int length = strlen(surround);
	int i = 0;

	//find if any characters need to be escaped
	for(; i < length; ++i){
		switch(surround[i]){
			case '$': case '\"': case '\\': case '`':
				++length;
			break;
			default: break;
		}
	}

	//+2 for quotes
	length += 2;
	char* newString = malloc(length + 1);
	if(newString == NULL) printError(FAILED_MALLOC_CODE,FAILED_MALLOC_MSG);
	newString[0] = '\"';
	i = 0;
	int p = 1;
	for(; i < strlen(surround); ++i, ++p){
		switch(surround[i]){
			case '$': newString[p] = '\\'; newString[++p] = '$'; break;
			case '\"': newString[p] = '\\'; newString[++p] = '\"'; break;
			case '\\': newString[p] = '\\'; newString[++p] = '\\'; break;
			case '`': newString[p] = '\\'; newString[++p] = '`'; break;
			default: newString[p] = surround[i]; break;
		}
	}
	newString[length - 1] = '\"';
	newString[length] = '\0';
	return newString;
}

void grepIntoFile(const char* text){
	const char GREP_PT1 [] = "ls | grep \"";
	const char GREP_PT2 [] = "\" > GrepTemp.txt";
	
	//greps text so that it can be analyized more
	char* grepCommand = NULL;
	if(text[0] == '-'){
		int length = strlen(GREP_PT1) + strlen(text) + strlen(GREP_PT2) + 1;
		grepCommand = malloc(length + 1);
		if(grepCommand == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
		snprintf(grepCommand, length + 1, "%s\\%s%s", GREP_PT1, text, GREP_PT2);
	}else{
		int length = strlen(GREP_PT1) + strlen(text) + strlen(GREP_PT2);
		grepCommand = malloc(length + 1);
		if(grepCommand == NULL) printError(FAILED_MALLOC_CODE, FAILED_MALLOC_MSG);
		snprintf(grepCommand, length + 1, "%s%s%s", GREP_PT1, text, GREP_PT2);
	}

	printf(PNT_GREEN "%s\n" PNT_RESET, grepCommand);
	if(system(grepCommand) != 0) printError(FAILED_GREP_CODE, FAILED_GREP_MSG);
	free(grepCommand);
}
