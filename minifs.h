#ifndef MINIFS_H
#define MINIFS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DISK_NAME "disk.img"
#define TOTAL_BLOCKS 64
#define BLOCK_SIZE 256
#define MAX_FILES 8
#define MAX_OPEN_FILES 4
#define FILE_BLOCKS 2
#define MAX_FILE_SIZE (FILE_BLOCKS * BLOCK_SIZE)

// Part 1: File Representation
typedef struct {
    char name[32];
    int in_use;
    int size_bytes;
    int start_block;
    int num_blocks;
} FileEntry;

// Part 2: Superblock
typedef struct {
    int magic;
    int block_size;
    int total_blocks;
    int max_files;
} Superblock;

// Part 4: Open File Table
typedef struct {
    int entry_index;
    int read_pos;
    int in_use;
} OpenFile;

// Function Prototypes
int fs_format();
int fs_mount();
int fs_unmount();
int fs_create(char *name);
int fs_delete(char *name);
int fs_open(char *name);
void fs_close(int fd);
int fs_write(int fd, void *data, int len);
int fs_read(int fd, void *data, int len);
void fs_ls();

// Extra Credit: Additional Commands
void fs_stat(char *name);       /* Print detailed file metadata */
int  fs_rename(char *old_name, char *new_name); /* Rename a file */
int  fs_seek(int fd, int pos);  /* Set read position for a file descriptor */
void fs_bitmap();               /* Display the free-space bitmap visually */

#endif
