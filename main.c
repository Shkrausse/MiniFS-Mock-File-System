#include "minifs.h"

void print_help() {
    printf("Commands: format, mount, unmount, ls, create <name>, delete <name>,\n");
    printf("          open <name>, close <fd>, read <fd> <len>, write <fd> <string>, exit\n");
}

int main() {
    char line[128];
    char cmd[16], arg1[32], arg2[64];

    printf("MiniFS Shell - Type 'help' for commands\n");

    while (1) {
        printf("minifs> ");
        if (!fgets(line, sizeof(line), stdin)) break;

        // Reset arguments
        arg1[0] = '\0'; arg2[0] = '\0';
        int num_args = sscanf(line, "%s %s %[^\n]", cmd, arg1, arg2);

        if (strcmp(cmd, "exit") == 0) break;
        else if (strcmp(cmd, "help") == 0) print_help();
        else if (strcmp(cmd, "format") == 0) {
            if (fs_format() == 0) printf("Disk formatted.\n");
        } 
        else if (strcmp(cmd, "mount") == 0) {
            if (fs_mount() == 0) printf("Mounted successfully.\n");
            else printf("Mount failed.\n");
        } 
        else if (strcmp(cmd, "unmount") == 0) {
            fs_unmount();
            printf("Unmounted.\n");
        } 
        else if (strcmp(cmd, "ls") == 0) fs_ls();
        else if (strcmp(cmd, "create") == 0) {
            if (fs_create(arg1) == 0) printf("File created.\n");
            else printf("Error creating file.\n");
        } 
        else if (strcmp(cmd, "delete") == 0) {
            if (fs_delete(arg1) == 0) printf("File deleted.\n");
            else printf("Error deleting file.\n");
        } 
        else if (strcmp(cmd, "open") == 0) {
            int fd = fs_open(arg1);
            if (fd >= 0) printf("Opened %s. FD: %d\n", arg1, fd);
            else printf("Error opening file.\n");
        } 
        else if (strcmp(cmd, "close") == 0) {
            fs_close(atoi(arg1));
            printf("FD %s closed.\n", arg1);
        } 
        else if (strcmp(cmd, "write") == 0) {
            int result = fs_write(atoi(arg1), arg2, strlen(arg2));
            if (result >= 0) printf("Wrote %d bytes.\n", result);
            else printf("Write failed (File full or invalid FD).\n");
        } 
        else if (strcmp(cmd, "read") == 0) {
            int len = atoi(arg2);
            char *buf = malloc(len + 1);
            int bytes = fs_read(atoi(arg1), buf, len);
            if (bytes >= 0) {
                buf[bytes] = '\0';
                printf("Read %d bytes: %s\n", bytes, buf);
            } else printf("Read failed.\n");
            free(buf);
        } 
        else {
            printf("Unknown command. Type 'help'.\n");
        }
    }
    return 0;
}