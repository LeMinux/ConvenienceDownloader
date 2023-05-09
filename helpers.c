#include "helpers.h"

//printing an Error is more dynamic and more compact
//this also means there is no table to shift
void printError(int code, const char* message){
	puts(message);
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

	return ++index;
}
