#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

//user defined functions
#include "usage.h"
#include "directoryTraverse.h"

int main(int argc, char* argv[])
{
	int errnum;
	/*if no arguments were passed, assume the user wants to know how to use the function*/
	if (argc < 2){
		usage();
		return 0;
	}
    int option;
    opterr = 0;
	char* pathname = NULL;
	char* searchString = NULL;
	char fileType = 0;
	int searchSymLink = 0; // 0 if dont search, 1 if search 

	/*parse the command line arguments*/
	opterr = 0;
	while ((option = getopt (argc, argv, "lf:p:s:")) != -1)
		switch(option)
		{
			case 'l':
				searchSymLink = 1;	
				break;

			case 'f':
				fileType = optarg[0];
				break;

			case 'p':
				pathname = optarg;
				break;
	
			case 's':
				searchString = optarg;

				/*we will check the validity of the string passed*/
				int sLen = strlen(searchString);
				int i;
				int numDots = 0;
				int numStars = 0;
				int numQs = 0;
				for(i = 0; i < sLen; i++){
					if (searchString[i] == '.'){
						numDots++;
					}
					else if (searchString[i] == '*'){
						numStars++;
					}
					else if (searchString[i] == '?'){
						numQs++;
					}

					/*make sure that each opening parenthesis has a closing parenthesis*/
					if (searchString[i] == '('){
						i++;
						if (searchString[i] == ')' ){
							fprintf(stderr, "Must have 2 or more characters between the parenthesis\n");
							return;
						}
						i++;
						if (searchString[i] == ')'){
							fprintf(stderr, "Must have 2 or more characters between the parenthesis\n");
							return;
						}
						i++;
						while(i < sLen){
							if(searchString[i] == ')'){
								/*parenthesis must be followed by a control character*/
								if (i + 1 >= sLen){
									fprintf(stderr, "Parenthesis must be followed by a control character\n");
									return 0;
								}
								if (searchString[i+1] != '.' && searchString[i+1] != '*' && searchString[i+1] != '?'){
									fprintf(stderr, "Parenthesis must be followed by a control character\n");
									return 0;  
								}
								break;
							}
							else if ( i >= (sLen -1)){
								/*if we get here we dont have matching parenthesis*/
								fprintf(stderr, "Search string parenthesis not used correctly\n");
								return 0;
							}
							i++;	
						}
					}

				}
				
				/*check to make sure we only have 1 of each '.' '*' '?' */
				if(numDots > 1)
					fprintf(stderr, "Search String can have a maximum of 1 '.' \n");
				if(numStars > 1)
					fprintf(stderr, "Search String can have a maximum of 1 '*'\n");
				if(numQs >1)
					fprintf(stderr, "Search String can have a maximum of 1 '?'\n");
				if (numQs > 1 || numStars > 1 || numDots > 1)
					return 0;

				break;

			case '?':
				
				return 1;
			default:
				abort();
		}

	/*check if a search string was provided*/
	if (searchString == NULL){
		printf("No strings will be found if a search string is not provided \n");
		usage();
		return;
	}

	/*check to see if a path was provided*/
	if (pathname == NULL){
		printf("Please provide a pathname to start at\n");
		usage();
		return;
	}
	
	/*check if valid file type was provided or if no file type was provided*/
	if(fileType != 0 && fileType != 'c' && fileType != 'h' && fileType != 'S'){
		usage();
		return;
	}
	
	/*start walking the file tree*/
	myftw(pathname, searchString, fileType, searchSymLink);


    return;
}
