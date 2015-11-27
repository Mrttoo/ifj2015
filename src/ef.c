/*
Project: IFJ15
*/
#include "string.h"
#include "ef.h"
/*
int main() {
	
	

	TData *puvodni=initData();
	puvodni->s = createString(DEFAULT_STRING_CHUNK);
	(*puvodni).s->data="AH\025j";
	(*puvodni).s->length = strlen((*puvodni).s->data) +1 ;	//+1 with \0
	printf("%d",(*puvodni).s->length );
	TData *ret=initData();
	(*ret).s = createString(DEFAULT_STRING_CHUNK);


	ef_substr(puvodni,ret,-2,1);
	printf("heheh:%d",(*ret).s->length);
	ef_length(puvodni,ret);
	printf("puv length: %d",puvodni -> s->length);
	printf("heheh:%d",(*ret).i);
	printf("Hodnota substringu: %s",(*ret).s->data);
	return 0;

}
*/
/*
int length (string s) 
*/

void ef_length(TData *string, TData *ret)
{	if(string->type != DT_STRING)
	{
		fprintf(stderr, "%s: Bad arguments. \n",__func__);
		exit(IFJ_INTERNAL_ERR);
	}
	ret->i=(string->s->length) -1;
	ret->type = DT_INT;
}

/*
string substr(string s, int i, int n)   

*/
void ef_substr(TData *string, TData *ret,int from, int length)		
{	
	if(string -> type != DT_STRING || ret->type != DT_STRING){
		fprintf(stderr, "%s: Bad arguments. \n",__func__);
		exit(IFJ_INTERNAL_ERR);	
	}
	memcpy((*ret).s->data,(*string).s->data,(*string).s->length);
	(*ret).s->length = (*string).s->length;
	ret->type = DT_STRING;
	
	//printf("Hodnota stringu: %s \n", (*ret).s->data);

	int countch = ((*ret).s->length) - 1 ;//without \0
	printf("%d",countch);
	// Can be substring there?
	if(length < 0 ||  from >= countch || countch == 0){
		fprintf(stderr, "%s: Bad arguments. \n",__func__);
		exit(IFJ_INTERNAL_ERR);
	}

	pString mystring = ret->s;

	// If from parameter is negative.
	if(from < 0)	
	{
		from = countch + from ;
	}

	// Call function with right parameters.
	getSubstring(mystring,from,length);
	
}

//// TEST THIS
/*
string concat(string s1, strings2)
*/
void ef_concat(TData *string1, TData *string2, TData *ret)
{	
	if(string1->type != DT_STRING || string2 -> type != DT_STRING){
		fprintf(stderr, "%s: Bad arguments. \n",__func__);
		exit(IFJ_INTERNAL_ERR);
	}
	ret->type = DT_STRING;
	memcpy(ret -> s -> data, string1 -> s -> data, (string1 -> s -> length)-1);
	memcpy((ret -> s -> data)+(string1 -> s -> length),string2 -> s -> data,(string2 -> s -> length)-1 );
	ret->s->length=string1->s->length + string2 -> s -> length -1;
	ret->s->data[(ret->s->length)-1] = '\0';
}
/*
int find(string s, string search) !!!!!!!!!!!!!BOYER-MOOR

*/
void ef_find(TData * stringP, TData * stringT, TData *ret)
{	
	if(stringP->type != DT_STRING || stringT -> type != DT_STRING){
		fprintf(stderr, "%s: Bad arguments. \n",__func__);
		exit(IFJ_INTERNAL_ERR);
	}
	int * CharJumpArray = malloc(sizeof(int)*255);
	int * MatchJumpArray = malloc(sizeof(int) * strlen(stringP->s->data));
	if(CharJumpArray == NULL|| MatchJumpArray == NULL){
		fprintf(stderr, "%s: Malloc error. \n",__func__);
		exit(IFJ_INTERNAL_ERR);
	}
	ComputeJumps(stringP->s->data, CharJumpArray);
	ComputeMatchJumps(stringP->s->data, MatchJumpArray);
	ret -> i = Boyer_Moor_Alg(stringT->s->data, stringP->s->data,CharJumpArray,MatchJumpArray);
	ret -> type =DT_INT;
	free(CharJumpArray);
	free(MatchJumpArray);
}


/////////////////////////////////////////////////////

TData* initData(){
	TData *data = malloc(sizeof(TData));
	if(data == NULL){
		fprintf(stderr, "%s: Malloc error. \n",__func__);
		exit(IFJ_INTERNAL_ERR);
	}
	data->s = NULL;
	data->type = DT_NULL;
}