#include "minifs.h"

Superblock sb;
FileEntry directory[MAX_FILES];
char bitmap[TOTAL_BLOCKS];
OpenFile open_table[MAX_OPEN_FILES];
FILE *disk_fp = NULL;

int fs_format() {
    FILE *fp = fopen(DISK_NAME, "wb");
    if (!fp) return -1;

    // Initialize blocks with zeros
    char zero_block[BLOCK_SIZE] = {0};
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        fwrite(zero_block, BLOCK_SIZE, 1, fp);
    }

    // Set Superblock (Block 0)
    Superblock new_sb = {0x4d494e49, BLOCK_SIZE, TOTAL_BLOCKS, MAX_FILES};
    fseek(fp, 0, SEEK_SET);
    fwrite(&new_sb, sizeof(Superblock), 1, fp);

    // Set Bitmap (Block 1): Blocks 0-3 are system reserved
    char new_bitmap[TOTAL_BLOCKS] = {0};
    for (int i = 0; i < 4; i++) new_bitmap[i] = 1; 
    fseek(fp, BLOCK_SIZE, SEEK_SET);
    fwrite(new_bitmap, TOTAL_BLOCKS, 1, fp);

    fclose(fp);
    return 0;
}

int fs_mount() {
    disk_fp = fopen(DISK_NAME, "rb+");
    if (!disk_fp) return -1;

    fseek(disk_fp, 0, SEEK_SET);
    fread(&sb, sizeof(Superblock), 1, disk_fp);

    fseek(disk_fp, BLOCK_SIZE, SEEK_SET);
    fread(bitmap, TOTAL_BLOCKS, 1, disk_fp);

    fseek(disk_fp, BLOCK_SIZE * 2, SEEK_SET);
    fread(directory, sizeof(FileEntry), MAX_FILES, disk_fp);

    for (int i = 0; i < MAX_OPEN_FILES; i++) open_table[i].in_use = 0;

    return 0;
}

int fs_unmount() {
    if (!disk_fp) return -1;

    fseek(disk_fp, BLOCK_SIZE, SEEK_SET);
    fwrite(bitmap, TOTAL_BLOCKS, 1, disk_fp);

    fseek(disk_fp, BLOCK_SIZE * 2, SEEK_SET);
    fwrite(directory, sizeof(FileEntry), MAX_FILES, disk_fp);

    fclose(disk_fp);
    disk_fp = NULL;
    return 0;
}

int fs_create(char *name) {
    int entry_idx = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (!directory[i].in_use) { entry_idx = i; break; }
    }
    if (entry_idx == -1) return -1;

    // Part 3: Contiguous Allocation Search
    int start = -1;
    for (int i = 4; i < TOTAL_BLOCKS - 1; i++) {
        if (bitmap[i] == 0 && bitmap[i+1] == 0) {
            start = i;
            break;
        }
    }
    if (start == -1) return -1;

    strncpy(directory[entry_idx].name, name, 32);
    directory[entry_idx].in_use = 1;
    directory[entry_idx].size_bytes = 0;
    directory[entry_idx].start_block = start;
    directory[entry_idx].num_blocks = FILE_BLOCKS;

    bitmap[start] = 1;
    bitmap[start+1] = 1;

    return 0;
}

int fs_open(char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].in_use && strcmp(directory[i].name, name) == 0) {
            for (int j = 0; j < MAX_OPEN_FILES; j++) {
                if (!open_table[j].in_use) {
                    open_table[j].entry_index = i;
                    open_table[j].read_pos = 0;
                    open_table[j].in_use = 1;
                    return j;
                }
            }
        }
    }
    return -1;
}

void fs_close(int fd) {
    if (fd >= 0 && fd < MAX_OPEN_FILES) open_table[fd].in_use = 0;
}

int fs_write(int fd, void *data, int len) {
    if (fd < 0 || fd >= MAX_OPEN_FILES || !open_table[fd].in_use) return -1;
    
    FileEntry *fe = &directory[open_table[fd].entry_index];
    
    // Check if append would exceed reserved 512 bytes (2 blocks)
    if (fe->size_bytes + len > MAX_FILE_SIZE) {
        printf("Error: Write exceeds reserved contiguous space.\n");
        return -1;
    }

    // Contiguous Append: Seek to (Start Block * 256) + current size
    int phys_offset = (fe->start_block * BLOCK_SIZE) + fe->size_bytes;
    fseek(disk_fp, phys_offset, SEEK_SET);
    
    if (fwrite(data, 1, len, disk_fp) != (size_t)len) return -1;

    fe->size_bytes += len; // Update size after successful append
    return len;
}

int fs_read(int fd, void *data, int len) {
    if (fd < 0 || !open_table[fd].in_use) return -1;
    
    FileEntry *fe = &directory[open_table[fd].entry_index];
    int available = fe->size_bytes - open_table[fd].read_pos;
    int to_read = (len < available) ? len : available;

    int phys_offset = (fe->start_block * BLOCK_SIZE) + open_table[fd].read_pos;
    fseek(disk_fp, phys_offset, SEEK_SET);
    fread(data, to_read, 1, disk_fp);

    open_table[fd].read_pos += to_read;
    return to_read;
}

int fs_delete(char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].in_use && strcmp(directory[i].name, name) == 0) {
            // Check if any open descriptor points to this file entry
            for (int j = 0; j < MAX_OPEN_FILES; j++) {
                if (open_table[j].in_use && open_table[j].entry_index == i) {
                    printf("Error: Cannot delete an open file.\n");
                    return -1;
                }
            }
            // Free the contiguous blocks in bitmap
            bitmap[directory[i].start_block] = 0;
            bitmap[directory[i].start_block + 1] = 0;
            directory[i].in_use = 0;
            return 0;
        }
    }
    return -1;
}

void fs_ls() {
    printf("\n--- File List ---\n");
    printf("%-15s %-10s %-10s\n", "Name", "Size", "Start Block");
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].in_use) {
            printf("%-15s %-10d %-10d\n", directory[i].name, directory[i].size_bytes, directory[i].start_block);
        }
    }
    printf("-----------------\n");
}

/* ---------------------------------------------------------------
 * Extra Credit: stat - Print detailed metadata for a named file
 * --------------------------------------------------------------- */
void fs_stat(char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].in_use && strcmp(directory[i].name, name) == 0) {
            printf("\n--- stat: %s ---\n", name);
            printf("  Directory slot : %d\n", i);
            printf("  In use         : %s\n", directory[i].in_use ? "yes" : "no");
            printf("  File size      : %d bytes\n", directory[i].size_bytes);
            printf("  Start block    : %d\n", directory[i].start_block);
            printf("  Blocks reserved: %d\n", directory[i].num_blocks);
            printf("  Max capacity   : %d bytes\n", MAX_FILE_SIZE);
            printf("  Space used     : %d / %d bytes\n",
                   directory[i].size_bytes, MAX_FILE_SIZE);
            return;
        }
    }
    printf("stat: file '%s' not found.\n", name);
}

/* ---------------------------------------------------------------
 * Extra Credit: rename - Rename a file (must not be open)
 * --------------------------------------------------------------- */
int fs_rename(char *old_name, char *new_name) {
    /* Prevent renaming to a name that already exists */
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].in_use && strcmp(directory[i].name, new_name) == 0) {
            printf("Error: A file named '%s' already exists.\n", new_name);
            return -1;
        }
    }

    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].in_use && strcmp(directory[i].name, old_name) == 0) {
            /* Prevent renaming an open file */
            for (int j = 0; j < MAX_OPEN_FILES; j++) {
                if (open_table[j].in_use && open_table[j].entry_index == i) {
                    printf("Error: Cannot rename an open file.\n");
                    return -1;
                }
            }
            strncpy(directory[i].name, new_name, 32);
            directory[i].name[31] = '\0'; /* ensure null termination */
            return 0;
        }
    }
    printf("rename: file '%s' not found.\n", old_name);
    return -1;
}

/* ---------------------------------------------------------------
 * Extra Credit: seek - Set the read position for a file descriptor
 * --------------------------------------------------------------- */
int fs_seek(int fd, int pos) {
    if (fd < 0 || fd >= MAX_OPEN_FILES || !open_table[fd].in_use) {
        printf("Error: Invalid file descriptor.\n");
        return -1;
    }
    FileEntry *fe = &directory[open_table[fd].entry_index];
    if (pos < 0 || pos > fe->size_bytes) {
        printf("Error: Seek position %d out of range (0-%d).\n",
               pos, fe->size_bytes);
        return -1;
    }
    open_table[fd].read_pos = pos;
    return 0;
}

/* ---------------------------------------------------------------
 * Extra Credit: bitmap - Display the free-space bitmap visually
 * --------------------------------------------------------------- */
void fs_bitmap() {
    printf("\n--- Free-Space Bitmap (%d blocks, block size %d bytes) ---\n",
           TOTAL_BLOCKS, BLOCK_SIZE);
    printf("  Legend: [#] = allocated, [.] = free\n\n");

    /* Print column indices header */
    printf("     ");
    for (int c = 0; c < 16; c++) printf("%2d ", c);
    printf("\n");

    /* Print bitmap rows of 16 blocks each */
    for (int row = 0; row < TOTAL_BLOCKS / 16; row++) {
        printf("%3d: ", row * 16);
        for (int col = 0; col < 16; col++) {
            int blk = row * 16 + col;
            printf("[%c] ", bitmap[blk] ? '#' : '.');
        }
        printf("\n");
    }

    /* Count free blocks */
    int free_count = 0;
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        if (!bitmap[i]) free_count++;
    }
    printf("\n  Free blocks: %d / %d  (%d bytes available)\n",
           free_count, TOTAL_BLOCKS, free_count * BLOCK_SIZE);
    printf("--------------------------------------------------------\n");
}
