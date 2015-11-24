#ifndef __IAL_H
#define __IAL_H
void ComputeJumps(char* pattern, int * CharJumpArray);
void ComputeMatchJumps(char* pattern, int * MatchJumpArray);
int Boyer_Moor_Alg(char* string,char* pattern, int * CharJumpArray, int * MatchJumpArray);
#endif