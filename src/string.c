#include "error.h"
#include <stdio.h>
#include "string.h"
#include <string.h>
#include <stdlib.h>
/*int main()
{
	pString pokus;
	pokus =NULL;

	if((pokus=createString()) !=NULL)
	deleteString(pokus);

	return 0;
}*/

// If something will n otgo well, null pointer will be returned.


/*
 * Creates space for data.
 */
int createStringData(String *string, int size)
{

	if((string -> data = malloc(sizeof(char) * size)) != NULL){
	string -> data[0] = '\0';
	string -> length = 1;
	string -> size = size;
	}else{
		fprintf(stderr, "%s: Unable to allocate %d bytes for string data\n",__func__, size);
		return(-1);
	}
	return 0;
}

/*
 * Creates String instance with specified size.
 */
pString createString(int size){
    String* string;
	if((string = malloc(sizeof(String))) != NULL){
		if (-1 == createStringData(string, size)){
			free(string);
			string = NULL;
			fprintf(stderr, "%s: Malloc error. \n",__func__);
			exit(IFJ_INTERNAL_ERR);
		}
	}else{
		fprintf(stderr, "%s: Unable to allocate %ld bytes for String \n",__func__, sizeof(String));
		exit(IFJ_INTERNAL_ERR);
	}
	return string;
}



/*
 *  Makes String instance free.
 */
void deleteString(String *string)
{
	if(string != NULL)
		if(string -> data != NULL){
			free(string-> data);
		}
	free(string);
	string = NULL;
}
/*
 * Get substring.
 */
 void getSubstring(pString string,int from,int length)
 {	
 		memcpy(string -> data, (string -> data) + from, length);
 		string -> length = length +1; //+ '\0'
 		string -> data[length] = '\0';
 		//substring -> size 
 }

/*
 * Resize given string with specified  size
 */
 pString rsizeString(pString string, int size)
 {	
 	string -> data = realloc(string -> data, size);

 	if(string -> data == NULL) {	//Cchyba
 		fprintf(stderr, "%s: Malloc error. \n",__func__);
 		exit(IFJ_INTERNAL_ERR);
 	}

 	string -> size = size;
 	return string;
 }
