#ifndef INI_FILE
#define INI_FILE

#ifdef __cplusplus 
extern "C" {
#endif

extern FILE* IniFile;
extern char IniFileLine[501];

int OpenIniFile(char* FileName);
int CloseIniFile();
char* GetFirstParam(char* Param);
char* GetNextParam(char* Param);

#ifdef __cplusplus 
}
#endif

#endif

