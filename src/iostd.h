#ifndef __IOSTD__
#define __IOSTD___

struct FYLE;
typedef struct FYLE FYLE;

FYLE* openf(char *filename, int bufferSize, char mode);
void closef(FYLE *pHandle);
char fgetachar(FYLE *pHandle);
void fputachar(FYLE *pHandle, char c);
void fputabatch(FYLE *pHandle, char* str);

#endif
