/*Po pouziti funkcie sort treba uvolnit pamat vrateneho prvku*/

#include <string.h>
#include <stdlib.h>
#include "util.h"

char *sort (char *string)
{	
	/*vyrobime duplikat mimo READ-ONLY pamate*/
	char *s2 = ifj_strdup(string);
	unsigned int size = strlen(s2);
	/*kazdy prvok posuvame vyssie do haldy ak sa da*/
	for(unsigned int i=0; i<size; ++i)
	{
	/*Pre rodica plati vztah s potomkom rodic=(potomok-1)/2*/
		for(int j=i; j>0; j=(j-1)/2)
		{
			int k = (j-1)/2;
			if(s2[j]>s2[k]) 
			{
				char swap=s2[j];
				s2[j]=s2[k];
				s2[k]=swap;
			}
		}
	}
	//printf("%s\n", s2); //vypise vytvorenu haldu (heap)
	for(unsigned int i=size; i>0; --i)
	{	
		unsigned int parent, child, L, R;
		/* Vymenime najvyssi prvok s poslednym listom starej haldy
 		** znizujeme index posledneho prvku po kazdom jednom triedeni*/
		char swap=s2[0];
		s2[0]=s2[i-1];
		s2[i-1]=swap;
		parent = 0;

		/* Vymiename najvyssi prvok haldy s najvacsim potomkom
		** dokym neexistuje dalsi potomok, ak je potomok iba jeden,
		** skontrolujeme, ci je potomok vacsi, ak ano, vymenime*/
		while(2*parent+3 < i)
		{
			L = parent*2+1;
			R = parent*2+2;
			if (R==i)
				child = L;
			else
				child = s2[L]>s2[R]?L:R;
			if (s2[child] > s2[parent])
			{
				swap = s2[parent];
				s2[parent]=s2[child];
				s2[child]=swap;
			}
			parent = child;
		}
		parent = 0;
	}
	return s2;
}
