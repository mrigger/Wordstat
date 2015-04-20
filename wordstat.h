#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

struct nlist{    
    struct nlist *next;    
    char *word;   
    int frequency;   
    int version;
};


#define HASHSIZE 105943

char* strdup(char* word);
unsigned hashkey(char *s);
struct nlist *find(char *s);
struct nlist *findSensitive(char *s);
struct nlist *update(char *word);
struct nlist *updateCase(char *word);
void swap(struct nlist **master, int k, int swapindex);
void siftUp(struct nlist **master, int start);
void siftDown(struct nlist **master, int start, int count);
void heapify(struct nlist **master, int count);
void heapsort(struct nlist **master, int count);


