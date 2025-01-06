#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "queue.c"

#define DISK_FILENAME "disk.bin"


typedef struct {
    int disk_size;
    int block_size;
    int directory_offset;
    int fat_offset;
    int data_offset;
    int number_of_blocks;
    int directory_size;
    int data_blocks;
    int max_filesize;
} __attribute__((packed)) superblock;











typedef struct {
    int isFree;
    int next;
} __attribute__((packed)) fat_entry;

typedef struct {
    char name[64];
    int size;
    int starting_block;
    int parent;
    int isDelete;
    int isFree;
} __attribute__((packed)) dir_entry;

superblock initializing_metadata() {
    superblock meta;
    meta.disk_size = 67108864;
    meta.block_size = 1024;
    meta.directory_size = 128;
    meta.number_of_blocks = (meta.disk_size / meta.block_size) - (meta.block_size * 3);
    meta.directory_offset = 1;
    meta.fat_offset = meta.directory_offset + (((sizeof(dir_entry)*meta.directory_size) + meta.block_size - 1) / meta.block_size);
    int r_block = meta.number_of_blocks - meta.fat_offset ;
    int fat_blocks = (r_block + (meta.block_size / sizeof(fat_entry)) - 1) / (meta.block_size / sizeof(fat_entry));
    meta.data_offset = meta.fat_offset + fat_blocks;
    meta.data_blocks = meta.number_of_blocks - meta.data_offset;
    meta.max_filesize = 128 * meta.block_size;

    FILE *file = fopen(DISK_FILENAME, "rb+"); 
    if (file == NULL) {
        perror("Error opening disk file for writing");
        return;
    }

    if (fwrite(&meta, sizeof(superblock), 1, file)!= 1) {
        perror("Error writing metadata to disk file");
    }

    fclose(file);

    printf("Metadata successfully initialized.\n");
    return meta;
}

superblock block;


typedef struct {
    dir_entry entries[128];
} __attribute__((packed)) DIR;


typedef struct {
    fat_entry entries[61964];
} __attribute__((packed)) FAT;

DIR *dir;
FAT *fat;

queue *free_Dir;
queue *free_fat;

int parent = 0;

void loadMetadata() {
    FILE *file = fopen(DISK_FILENAME, "rb+"); 
    if (file == NULL) {
        perror("Error opening disk file for reading");
        return;
    }
    if (fread(&block, sizeof(superblock), 1, file)!= 1) {
        perror("Error reading metadata from disk file");
        fclose(file);
        return;
    }

}

void load_free_dir()
{
    for(int i = 0; i < block.directory_size; i++) {
        if (dir->entries[i].isFree) {
            enqueue(free_Dir, i);
        }
    }
}

void load_free_fat()
{
    for(int i = 0; i < block.data_blocks; i++) {
        if (fat->entries[i].isFree) {
            enqueue(free_fat, i);
        }
    }
}

int loadDirectory() {
    FILE *file = fopen(DISK_FILENAME, "rb+"); 
    if (file == NULL) {
        perror("Error opening disk file for reading");
        return -1;
    }

    fseek(file, block.directory_offset*block.block_size, SEEK_SET);
    fread(dir, sizeof(DIR), 1, file);

    fclose(file);
    return 0; 
}

int saveDirectory() {
    FILE *file = fopen(DISK_FILENAME, "rb+"); 
    if (file == NULL) {
        file = fopen("file.fs", "wb+"); 
        if (file == NULL) {
            perror("Error creating disk file");
            return -1;
        }
    }
    fseek(file, block.directory_offset*block.block_size, SEEK_SET);

    fwrite(dir, sizeof(DIR), 1, file);

    fclose(file);
    return 0;
}

int loadFAT() {
    FILE *file = fopen(DISK_FILENAME, "rb+"); 
    if (file == NULL) {
        perror("Error opening disk file for reading");
        return -1;
    }
    fseek(file, block.fat_offset*block.block_size, SEEK_SET);

    fread(fat, sizeof(FAT), 1, file);

    fclose(file);
    return 0; 
}

int saveFAT() {

    FILE *file = fopen(DISK_FILENAME, "rb+");
    if (file == NULL) {
        perror("Error creating disk file");
        return -1;
    }

    int sz = block.fat_offset*block.block_size;
    int sz2 = sizeof(FAT);

    fseek(file, block.fat_offset*block.block_size, SEEK_SET);

    fwrite(fat, sizeof(FAT), 1, file);
    fclose(file);

    return 0;
}

void save_directory_to_file() {
    FILE *file = fopen("directory.txt", "w");
    if (file == NULL) {
        perror("Error opening directory.txt for writing");
        return;
    }

    fprintf(file, "\nDirectory:\n");
    for (int i = 0; i < block.directory_size; i++) {
        fprintf(file, "Name: %s, Size: %u, Start Block: %d, Parent: %s, IsDeleted: %s\n",
                dir->entries[i].name,
                dir->entries[i].size,
                dir->entries[i].starting_block,
                dir->entries[i].parent == -2 ? "Root" : dir->entries[dir->entries[i].parent].name,
                dir->entries[i].isDelete ? "Yes" : "No");
    }

    fclose(file);
    printf("Directory saved to directory.txt\n");
}

void save_fat_to_file() {
    FILE *file = fopen("FAT.txt", "w");
    if (file == NULL) {
        perror("Error opening fat.txt for writing");
        return;
    }

    fprintf(file, "\nFAT:\n");
    for (int i = 0; i < block.data_blocks; i++) {
        fprintf(file, "BLOCK %d: %s, Next: %d\n", 
                i, 
                fat->entries[i].isFree ? "Yes" : "No", 
                fat->entries[i].next);
    }

    fclose(file);
    printf("index saved to index.txt\n");
}

void writediskBlock(int blockIndex, const char *data) {

    FILE *file = fopen(DISK_FILENAME, "wb+");
    if (file == NULL) {
        perror("Error opening disk file for writing");
        exit(EXIT_FAILURE);
    }

    fseek(file, blockIndex*block.block_size, SEEK_SET);

    if (fwrite(data, block.block_size, 1, file) != 1) {
        perror("Error writing block to disk");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // fputc('\n', file);

    fclose(file);
}

void writeBlock(int blockIndex, const char *data) {

    FILE *file = fopen(DISK_FILENAME, "rb+");
    if (file == NULL) {
        perror("Error opening disk file for writing");
        exit(EXIT_FAILURE);
    }

    int sz = (block.data_offset*block.block_size)+(blockIndex*block.block_size);
    fseek(file, (block.data_offset*block.block_size)+(blockIndex*block.block_size), SEEK_SET);

    if (fwrite(data, block.block_size, 1, file) != 1) {
        perror("Error writing block to disk");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // fputc('\n', file);

    fclose(file);
}

void readBlock(int blockIndex, char *buffer) {

    FILE *file = fopen(DISK_FILENAME, "rb+");
    if (file == NULL) {
        perror("Error opening disk file for reading");
        exit(EXIT_FAILURE);
    }

    int sz = (block.data_offset*block.block_size)+(blockIndex*block.block_size);


    fseek(file, (block.data_offset*block.block_size)+(blockIndex*block.block_size), SEEK_SET);

    if (fread(buffer, block.block_size, 1, file) != 1) {
        perror("Error reading block from disk");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

void save_10_block(int n)
{
    FILE *file = fopen("block.txt", "w");
    if (file == NULL) {
        perror("Error opening fat.txt for writing");
        return;
    }

    char buffer[1024];
    fprintf(file, "\nBlocks:\n");
    for(int i = n; i < n+10; i++) {
        readBlock(i, buffer);
        fprintf(file, "%s\n", buffer);
    }
    fclose(file);
}

void create() {
    char buffer[1024];
    memset(buffer, '-', sizeof(buffer));
    for(int i = 0; i < block.number_of_blocks; i++)
    {
        writediskBlock(i, buffer);
    }
}

void format() {

    FILE *file = fopen(DISK_FILENAME, "rb+");
    if (file == NULL) {
        perror("Error opening the disk file for formatting");
        exit(EXIT_FAILURE);
    }

    block = initializing_metadata();
    if (fwrite(&block, sizeof(superblock), 1, file) != 1) {
        perror("Error writing superblock metadata during formatting");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    
    for(int i = 0; i < block.directory_size; i++) {
        dir->entries[i].isFree = true;
        dir->entries[i].parent = -1; 
        dir->entries[i].isDelete = false;
    }

    saveDirectory(dir);

    for(int i = 0; i < block.data_blocks; i++) {
        fat->entries[i].isFree = true;
        fat->entries[i].next = -1;
    }

    saveFAT(fat);

    char garbage[1024];
    memset(garbage, '-', sizeof(garbage));

    for (int blockIndex = 0; blockIndex < block.data_blocks; blockIndex++) {
        writeBlock(blockIndex, garbage);
    }

    clearQueue(free_Dir);
    clearQueue(free_fat);
    load_free_dir(*dir, free_Dir);
    load_free_fat(*fat, free_fat);

    printf("Disk formatted\n");
}

void mkdir(char *command) {

    int index = dequeue(free_Dir);
    if(index == -1) {
        printf("No Space\n");
        return;
    }

    char directory[64] = {};
    strncpy(directory, command+6, strlen(command)-6);
    directory[strcspn(directory, "\n")] = '\0';
    printf("%s\n", directory);
    bzero(dir->entries[index].name, 64);
    strncpy(dir->entries[index].name, directory, sizeof(dir->entries[index].name));
    dir->entries[index].name[sizeof(dir->entries[index].name) - 1] = '\0';
    dir->entries[index].size = 0;
    dir->entries[index].parent = parent;
    dir->entries[index].starting_block = -1;
    dir->entries[index].isDelete = false;
    dir->entries[index].isFree = false;
    // print_directory(*dir);
    
}

void touch(char *command) {

    char filename[64] = {};
    strncpy(filename, command+6, strlen(command)-6);
    filename[strcspn(filename, "\n")] = '\0';
    printf("%s\n", filename);

    for (int i = 0; i < block.directory_size; i++) {
        if (!dir->entries[i].isFree && 
            !dir->entries[i].isDelete && 
            dir->entries[i].parent == parent && 
            strcmp(dir->entries[i].name, filename) == 0) {
            printf("Error: A file or directory with the name '%s' already exists in this directory.\n", filename);
            return;
        }
    }
    int index = dequeue(free_Dir);
    if(index == -1) {
        printf("No Space\n");
        return;
    }
    int block = dequeue(free_fat);
    fat->entries[block].isFree = false;
    if(block == -1) {
        printf("No Space\n");
        return;
    }

    bzero(dir->entries[index].name, 64);

    strncpy(dir->entries[index].name, filename, sizeof(dir->entries[index].name));
    dir->entries[index].name[sizeof(dir->entries[index].name) - 1] = '\0';
    dir->entries[index].size = 0;
    dir->entries[index].parent = parent;
    dir->entries[index].starting_block = block;
    dir->entries[index].isDelete = false;
    dir->entries[index].isFree = false;
}

void nano(char *command) {

    char filename[64] = {};
    strncpy(filename, command+5, strlen(command)-5);
    filename[strcspn(filename, "\n")] = '\0';
    printf("%s\n", filename);

    int file_index = -1;
    for (int i = 0; i < block.directory_size; i++) {
        if (!dir->entries[i].isFree && 
            !dir->entries[i].isDelete && 
            dir->entries[i].parent == parent && 
            strcmp(dir->entries[i].name, filename) == 0) {
            file_index = i;
            break;
        }
    }

    if (file_index == -1) {
        printf("Error: File '%s' does not exist.\n", filename);
        return;
    }

    char content[131072] = {};
    printf("Write the content of the file (Press Enter to exit):\n");
    fflush(stdout);
    fgets(content, block.max_filesize, stdin);
    content[strcspn(content, "\n")] = '\0';

    int content_size = strlen(content);
    int block_req = (content_size/block.block_size) + 1;

    if (block_req > free_fat->size) {
        printf("Error: Not enough free blocks to save the content.\n");
        return;
    }

    int current_block = dir->entries[file_index].starting_block;

    for (int i = 0; i < block_req; i++) {

        char block_data[1024] = {};
        size_t copy_size = strlen(content + (i * block.block_size)); 

        if (copy_size > block.block_size) {
            copy_size = block.block_size; 
        }
        memset(block_data, '\0', block.block_size); 
        strncpy(block_data, content + (i * block.block_size), copy_size);

        writeBlock(current_block, block_data);

        if (i < block_req - 1) {
            int new_block = dequeue(free_fat);
            if (new_block == -1) {
                printf("Error: Could not allocate a new block.\n");
                break;
            }
            fat->entries[current_block].next = new_block;
            fat->entries[new_block].isFree = false;
            current_block = new_block;
        }
    }

    fat->entries[current_block].next = -1;

    dir->entries[file_index].size += content_size;

    printf("File '%s' updated successfully (%d bytes written).\n", filename, content_size);

}

void vim(char *command) {

    char filename[64] = {};
    strncpy(filename, command + 4, strlen(command) - 4); 
    filename[strcspn(filename, "\n")] = '\0';

    int fileIndex = -1;
    for (int i = 0; i < block.directory_size; i++) {
        if (!dir->entries[i].isFree &&
            strcmp(dir->entries[i].name, filename) == 0 &&
            dir->entries[i].parent == parent) {
            fileIndex = i;
            break;
        }
    }

    if (fileIndex == -1) {
        printf("File '%s' not found in directory.\n", filename);
        return;
    }

    int blockIndex = dir->entries[fileIndex].starting_block;
    if (blockIndex == -1) {
        printf("Error: File '%s' has no valid data blocks.\n", filename);
        return;
    }

    printf("Content of '%s':\n", filename);
    char buffer[1024]; 
    // buffer[BLOCK_SIZE] = '\0';  

    while (blockIndex != -1) {
        readBlock(blockIndex, buffer);
        printf("%s", buffer);
        blockIndex = fat->entries[blockIndex].next; 
    }

    printf("\n");
}

void cat(char *command) {
    char filename[64] = {};
    strncpy(filename, command + 4, strlen(command) - 4);
    filename[strcspn(filename, "\n")] = '\0';

    int fileIndex = -1;
    for (int i = 0; i < block.directory_size; i++) {
        if (!dir->entries[i].isFree &&
            !dir->entries[i].isDelete &&
            dir->entries[i].parent == parent &&
            strcmp(dir->entries[i].name, filename) == 0) {
            fileIndex = i;
            break;
        }
    }

    if (fileIndex == -1) {
        printf("Error: File '%s' does not exist.\n", filename);
        return;
    }

    int blockIndex = dir->entries[fileIndex].starting_block;
    if (blockIndex == -1) {
        printf("Error: File '%s' has no valid data blocks.\n", filename);
        return;
    }

    // Display existing content
    printf("Content of '%s':\n", filename);
    char buffer[1024];
    int nob = -1;
    int lbs = 0;
    // buffer[block.block_size] = '\0';
    int lastBlockIndex = -1;
    while (blockIndex != -1) {
        readBlock(blockIndex, buffer);
        printf("%s", buffer);
        lastBlockIndex = blockIndex;
        blockIndex = fat->entries[blockIndex].next;
        nob++;
        lbs = strlen(buffer);
    }

    // // Calculate remaining space in the last block
    // blockIndex = dir->entries[fileIndex].starting_block;
    // while (blockIndex != -1) {
    //     lastBlockIndex = blockIndex;
    //     blockIndex = fat->entries[blockIndex].next;
    // }

    int existingfilesize = (nob * block.block_size)+lbs;

    int rs = block.max_filesize-existingfilesize;
    char content[rs];
    bzero(content, sizeof(content));
    printf("\nAppend to '%s' (Press Enter to exit):\n", filename);
    fflush(stdout);
    fgets(content, sizeof(content), stdin);
    // content[strcspn(content, "\n")] = '\0';
    int es = block.block_size - lbs;
    int nl = strlen(content);
    int br = 0;
    if(es >= nl)
    {
        es = nl;
        br = 1;
    }
    else 
    {
        rs = nl - es;
    }
    if (es > 0)
    {
        readBlock(lastBlockIndex, buffer);
        strncat(buffer, content, es);
        writeBlock(lastBlockIndex, buffer);
        if (br == 1)
            return;
    }

    int reqblock = rs / block.block_size;

    int nb = dequeue(free_fat);

    fat->entries[lastBlockIndex].next = nb;
    fat->entries[nb].isFree = true;


    // Allocate new blocks if needed
    for (int i = 0; i < reqblock; i++) {
        char block_data[1024] = {};
        size_t copy_size = strlen(content + (i * block.block_size)); 

        if (copy_size > block.block_size) {
            copy_size = block.block_size; 
        }
        memset(block_data, '\0', block.block_size); 
        strncpy(block_data, content + (i * block.block_size), copy_size);

        writeBlock(nb, block_data);

        if (i < reqblock - 1) {
            int new_block = dequeue(free_fat);
            if (new_block == -1) {
                printf("Error: Could not allocate a new block.\n");
                break;
            }
            fat->entries[nb].next = new_block;
            fat->entries[new_block].isFree = false;
            nb = new_block;
        }
    }
    fat->entries[nb].next = -1;

    dir->entries[fileIndex].size += nl;

    printf("File '%s' updated successfully (%d bytes written).\n", filename, nl);
}

void ls() {
    int sz = 0;
    printf("Contents of the current directory:\n");
    for (int i = 0; i < block.directory_size; i++) {
        if (!dir->entries[i].isFree && 
            !dir->entries[i].isDelete && 
            dir->entries[i].parent == parent) {
            
            printf("%s\n", dir->entries[i].name); 
            // sz++;
        }
    }

}

int lsche() {
    int sz = 0;
    printf("Contents of the current directory:\n");
    for (int i = 0; i < block.directory_size; i++) {
        if (!dir->entries[i].isFree && 
            !dir->entries[i].isDelete && 
            dir->entries[i].parent == parent) {
            
            // printf("%s\n", dir->entries[i].name); 
            sz++;
        }
    }
    
}

void rm(char *command) {
    char name[64] = {};
    strncpy(name, command + 3, strlen(command) - 3);
    name[strcspn(name, "\n")] = '\0';

    for (int i = 0; i < block.directory_size; i++) {
        if (!dir->entries[i].isFree &&
            !dir->entries[i].isDelete &&
            dir->entries[i].parent == parent &&
            strcmp(dir->entries[i].name, name) == 0) {

            if (dir->entries[i].starting_block == -1)
            {
                int n = lsche();
                if (n == 0)
                {
                    printf("This folder have already some files in it\n");
                    return;
                }
            }
            dir->entries[i].isDelete = true;
            dir->entries[i].isFree = true;

            if (dir->entries[i].starting_block != -1) {
                int current_block = dir->entries[i].starting_block;
                while (current_block != -1) {
                    int next_block = fat->entries[current_block].next;
                    fat->entries[current_block].isFree = true;
                    fat->entries[current_block].next = -1;
                    enqueue(free_fat, current_block); 
                    current_block = next_block;
                }
            }

            enqueue(free_Dir, i);

            printf("'%s' has been removed successfully.\n", name);
            return;
        }
    }

    printf("Error: '%s' not found in the current directory.\n", name);
}

void cd(char *command) {
    char name[64] = {};
    strncpy(name, command + 3, strlen(command) - 3);
    name[strcspn(name, "\n")] = '\0'; 
    if (strcmp(name, ".") == 0) {
        parent = -2;
        printf("Changed to special directory (-2).\n");
        return;
    }
    for (int i = 0; i < block.directory_size; i++) {
        if (!dir->entries[i].isFree &&
            !dir->entries[i].isDelete &&
            dir->entries[i].parent == parent &&
            strcmp(dir->entries[i].name, name) == 0) {
            if (dir->entries[i].starting_block == -1) {
                parent = i;
                printf("Changed to directory: %s\n", name);
                return;
            } else {
                printf("Error: '%s' is not a directory.\n", name);
                return;
            }
        }
    }
    printf("Error: Directory '%s' not found.\n", name);
}



int main() {

    loadMetadata();
    save_10_block(0);

    dir = malloc(sizeof(DIR));
    fat = malloc(sizeof(FAT));

    // block = initializing_metadata();
    loadDirectory();
    loadFAT();

    // block = initializing_metadata();

    free_Dir = createQueue(block.data_blocks);
    free_fat = createQueue(block.data_blocks);

    load_free_dir();
    load_free_fat();

    save_directory_to_file();
    save_fat_to_file();
    save_10_block(0);
    // create();
    // format();
    parent = -2;

    while(1)
    {
        char command[256];
        bzero(&command, sizeof(command));
        printf("> ");
        fflush(stdout);
        fgets(command, sizeof(command), stdin);
        if(strncmp(command, "FORMAT", 6) == 0)
        {
            format();
        }
        else if(strncmp(command, "mkdir", 5) == 0)
        {
            mkdir(command);
        }
        else if(strncmp(command, "touch", 5) == 0)
        {
            touch(command);
        }
        else if(strncmp(command, "nano", 4) == 0) 
        {
            nano(command);
        }
        else if(strncmp(command, "vim", 3) == 0) 
        {
            vim(command);
        }
        else if(strncmp(command, "cat", 3) == 0) 
        {
            cat(command);
        }
        else if(strncmp(command, "rm", 2) == 0) 
        {
            rm(command);
        }
        else if(strncmp(command, "cd", 2) == 0) 
        {
            cd(command);
        }
        else if(strncmp(command, "ls", 2) == 0) 
        {
            ls();
        }
        else if(strncmp(command, "end", 3) == 0)
        {
            saveDirectory(&dir);
            saveFAT(&fat);
            save_10_block(0);
            printf("Exiting...\n");
            break;
        }
        save_directory_to_file();
        save_fat_to_file();
        save_10_block(0);

    }



    return 0;
}













/*

First I break my disk in blocks so for 64 MB disk Total blocks are 65536

From this I have given 12 blocks to my directory as my directory takes about 11520 bytes remaning are divided in FAT and contents.

Basically I have created a struct for Directory entry which have a name of 64 bytes which can be less I have a size
of file in bytes and a starting block index from FAT a parent directory index a flag for deletion and a flag for free.
Now my directry will be obviously an array of these entries which is of directry size. I can load and store the directory
from and to disk file.

Also I have a struct for FAT which has a boolean value to check if a block is free or not and an integer to store the next 
block index. I can load and store the FAT from and to disk file. 

Some Calculations:
Disk Size = 64MB
My one directry entry = 80 bytes
Directory Size = 80 * 128 = 10240 bytes
One FAT entry = 8 bytes
Total Blocks = 65018
FAT Size = 8 * 65018 = 520144 bytes
Contents = 1024 * 65018 = 66578432 bytes
Total = 64MB


I have also created a queue to store free directory and free FAT entries. I can enqueue and dequeue entries.


*/




