#ifndef ERROR_H
#define ERROR_H

void HTMLError(char* Text);

void DebugToHTML(char* DebugString);
void DebugToFile(char* DebugString);
void ErrorToFile(char* DebugString);
void DebugToFile2(char* DebugString1,char* DebugString2);
void EndHTMLDebug();
void StartHTMLDebug();

void RuntimeError1(char* Text);
void RuntimeError2(char* Text1, char* Text2);
void RuntimeError3(char* Text, long Value);
void RuntimeError4(char* Text1, char* Text2);



#endif

