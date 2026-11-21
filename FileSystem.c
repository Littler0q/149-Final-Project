#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>



#define NUMENTRIES 25
#define FREE 'F'
#define USERDATA 'U'
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

//Free sector management
struct nodeType { int info; struct nodeType *link; };

struct nodeType *head;

struct nodeType* createNode(int info){
    struct nodeType *new;
     new = (struct nodeType*) malloc(sizeof(struct nodeType));
     new->info = info;
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
        printf("Current's address: %p\n", current);
    }

}


// Initializes a dir with free directory entries, and allows for the link value to be specified.
void initializeDirectory(struct dirEntries *dir, int linkValue){
    for(int i = 0; i < NUMENTRIES; ++i){
        dir[i].link = linkValue;
        dir[i].type = FREE;
        strcpy(dir[i].name, "");
        dir[i].size = 0;
    }
}

// Inializes the file system's data structure.
void initialize(){
    buildList();
    sector[0].directory.back = 0;
    sector[0].directory.frwd = 0;
    sector[0].directory.free = 25;
    initializeDirectory(sector[0].directory.dir, 0);

}



// Takes a string, turns all characters uppercase.
void toUpperCase(char *original){
    int length = strlen(original);
    for(int i = 0; i < length; ++i){
        original[i] = toupper(original[i]);
    }
}

int main(int argc, char *argv[]){
    initialize();
    buildList();

    char *input = "default";
    
    //in-progress REPL for commands
    while(strcmp(input, EXIT) != 0){
        scanf("%ms", &input);
        toUpperCase(input);
        
        if(strcmp(input, EXIT) == 0)
        {
            break;
        }
        else if(strcmp(input, CREATE) == 0)
        {
            // TODO: Define and call function
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

        
    }

    return 0;
}