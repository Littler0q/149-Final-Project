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
/*
todo - to track for files:
- open file state (-1 or file sector # of open file)
- mode (I, U, or O)
- current byte/file position (for read and write - adjusted by SEEK)
*/

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

void printFreeSectorList(){
    struct nodeType *current = head;
    printf("Free sectors: ");
    while(current != NULL){
        printf("%d", current->info);
        if(current->link != NULL){
            printf(" -> ");
        }
        current = current->link;
    }
    printf("\n");
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


/*
todos:
- refactor initializing directory to helper function
- (done) test/debug print statements
- separate commands into module/file
*/

// Commands

// Creates an empty regular file or subdirectory in the current directory
void create() {
    char type;
    char name[10];

    if(scanf(" %c %9s", &type, name) != 2){
        printf("ERROR: command must have format 'CREATE U|D name'\n");
    }
    else {
        type = (char)toupper((unsigned char)type);
        if(type != USERDATA && type != SUBDIRECTORY){
            printf("ERROR: Unknown CREATE type: %c\n", type);
            return;
        }

        int directorySector = currentDirectorySector; 
        // search from starting sector of current directory
        while(sector[directorySector].directory.back != -1){
            directorySector = sector[directorySector].directory.back;
        }

        int entryIndex = -1;                // index of (free) entry to add to
        int entryDirectorySector = -1;      // directory sector number holding (free) entry

        // search all sectors in directory chain: check if space (empty entry) exists and duplicate exists
        while (true) {
            for(int i = 0; i < NUMENTRIES; ++i){
                struct dirEntries dirEntry = sector[directorySector].directory.dir[i];
                if(dirEntry.type == FREE && entryIndex == -1){
                    entryIndex = i;
                    entryDirectorySector = directorySector;
                }
                if(dirEntry.type != FREE && strcmp(dirEntry.name, name) == 0){
                    printf("ERROR: Duplicate name: %s\n", name);
                    return;
                }
            }
            if(sector[directorySector].directory.frwd == -1){
                break;
            }
            directorySector = sector[directorySector].directory.frwd;
        }

        // empty entry not found: extend with another directory sector if SPACE IS AVAILABLE
        if(entryIndex == -1){
            int newDirectorySector = removeNode();
            if(newDirectorySector == -1){
                printf("ERROR: No free sector available\n");
                return;
            }

            sector[directorySector].directory.frwd = newDirectorySector;
            sector[newDirectorySector].directory.back = directorySector;
            sector[newDirectorySector].directory.frwd = -1;
            sector[newDirectorySector].directory.free = NUMENTRIES;
            initializeDirectory(sector[newDirectorySector].directory.dir);

            entryIndex = 0;
            entryDirectorySector = newDirectorySector;
        }

        // create sector for new file/directory and initialize
        int newSector = removeNode();
        if(newSector == -1){
            printf("ERROR: No free sector available\n");
            return;
        }
        if(type == USERDATA){
            printf("CREATE a regular file named %s\n", name);
            sector[newSector].data.backf = -1;
            sector[newSector].data.frwdf = -1;
            memset(sector[newSector].data.userdata, 0, sizeof(sector[newSector].data.userdata));
        }
        else if(type == SUBDIRECTORY){
            printf("CREATE a subdirectory named %s\n", name);
            sector[newSector].directory.back = -1;
            sector[newSector].directory.frwd = -1;
            sector[newSector].directory.free = NUMENTRIES;
            initializeDirectory(sector[newSector].directory.dir);
        }

        // add to current directory
        struct dirEntries newDirEntry = sector[entryDirectorySector].directory.dir[entryIndex];
        newDirEntry.type = type;
        strncpy(newDirEntry.name, name, 9);
        newDirEntry.name[8] = '\0';
        newDirEntry.link = newSector;
        newDirEntry.size = 0;
        sector[entryDirectorySector].directory.dir[entryIndex] = newDirEntry;

        sector[entryDirectorySector].directory.free--;
    }

    printFreeSectorList();
}

// Display names and tpye of all **occupied entries in current directory
void dir() {
    int dirSector = currentDirectorySector; // start from starting sector of same directory
    while (sector[dirSector].directory.back != -1) {
        dirSector = sector[dirSector].directory.back;
    }

    bool sectorExists = true;
    while(sectorExists) {
        struct dirEntries *entries = sector[dirSector].directory.dir;
        for (int i = 0; i < NUMENTRIES; i++) {
            char type = entries[i].type;
            if (type == FREE) continue;
            // printf("(%d:%d) ", dirSector, i); // debug: current sector # and entry #
            printf("%s (%c)", entries[i].name, type);
            if (type == USERDATA) {
                printf(" %d bytes", entries[i].size);
            }
            printf("\n");
        }

        if (sector[dirSector].directory.frwd == -1) {
            sectorExists = false;
        }
        else {
            dirSector = sector[dirSector].directory.frwd;
        }
    }
}

// Change current directory to named subdirectory
void cd() {

}

// Change current directory to sector 0
void root() {
    currentDirectorySector = 0;
}


int main(int argc, char *argv[]){
    if(!verifyStructureSizes()){
        return 1;
    }

    initialize();

    currentDirectorySector = 0;
    openFileState = -1;
    
    char *input = "default";
    
    //in-progress REPL for commands
    while(strcmp(input, EXIT) != 0){
        printf("$ ");
        scanf("%ms", &input);
        toUpperCase(input);
        printf("calling: %s\n", input);

        printf("current directory: %d\n", currentDirectorySector);
        
        if(strcmp(input, EXIT) == 0)
        {
            break;
        }
        else if(strcmp(input, CREATE) == 0)
        {
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
            dir();
        }
        else if(strcmp(input, CD) == 0)
        {
            // TODO: Define and call function
        }
        else if(strcmp(input, ROOT) == 0)
        {
            root();
        }
        else {
            printf("ERROR: Unrecognized command\n");
            break;
        }
        printf("----------------------------------------\n");
    }

    return 0;
}
