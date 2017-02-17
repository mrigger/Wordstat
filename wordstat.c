#include "wordstat.h"

static struct nlist *hashtab[HASHSIZE];    
static struct nlist *Sensitivehashtab[HASHSIZE]; 

  
/*************************************
 * Hashing function which returns the key
 ***************************************/
unsigned hashkey(char *s){
    unsigned hashval;
    for (hashval = 0; *s != '\0'; s++)
    hashval = *s + 31 * hashval;
    return hashval % HASHSIZE;
}


/*******************************************
 * Parameters: Word
 * Returns a duplicate of the given word
 ******************************************/
char *strdup(char *word){   
    char *duplicate = malloc(strlen (word) + 1);  
    if (duplicate == NULL) return NULL;         
    strcpy (duplicate,word);                     
    return duplicate;                            
}

/*******************************************
 * Parameters: Word
 * Traverses the linked list at the word's hashkey's
 * 		location in the case INSENSITIVE hashtable
 * Returns Pointer to found word, NULL if not found
 ******************************************/
struct nlist *find(char *s){
    struct nlist *np;
    for (np = hashtab[hashkey(s)]; np != NULL; np = np->next)
        if (strcmp(s, np->word) == 0)
            return np; 
    return NULL; 
}

/*******************************************
 * Parameters: Word
 * Traverses the linked list at the word's hashkey's
 * 		location in the case SENSITIVE hashtable
 * Returns Pointer to found word, NULL if not found
 ******************************************/
struct nlist *findSensitive(char *s){
    struct nlist *np;
    for (np = Sensitivehashtab[hashkey(s)]; np != NULL; np = np->next)
        if (strcmp(s, np->word) == 0)
            return np; 
    return NULL; 
}

/*******************************************
 * Parameters: Word
 * Creates a lower case version of the word and
 * 		looks through the hashkey location in the 
 * 		case INSENSITIVE hashtable and updates.
 * Returns pointer to the word in hashtable, NULL if not found
 ******************************************/
struct nlist *update(char *word){ 
	struct nlist *ptr;
	unsigned hashval;
	char *wordString=word;
	int i;
	for(i=0;wordString[i];i++)
	{
		wordString[i]=tolower(wordString[i]);
	}
	if ((ptr = find(wordString)) == NULL) {   
		ptr = (struct nlist *) malloc(sizeof(*ptr));
		if (ptr == NULL || (ptr->word = strdup(wordString)) == NULL)
			return NULL;
		hashval = hashkey(wordString);
		ptr->next = hashtab[hashval];
		hashtab[hashval] = ptr;
		ptr->frequency=1;
		ptr->version=1;
	}else 
		ptr->frequency++;
return ptr;
}

/*******************************************
 * Parameters: Word
 * Creates a lower case version of the word and
 * 		looks through the hashkey location in the 
 * 		case SENSITIVE hashtable and updates.
 * Returns pointer to the word in hashtable, NULL if not found
 ******************************************/
struct nlist *updateCase(char *word){
	struct nlist *node;
    struct nlist *ptr;
    unsigned hashval;
    int i;
    if ((ptr = findSensitive(word)) == NULL) {
        ptr = (struct nlist *) malloc(sizeof(*ptr));
        if (ptr == NULL || (ptr->word = strdup(word)) == NULL)
            return NULL;
        hashval = hashkey(word);
        ptr->next = Sensitivehashtab[hashval];
        Sensitivehashtab[hashval] = ptr;
        for(i=0;word[i];i++)
        {
            word[i]=tolower(word[i]);
        }
        node = find(word);
        if(node!=NULL)
        {
            node->version++;
        }


    }
    return ptr;
    }



/*******************************************
 * Parameters: Array, index1, index2
 * Swaps the values in the given indexes in the 
 * 		array.
 ******************************************/
void swap(struct nlist **master, int k, int swapindex){
	struct nlist *temp = master[k];
	master[k] = master[swapindex];
	master[swapindex] = temp;
}

/*******************************************
 * Parameters: Array, startindex
 * Performs a siftUp algorithm used in Heaps.
 * This is for a max-heap by design
 ******************************************/
void siftUp(struct nlist **master, int start){
	int compareValue;
	int child = start;
	int parent;
	while (child > 0){
		parent = (child - 1) / 2;
		compareValue = strcmp(master[parent]->word, master[child]->word);
		if (compareValue < 0){
			swap(master, parent, child);
			child = parent;
		}
		else return;
	}
}

/*******************************************
 * Parameters: Array, startindex, numElements 
 * Performs a siftDown algorithm used in Heaps.
 * This is for a max-heap by design
 ******************************************/
void siftDown(struct nlist **master, int start, int count){
	int child;
	int swapindex;
	while ((start * 2) + 1 <= count-1){
		child = start * 2 + 1;
		swapindex = start;

		if (strcmp(master[swapindex]->word, master[child]->word) < 0){
			swapindex = child;
		}

		if (child +1 <= count-1 && strcmp(master[swapindex]->word, master[child+1]->word) < 0){
			swapindex = child + 1;
		}
		if (swapindex != start){
			swap(master, start, swapindex);
			start = swapindex;
		}
		else return;
	}
}

/*******************************************
 * Parameters: Array, numElements
 * Turns a regular unsorted array into a max-heap
 ******************************************/
void heapify(struct nlist **master, int count){
	int end = 1;
	while (end < count){
		siftUp(master, end);
		end++;
	}

}

/*******************************************
 * Parameters: Array, numElements
 * Sorts the given array by heapifying then
 * removing form top by swapping and sifting down
 ******************************************/
void heapsort(struct nlist **master, int count){
	int end;
	heapify(master, count);
	end = count -1;
	while (end > 0){
		swap(master, end, 0);
		end--;
		siftDown(master, 0, end);
	}
	if(count != 1) swap(master, 0, 1);
}

void freearray(struct nlist **master, int count){
	int i;
	for(i = 0; i<count-1; i++){
		free(master[i]);
	}
}

void freenode(struct nlist *ptr){
	/*recursively delete each node's pointer, then itself */
	if(ptr->next != NULL){
		freenode(ptr->next);
	}
	free(ptr);

}
/*******************************************
 * Main function
 ******************************************/
int main(int argc, char *argv[]){
	struct nlist **master;
	int i;
	int count;
	char *wordgather;
	FILE *filePointer;
    int wordlength=0;
    int memsize=0;
    int length = 10000;
    char currentchar;
    if (strcmp(argv[1], "-h") == 0){
         	printf("Usage Interface: \n");
 	        printf("wordstat <argument>\n");
	        printf("where <argument> is either the name of the file that wordstat should process, or -h,\n");
        	printf("wordstat should find and output all theunique words in the file in lexicographical order,\n");
        	printf("along with the total number of times each word appears (case-insensitive)\n");
        	printf("and a count of different case-sensitive versions of the word.\n");
        	printf("\n");
        	printf("\n");
        	return;
		
	}
    filePointer=fopen(argv[1], "r"); 
    
    /* wordgather will hold the current word as it's being read */ 
    wordgather = (char *)malloc(sizeof(char) * length);
    if(filePointer!= NULL)
    {
        while(!feof(filePointer) && length--)
        {
        	currentchar = fgetc(filePointer);
            /* check to see if c is a letter */
            if(isalpha(currentchar)){
            	wordgather[wordlength++] = currentchar;
                memsize++;
                continue;
            }
            /* check to see if c is a symbol */
            if(!isdigit(currentchar) && !isalpha(currentchar)){ 
                if(wordgather[0] == '\0') continue;
            
                wordgather[wordlength] = '\0';
                updateCase(wordgather);
                update(wordgather);
                memset(wordgather, '\0', sizeof(wordgather));
                wordlength =0;
                memsize = 0;
                continue;
            }
            /* check to see if c is a number */
            if(isdigit(currentchar)){
				/* check to see if c is a letter */
                if(!isalpha(wordgather[wordlength-1])){
                    if(wordgather[0] == '\0') continue;
                    wordgather[wordlength] = '\0';
                    updateCase(wordgather);
                    update(wordgather);
                    memset(wordgather, '\0', sizeof(wordgather));
                    wordlength = 0;
                    memsize = 0;
                    continue;
                }
                else{ /*isaplha(wordgather[wordlength-1])*/
					wordgather[wordlength++] = currentchar;
					memsize++;
					continue;
				}
            }
            /* check to see if c is not a letter */
            if (!isalpha(currentchar))
                if(wordgather[0] == '\0') continue;
                wordgather[wordlength] = '\0';
                updateCase(wordgather);
                update(wordgather);
                memset(wordgather, '\0', sizeof(wordgather));
                wordlength = 0;
                memsize = 0;
                continue;


        }

        count=1;
		master = (struct nlist **)malloc(sizeof(struct nlist *) * HASHSIZE);
		
		/* Extract all data from case INSENSITIVE hashtable and put in array*/
		for(i=0;i<HASHSIZE;i++){
		  struct nlist *ptr;
		  for(ptr=hashtab[i];ptr!=NULL;ptr=ptr->next){
				  master[count-1] = ptr;
				  count++;
		  }
		  if (ptr != NULL){
			  freenode(ptr);
		  }
		}
		
		printf("Word        Total No. Occurrences    No. Case-Sensitive Versions\n");
		
		/* Perform Heapsort on array */
		heapsort(master, count-1);
		for (i = 0; i < count-1; i++){
			printf("%-15s %15d %20d\n", master[i]->word,master[i]->frequency,master[i]->version);
		}
		freearray(master, count);
    }
    else{
    	printf("File could not be found!");
    }
    wordgather = NULL;
    return 0;
}
