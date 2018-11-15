#ifndef DIRECTORYTRAVERSE_H
#define DIRECTORYTRAVERSE_H

int findInLine(char* pathname, char* linkedPath, char* line, char* searchString, int searchSymLink);

void directoryTraverse(char* pathname, size_t pathlen, char* searchString, char fileType, int searchSymLink);

void myftw(char* pathname, char* searchString, char fileType, int searchSymLink);

void sSearchFile(char* pathname,char* linkedPath, char* searchString, int searchSymLink);

#endif
