#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <stdio.h>

#include "directoryTraverse.h"


/*this function recieves a line and a searchString then
  searches the line for the searchstring*/
int findInLine(char* pathname, char* linkedPath, char* line, char* searchString, int searchSymLink)
{
	int ii = 0;
	int originalLineLen = strlen(line);
	int ssLen = strlen(searchString);
	char aline[originalLineLen];
	strcpy(aline, line);

	while (ii < originalLineLen){
		int alineLen = strlen(aline);
		
	
		int i;  // will be position in the line
		int j = 0; // j will be the position in the search string
		
		for (i = ii; i < alineLen; i++){
			/*we need to check if the first character has a control after it.*/
			if (j == 0 && (searchString[j+1] == '?' || searchString[j+1] == '*' )){
				/*first character has a control after it so the 
	  			  first character may repeat zero(*), one(?) or more times*/
				int valid = 0;
				if (searchString[j+1] == '*'){
					valid = 1;
				}
				while (i < alineLen && (aline[i] == searchString[j])){
					if (aline[i+1] != searchString[j]){
						valid = 1;
						if (j+1 == (ssLen-1)){
							if(searchSymLink && linkedPath[0] != 0){
								printf("\n%s linked from %s\n", linkedPath, pathname);
								printf("%s\n", line);
								return 1;
							}
							else{
								printf("\n%s\n", pathname);
	                    		printf("%s\n", line);
	                    		return 1;
							}
						}
					}
					i++;
				}	
				if (valid == 1 && i == 0 && (j+1) == (ssLen-1)){
					if(searchSymLink && linkedPath[0] != 0){
						printf("\n%s linked from %s\n", linkedPath, pathname);
	                    printf("%s\n", line);
	                    return 1;
					}
					else{
	                    printf("\n%s\n", pathname);
	                    printf("%s\n", line);
	                	return 1;
	                }
				}
				if (valid == 0){
					j = 0;
				}
				else
					j++;
	
			j++;
			}
	
			while ((i < alineLen) && (aline[i] == searchString[j] || searchString[j] == '.')){
			//	if we are at the end of the search string and still in this 
  			//	while loop, then we must have found the string
				if (j == (ssLen-1)){
					if(searchSymLink && linkedPath[0] != 0){
						printf("\n%s linked from %s\n", linkedPath, pathname);
						printf("%s\n", line);
						return 1; //returns 1 if we found the string
					}
					else{
						printf("\n%s\n", pathname);
						printf("%s\n", line);
						return 1; // returns 1 if we found the string
					}
				}
				else if (searchString[j + 2] == '?' || searchString[j + 2] == '*'){
					//if we have a '?' two characters away, then the next character could be 1 or more  repeated
					j++;//lets check if there are 1 or more instances of searchString[j] 
					i++; 
					int valid = 0;
					if (searchString[j+1] == '*'){
						valid = 1;
					}
					while (i < alineLen && (aline[i] == searchString[j])){
						if (aline[i+1] != searchString[j]){
							valid = 1;
							if (j+1 == (ssLen-1)){
								if (searchSymLink && linkedPath[0] != 0){
									printf("\n%s linked from %s\n", linkedPath, pathname);
									printf("%s\n", line);
									return 1;
								}
								else{
									printf("\n%s\n", pathname);
    	                            printf("%s\n", line);
    	                            return 1;
								}
							}
						}
						i++;
					}
					if (valid == 1 && (j+1) >= (ssLen-1)){
    	        	    if(searchSymLink && linkedPath[0] != 0){
    	        	        printf("\n%s linked from %s\n", linkedPath, pathname);
    	        	        printf("%s\n", line);
    	        	        return 1;
    	        	    }
    	        	    else{
    	        	        printf("\n%s\n", pathname);
    	        	        printf("%s\n", line);
    	        	        return 1;
    	        	    }
    	        	}
					else if (valid == 0){
						j = 0; //didnt find the string
					}
					else{
						j++; //skip over the control char
						i--;
					}
				}
				i++;
				j++;
		
			}
			j = 0;
		}
		ii++;
	}
	return 0; /*returns 0 if we did not find the string*/
}


void sSearchFile(char* pathname, char* linkedPath, char* searchString, int searchSymLink)
{
	
	/*open a file for reading*/
	FILE* file = fopen(pathname, "rb");
	if (!file){
		return;
	}
	
	char c;
	//char buff[4000];
	int buffsize = 3000;
	char* buff = (char*)malloc(sizeof(char)*buffsize);
	int i = 0;
	/*get all characters from the file*/
	while( (c = getc(file)) != EOF ){
		/*get a line and store it in buff*/
		if (c != '\n'){
			/*check to make sure buff is large enough
			if (i >= buffsize){
				printf("in path %s | i = %d\n",pathname, i);
				buffsize = buffsize * 2;
				if((buff = realloc(buff, (sizeof(char)*buffsize))) == NULL){
                	write(1, "realloc failed", strlen("realloc failed"));
            		return;
				}
			}*/
			if(i >= buffsize){
				findInLine(pathname, linkedPath, buff, searchString, searchSymLink);
				i = 0;
				memset(buff, 0, buffsize);	
			}
			else {
				buff[i] = c;
				i++;
			}
		}
		else if(buff[0] != 0){ /*analyze the line*/
			/*printf("%s", buff); //uncomment this if we want to print the lines for testing*/
			findInLine(pathname, linkedPath, buff, searchString,searchSymLink);
			i = 0;
			memset(buff,0,buffsize);
		}
	}	
	
	free(buff);
	return;
}

/*function to set up and call the recursive directory traversal algorithm */
void myftw(char* pathname, char* searchString, char fileType, int searchSymLink)
{
	static char* fullpath;
	static size_t pathlen;
	
	if (pathlen <= strlen(pathname)){
		pathlen = strlen(pathname) * 2;
		if ((fullpath = realloc(fullpath, pathlen)) == NULL){
		//	write(1, "realloc failed", strlen("realloc failed"));
			fprintf(stderr, "realloc failed");
			return;
		}
	}

	strcpy(fullpath, pathname);
	directoryTraverse(fullpath, pathlen, searchString, fileType, searchSymLink);

	return;
}

/*function recurses through each directory and calls sSearchFile on appropriate files*/
/*note: fullpath should the path to a directory for functionn to work propperly*/
void directoryTraverse(char* fullpath, size_t pathlen, char* searchString, char fileType, int searchSymLink)
{	
	//printf("path: %s\n", fullpath);
	int errnum;

	struct stat 	statbuf, recurseStatbuf, linkedStatbuf;
	struct dirent 	*entry;
	DIR				*dir;

	/*lstat the fullpath*/
	if(lstat(fullpath, &statbuf) < 0) {
		/*error with stat*/
		errnum = errno;
		fprintf(stderr, "Error statting the file with path %s | %s\n", fullpath, strerror( errnum ));
		return;
	}
	
	/*make sure the path is a directory and open it if directory*/
	if(S_ISDIR(statbuf.st_mode) == 0) {
		/*not a directory, we should not get here if we only 
		  pass directory paths to this function as intended*/
		fprintf(stderr, "Path %s is not a directory\n", fullpath);		
		return;
	}
	else if (!(dir = opendir(fullpath))){ /*open up the directory*/
		/*report an error if we couldnt open the directory*/
		errnum = errno;
		fprintf(stderr, "Could not open directory at path %s | %s\n",fullpath, strerror( errnum ));
        return;
    }

	while ((entry = readdir(dir)) != NULL){
		/*As long as the entry is not the current or previous directory we 
		  will want to analyze it or recurse on it */
		if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
			if(entry->d_name[0] == '.'){ /*does not work on these files*/
				continue;
			}
			/*create a path to be recursed on*/
			char* recursePath = malloc(pathlen);
			memset(recursePath, 0, strlen(recursePath));
			strcpy(recursePath, fullpath);
        	strcat(strcat(recursePath, "/"), entry->d_name);
			
			/*will be used to store the linked path for symbolic links*/
			char* linkedPath = malloc(1024);
			memset(linkedPath, 0, 1024);

			/*now that we have a recurse path made
 			  we must stat it to figure out if its a directory
			  that we can recurse on */
 			if (lstat(recursePath, &recurseStatbuf) < 0){
				/* error with stating the recurse path*/
				errnum = errno;
        		fprintf(stderr, "Error statting the file with path %s | %s\n", fullpath, strerror( errnum ));
				free(linkedPath);
				free(recursePath);
        		continue;
			}
			
			/*do we care about following symlinks?*/
			if (!searchSymLink){/*we dont want to check symLinks if this is true*/
				if (S_ISLNK(recurseStatbuf.st_mode) != 0){ /*the recursePath is a symLink*/
					/*we do not want to read this file so we will instead
  					  continue on to the next entry in the directory in the while loop*/
					free(linkedPath);
					free(recursePath);
					continue;
				}
			}
			else{ /*we are going to want to figure out where the link points to and check there*/
				/*WARNING: If we follow symLinks we must be carefull that we dont recurse into an infinite loop*/
				if (S_ISLNK(recurseStatbuf.st_mode) != 0){/*the recursePath is a symLink*/
					/*get the path that the link points to*/
					ssize_t symPathLen = readlink(recursePath, linkedPath, 1024);
					linkedPath[symPathLen] = '\0';
				
					/*check to see if it links to a directory and if that directory is loop*/
					if (lstat(linkedPath, &linkedStatbuf) < 0){ /*error statting the file*/
						errnum = errno;
						fprintf(stderr, "error stating %s linked to by %s | %s \n", linkedPath, recursePath, strerror( errnum));
						free(recursePath);
						free(linkedPath);
						continue;
					}
					if(S_ISDIR(linkedStatbuf.st_mode) != 0){ /*its a directory*/
						int i = 0;
						int loop = 0;
						while(linkedPath[i] == recursePath[i] && i < symPathLen ){
							if(i == (symPathLen - 1)){
								/*if we are in this path still then the full linked path must be in the recurse path*/
								/*We have a loop*/
								loop = 1;
							}
							i++;	
						}
						if(loop == 1){
							/*we dont want to follow the loop so lets continue to the next entry*/
							free(recursePath);
							free(linkedPath);
							continue;
						}
					}
				}
			}
				/*if we are checking symLinks, search the linkedPath, not the recursePath*/
			if (searchSymLink && linkedPath[0] != 0){
				if (S_ISDIR(linkedStatbuf.st_mode) == 0){/*the recursePath is not a directory*/
            	   /*we only want to work on the file if its a regular file*/ 
            	   if (S_ISREG(linkedStatbuf.st_mode)){ 

            	        /*analyze the recursePath here*/
            	        int linkedPathLen = strlen(linkedPath);
            	        switch(fileType) /*check the file type*/
            	        {
            	            case 0: /*We are searching all regular files*/
            	                sSearchFile(recursePath, linkedPath, searchString, searchSymLink);
            	                break;
            	            case 'c': /*We are searching .c files*/
            	                if (linkedPath[linkedPathLen - 1] == 'c'){
            	                   sSearchFile(recursePath, linkedPath, searchString, searchSymLink);
            	                }
            	                break;
            	            case 'h': /*We are searching .h files*/
            	                if (linkedPath[linkedPathLen - 1] == 'h'){
            	                    sSearchFile(recursePath, linkedPath, searchString, searchSymLink);
            	                }
            	                break;
            	            case 'S': /*we are searching .S files*/
            	                if (linkedPath[linkedPathLen - 1] == 'S'){
            	                    sSearchFile(recursePath, linkedPath, searchString, searchSymLink);
            	                 }
            	            break;
            	        }
            	    }
            	}
			}
			else if (S_ISDIR(recurseStatbuf.st_mode) == 0){/*the recursePath is not a directory*/
     	       /*we only want to work on the file if its a regular file*/ 
               if (S_ISREG(recurseStatbuf.st_mode)){ 

                	/*analyze the recursePath here*/
                    int recursePathLen = strlen(recursePath);
                    switch(fileType) /*check the file type*/
                    {
                    	case 0: /*We are searching all regular files*/
                  			sSearchFile(recursePath, linkedPath, searchString, searchSymLink);
                            break;
                        case 'c': /*We are searching .c files*/
                            if (recursePath[recursePathLen - 1] == 'c'){
                 	           sSearchFile(recursePath, linkedPath, searchString, searchSymLink);
                            }
                            break;
                        case 'h': /*We are searching .h files*/
                            if (recursePath[recursePathLen - 1] == 'h'){
                                sSearchFile(recursePath, linkedPath, searchString, searchSymLink);
                            }
                            break;
                        case 'S': /*we are searching .S files*/
                            if (recursePath[recursePathLen - 1] == 'S'){
                                sSearchFile(recursePath, linkedPath, searchString, searchSymLink);
                             }
                        break;
                    }
                }
            }
            else{ /*the recursePath is a directory so we can recurse on it*/
                /*printf("%s\n", recursePath);*/
                directoryTraverse(recursePath, pathlen, searchString, fileType, searchSymLink);
            }
			/*clean up the dynamic memory to avoid memory leaks*/
        	free(recursePath);
			free(linkedPath);
		}
	}

	return;
}























