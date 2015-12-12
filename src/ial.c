#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ial.h"
#include "error.h"
#include "string.h"
#include "util.h"
#include "stable.h"

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
        return j +1;    // Uspesne hledani
    else
        return -1;        // Neuspesne hledani

}
/**
  *  Draft of Binary Search Tree
  *  which will be used for implementation
  *  of symbol table
*/
bst_node_t *bst_new_node(char *key, stable_data_t *data)
{
    bst_node_t *n = malloc(sizeof *n);

    if(n == NULL) {
        fprintf(stderr, "Unable to allocate memory for BST node\n");
        exit(IFJ_INTERNAL_ERR);
    }

    n->left = NULL;
    n->right = NULL;
    n->key = ifj_strdup(key);
    if(data != NULL) {
        //memcpy(&(n->data), data, sizeof *data);
        n->data = *data;
        n->data.id = n->key;
        // If a symbol is a function, copy its parameters as well
        if(data->type == STABLE_FUNCTION && data->func.nparam > 0) {
            n->data.func.params = malloc(sizeof *(n->data.func.params) * n->data.func.nparam);
            if(n->data.func.params == NULL) {
                fprintf(stderr, "Unable to allocate memory for function params");
                exit(IFJ_INTERNAL_ERR);
            } else {
                memcpy(n->data.func.params, data->func.params, sizeof *(data->func.params) * data->func.nparam);
            }
        }
    } else {
        n->data.type = STABLE_UNDEFINED;
        n->data.id = n->key;
    }

    return n;
}

void bst_destroy(bst_node_t *node)
{
    if(node == NULL) 
        return;
    if(node->left != NULL)
        bst_destroy(node->left);
    if(node->right != NULL)
        bst_destroy(node->right);

    if(node->data.type != STABLE_UNDEFINED)
        stable_destroy_data(&(node->data));

    free(node->key);
    free(node);
    node = NULL;
}

bst_node_t *bst_insert_node(bst_node_t *node, char *key, stable_data_t *data)
{
    if(node == NULL) {
        return bst_new_node(key, data);
    } else {
        // Don't allow duplicates
        int sres = strcmp(key, node->key);
        if(sres < 0)
            node->left = bst_insert_node(node->left, key, data);
        else if(sres > 0)
            node->right = bst_insert_node(node->right, key, data);
    }

    return node;
}

bst_node_t *bst_lookup_node(bst_node_t *node, char *key)
{
    if(node == NULL) {
        return NULL;
    } else {
        int sres = strcmp(key, node->key);
        if(sres == 0) {
            return node;
        } else {
            if(sres < 0)
                return bst_lookup_node(node->left, key);
            else
                return bst_lookup_node(node->right, key);
        }
    }
}

void bst_foreach_func(bst_node_t *node, void (*func)(bst_node_t *node))
{
    if(node == NULL || func == NULL)
        return;

    printf("Node %s | Type: %s\n", node->data.id, node->data.type == STABLE_FUNCTION ? "FUNCTION" : (node->data.type == STABLE_VARIABLE ? "VARIABLE" : "ERROR"));
    printf("Defined: %s\n", node->data.func.defined ? "true" : "false");

    bst_foreach_func(node->left, func);
    func(node);
    bst_foreach_func(node->right, func);
}

// Tests
#ifdef IFJ_BST_DEBUG

void debug_bst_print_tree(bst_node_t *node)
{
    if(node == NULL)
        return;

    debug_bst_print_tree(node->left);
    printf("%s\n", node->key);
    debug_bst_print_tree(node->right);
}

int debug_bst_size(bst_node_t *node)
{
    if(node == NULL)
        return 0;

    return 1 + debug_bst_size(node->left) + debug_bst_size(node->right);
}

#include <math.h>
#include <time.h>

int main(int argc, char *argv[])
{
    char *ta[] = { "key", "Key", "string", "sTrIng", "STRING", "id",
                   "num", "i", "I", "key", "asdf3", "tea23", "_2314", NULL };
    int rc = 0;
    bst_node_t *root = NULL;
    stable_data_t data = { .type = LEX_EOF, .var.val.i = 1 };
    // Test alloc
    root = bst_new_node("j", &data);

    // Test insert
    for(unsigned int i = 0; ta[i] != NULL; i++)
        root = bst_insert_node(root, ta[i], &data);

    // Print tree
    debug_bst_print_tree(root);
    printf("Size: %d\n", debug_bst_size(root));

    // Test lookup
    for(unsigned int i = 0; ta[i] != NULL; i++) {
        bst_node_t *search = bst_lookup_node(root, ta[i]);
        if(search == NULL) {
            fprintf(stderr, "[FAIL] Couldn't find node with value %s\n", ta[i]);
            rc = 1;
        } else {
            if(strcmp(ta[i], search->key) == 0) {
                fprintf(stderr, "[PASS] Found node with correct value (%s == %s) (%d)\n",
                        ta[i], search->key, search->data.var.val.i);
            } else {
                fprintf(stderr, "[FAIL] Found node with incorrect value (%s != %s)\n",
                        ta[i], search->key);
                rc = 1;
            }
        }
    }

    // Test free (check with valgrind)
    bst_destroy(root);

    return rc;
}
#endif
