#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>



#define NUMENTRIES 25   // # of directory entries
#define FREE 'F'
#define USERDATA 'U'    // file
#define SUBDIRECTORY 'D'
#define EXIT "EXIT"
#define CREATE "CREATE"
#define DELETE "DELETE"
#define OPEN "OPEN"
#define CLOSE "CLOSE"
#define READ "READ"
#define WRITE "WRITE"
#define SEEK "SEEK"
#define DIR "DIR"
#define CD "CD"
#define ROOT "ROOT"
#define DIRECTORY_ENTRY_SIZE 20
#define BLOCK_SIZE 512



union block 
{ 
    struct 
    { 
        int back; 
        int frwd; 
        int free; 
        struct dirEntries
        { 
            char type; 
            char name[9]; 
            int link; 
            short size; 
        } dir[25]; 
    } directory; 
    struct 
    {
        int backf; 
        int frwdf; 
        char userdata[504];     
    } data; 
};

//Core data structure.
union block sector[100];
int currentDirectorySector;
int openFileState;

//Free sector management
struct nodeType { int info; struct nodeType *link; };

struct nodeType *head;

struct nodeType* createNode(int sectorNumber){
    struct nodeType *new;
     new = (struct nodeType*) malloc(sizeof(struct nodeType));
     new->info = sectorNumber;
     new->link = NULL;
     return new;
}


void buildList(){
    struct nodeType *previous;
    struct nodeType *current;
    for(int i = 1; i < 100; ++i){
        
        current = createNode(i);
        if(i == 1){
            head = current;
            previous = current;
            continue;
        }
        previous->link = current;
        previous = current;
    }

}

void insertNode(int sectorNumber){
    struct nodeType *tempHead = head;
    head = createNode(sectorNumber);
    head->link = tempHead;
}

int removeNode(){
    if(head == NULL){
        return -1;
    }
    struct nodeType *temp = head;
    head = head->link;
    int filledSector = temp->info;
    free(temp);
    return filledSector;
}



// Initializes a dir with free directory entries
void initializeDirectory(struct dirEntries *dir){
    for(int i = 0; i < NUMENTRIES; ++i){
        dir[i].link = -1;
        dir[i].type = FREE;
        strcpy(dir[i].name, "");
        dir[i].size = 0;
    }
}

// Inializes the file system's data structure.
void initialize(){
    buildList();
    sector[0].directory.back = -1;
    sector[0].directory.frwd = -1;
    sector[0].directory.free = 25;
    initializeDirectory(sector[0].directory.dir);
}


bool verifyStructureSizes(){
    if(sizeof(struct dirEntries) != DIRECTORY_ENTRY_SIZE){
        printf("ERROR: Directory entry size is %zu bytes; expected %d\n", sizeof(struct dirEntries), DIRECTORY_ENTRY_SIZE);
        return false;
    }

    if(sizeof(union block) != BLOCK_SIZE){
        printf("ERROR: Disk block size is %zu bytes; expected %d\n", sizeof(union block), BLOCK_SIZE);
        return false;
    }

    return true;
}

// Takes a string, turns all characters uppercase.
void toUpperCase(char *original){
    int length = strlen(original);
    for(int i = 0; i < length; ++i){
        original[i] = toupper(original[i]);
    }
}

int main(int argc, char *argv[]){
    if(!verifyStructureSizes()){
        return 1;
    }

    initialize();
    buildList();

    currentDirectorySector = 0;
    openFileState = -1;
    
    char *input = "default";
    
    //in-progress REPL for commands
    while(strcmp(input, EXIT) != 0){
        printf("$ ");
        scanf("%ms", &input);
        toUpperCase(input);
        printf("calling: %s\n", input);
        
        if(strcmp(input, EXIT) == 0)
        {
            break;
        }
        else if(strcmp(input, CREATE) == 0)
        {
            // TODO: Define and call function
            create();
        }
        else if(strcmp(input, DELETE) == 0)
        {
            // TODO: Define and call function
        }
        else if(strcmp(input, OPEN) == 0)
        {
            // TODO: Define and call function
        }
        else if(strcmp(input, CLOSE) == 0)
        {
            // TODO: Define and call function
        }
        else if(strcmp(input, READ) == 0)
        {
            // TODO: Define and call function
        }
        else if(strcmp(input, WRITE) == 0)
        {
            // TODO: Define and call function
        }
        else if(strcmp(input, SEEK) == 0)
        {
            // TODO: Define and call function
        }
        else if(strcmp(input, DIR) == 0)
        {
            // TODO: Define and call function
        }
        else if(strcmp(input, CD) == 0)
        {
            // TODO: Define and call function
        }
        else if(strcmp(input, ROOT) == 0)
        {
            // TODO: Define and call function
        }
        else {
            printf("ERROR: Unrecognized command\n");
            break;
        }
        
    }

    return 0;
}
