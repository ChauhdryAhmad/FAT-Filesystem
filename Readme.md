# FAT-File System Simulation

This project is a simple simulation of a file system in C. It implements basic file system functionalities like creating files, directories, reading, writing, and managing blocks on a simulated disk of 64mB.

## Features

- **Disk Initialization**: Initializes a virtual disk with a specified size and block size.
- **Directory Management**: Creates and manages directories.
- **File Operations**: Supports creating, writing, reading, and appending to files.
- **File Allocation Table (FAT)**: Manages the allocation of blocks for files using a FAT.
- **Persistence**: Reads from and writes to a simulated disk file (`disk.bin`).

## Files

- `queue.c`: A helper file providing queue operations, used for managing free directory and FAT entries.
- `disk.bin`: A binary file representing the virtual disk.
- `directory.txt`: A text file where directory information is saved.
- `FAT.txt`: A text file where FAT information is saved.
- `block.txt`: A text file where the contents of specific blocks are saved.

## Structures

- **`superblock`**: Stores metadata about the file system.
- **`fat_entry`**: Represents an entry in the FAT.
- **`dir_entry`**: Represents an entry in the directory.

## Functions

### Initialization and Loading

- `superblock initializing_metadata()`: Initializes the file system metadata.
- `void loadMetadata()`: Loads the file system metadata from the disk.
- `int loadDirectory()`: Loads the directory structure from the disk.
- `int saveDirectory()`: Saves the directory structure to the disk.
- `int loadFAT()`: Loads the FAT from the disk.
- `int saveFAT()`: Saves the FAT to the disk.

### File and Directory Operations

- `void mkdir(char *command)`: Creates a new directory.
- `void touch(char *command)`: Creates a new file.
- `void nano(char *command)`: Edits a file by adding new content.
- `void vim(char *command)`: Reads the content of a file.
- `void cat(char *command)`: Appends content to an existing file.

### Disk Operations

- `void writediskBlock(int blockIndex, const char *data)`: Writes data to a specific block on the disk.
- `void writeBlock(int blockIndex, const char *data)`: Writes data to a block in the data section of the disk.
- `void readBlock(int blockIndex, char *buffer)`: Reads data from a block on the disk.

### Utility

- `void save_directory_to_file()`: Saves the directory structure to `directory.txt`.
- `void save_fat_to_file()`: Saves the FAT to `FAT.txt`.
- `void save_10_block(int n)`: Saves the content of 10 consecutive blocks starting from `n` to `block.txt`.
- `void create()`: Initializes the disk with empty blocks.
- `void format()`: Formats the disk, initializing metadata, directory, and FAT.

## Usage

1. **Initialization**: Call `initializing_metadata()` to set up the file system.
2. **Creating Files/Directories**: Use `mkdir` and `touch` to create directories and files.
3. **Reading/Writing Files**: Use `vim`, `nano`, and `cat` to read and write file content.
4. **Saving and Loading**: Use `saveDirectory`, `saveFAT`, `loadDirectory`, and `loadFAT` to manage persistence.

## Requirements

- C Compiler (GCC or similar)
- Standard C Libraries: `stdio.h`, `stdlib.h`, `string.h`, `stdbool.h`, `stdint.h`
- Custom `queue.c` for managing free entries in directory and FAT

## Compilation

To compile the program, use the following command:

```bash
gcc -o file_system_simulation main.c 
```

## Run the program

To run the program, run the following command

```bash
./file_system_simulation
```

## Note

After running the program for first time its recomended to FORMAT the disk and files like `directory.txt`, `FAT.txt` and `block.txt` are just for debugging purposes.

If you want to create your own virtual disk you have to create function and to change the attributes of disk like size of disk or directory or FAT you can change them in superblock struct.

Thank You!! 😊


