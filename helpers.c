#include "./includes/helpers.h"

//printing an Error is more dynamic and more compact
//this also means there is no table to shift
void printError(int code, const char* message){
	(void)fprintf(stderr, PNT_RED"%s\n"PNT_RESET,message);
	exit(code);
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
	if(newString == NULL) printError(EXIT_FAILURE,FAILED_MALLOC_MSG);
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
