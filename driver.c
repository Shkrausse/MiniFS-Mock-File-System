#include "minifs.h"

int main() {
    printf("MiniFS Demonstration starting...\n");

    printf("1. Formatting disk...\n");
    fs_format();

    printf("2. Mounting file system...\n");
    if (fs_mount() != 0) { printf("Mount failed!\n"); return 1; }

    printf("3. Creating files 'hello.txt' and 'data.bin'...\n");
    fs_create("hello.txt");
    fs_create("data.bin");

    printf("4. Writing to 'hello.txt'...\n");
    int fd = fs_open("hello.txt");
    char *msg = "Hello from MiniFS!";
    fs_write(fd, msg, strlen(msg));
    fs_close(fd);
    printf("   Closed 'hello.txt'\n");

    printf("5. Reopening and reading 'hello.txt'...\n");
    fd = fs_open("hello.txt");
    char buffer[64] = {0};
    fs_read(fd, buffer, 64);
    printf("   Data read: [%s]\n", buffer);
    fs_close(fd);

    printf("6. Listing directory contents...\n");
    fs_ls();

    printf("7. Deleting 'data.bin'...\n");
    fs_delete("data.bin");

    printf("8. Final directory list...\n");
    fs_ls();

    printf("9. Unmounting...\n");
    fs_unmount();

    printf("Demonstration complete.\n");
    return 0;
}