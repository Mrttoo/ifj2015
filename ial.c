
#include <stdio.h>
#include "ial.h"
#include "string.h"
#include <string.h>
#include <stdlib.h>
/*
int main(){
	char * pattern = "";
	char * string="bbasdjkadahdahojasdhkjadb";
	int * CharJumpArray = malloc(sizeof(int)*255);
	int * MatchJumpArray = malloc(sizeof(int) * strlen(pattern));
	ComputeJumps(pattern, CharJumpArray);
	ComputeMatchJumps(pattern, MatchJumpArray);
	int i = Boyer_Moor_Alg(string, pattern,CharJumpArray,MatchJumpArray);
	printf("%d\n",i);
	free(CharJumpArray);
	free(MatchJumpArray);
	return 0;
}
*/
  void ComputeJumps(char* pattern, int * CharJumpArray)
{
	/*
		Stanovení hodnot pole CharJump, které určují posuv vzorku.
	*/

	// Vytvoření pole, kde jednotlivé symboly řetezce budou mít 
	// uloženou svoji ascii hodnotu	
	int len = strlen(pattern);
	int * ArrayOfASCII = malloc(sizeof(int)* len);
	if(ArrayOfASCII == NULL){
		fprintf(stderr, "%s: Malloc error. \n",__func__);
		exit(IFJ_INTERNAL_ERR);
	}

	int i;
	for(i = 0; i < len; i++)
		ArrayOfASCII[i] = pattern[i];

	// Vložení maximální délky vzoru jednotlive pro všechny prvky abecedy
	for(i = 0; i < 255; i++)
		CharJumpArray[i] = len;
	// Pro písmena ve vzoru nastavím speciální hodnoty pro skoky
	for(i = 0; i < len; i++)
		CharJumpArray[ArrayOfASCII[i]] = len - i - 1;

	free(ArrayOfASCII);

}

void ComputeMatchJumps(char* pattern, int * MatchJumpArray)
{	

	int len = strlen(pattern);
	int* Backup = malloc(sizeof(int) * len);
	if(Backup == NULL){
		fprintf(stderr, "%s: Malloc error. \n",__func__);
		exit(IFJ_INTERNAL_ERR);
	}
	int k;
	// Vpočítáme největší možné skoky posunu
	for(  k = 0 ; k < len; k++)
		MatchJumpArray[k] = 2*len-(k+1);
	k=len-1; int q= len;
	while(k >= 0){
		Backup[k] = q;
		while(q < len && pattern[k] != pattern[q]){
			MatchJumpArray[q] = MatchJumpArray[q] < len-(k+1) ? MatchJumpArray[q]: len-(k+1) ;
			q= Backup[q];
		}
		k = k-1;
		q=q-1;
	}

	for( k=0; k<q;k++)
		MatchJumpArray[k] = MatchJumpArray[k] < len+q-k+1?MatchJumpArray[k]:len+q-k+1;

	int qq=Backup[q];
	while (q <len)
	{
		while (q <= qq){
			MatchJumpArray[q] = MatchJumpArray[q] < qq-q+len-1 ? MatchJumpArray[q]:qq-q+len-1;
			q=q+1;
		}
		qq=Backup[qq];
	}
}

int Boyer_Moor_Alg(char* string,char* pattern, int * CharJumpArray, int * MatchJumpArray)
{
	/*
		BMA porovnává vzorek zprava doleva a pro posuv bere ten z výsledků dvou heuristik,
		který je výhodnější.
	*/
	

	int j = strlen(pattern)-1;
	int k = strlen(pattern)-1;

	while(j < strlen(string) && k >= 0){
		if( string[j] ==  pattern[k]){
			j = j-1;
			k=k-1;
		}else{
			j=j+ (CharJumpArray[string[j]] > MatchJumpArray[k]?CharJumpArray[string[j]]:MatchJumpArray[k]);
			k = strlen(pattern)-1;
		}
	}
	if (k < 0)
		return j +1;	// Uspesne hledani
	else
		return -1;		// Neuspesne hledani

}
