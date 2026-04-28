#include "minifs.h"

void print_help() {
    printf("Commands:\n");
    printf("  format              - Initialize a fresh file system on disk.img\n");
    printf("  mount               - Load file system metadata from disk.img\n");
    printf("  unmount             - Save metadata and close the file system\n");
    printf("  ls                  - List all files (name, size, start block)\n");
    printf("  create <name>       - Create a new file with 2 reserved data blocks\n");
    printf("  delete <name>       - Remove a file and free its blocks\n");
    printf("  open <name>         - Open a file and return a file descriptor\n");
    printf("  close <fd>          - Close an open file descriptor\n");
    printf("  read <fd> <len>     - Read len bytes from current read position\n");
    printf("  write <fd> <data>   - Append data string to end of file\n");
    printf("  --- Extra Credit ---\n");
    printf("  stat <name>         - Print detailed metadata for a file\n");
    printf("  rename <old> <new>  - Rename a file\n");
    printf("  seek <fd> <pos>     - Set the read position for a file descriptor\n");
    printf("  bitmap              - Display the free-space bitmap\n");
    printf("  exit                - Exit the shell\n");
}

int main() {
    char line[256];
    char cmd[16], arg1[64], arg2[128];

    printf("MiniFS Shell - Type 'help' for commands\n");

    while (1) {
        printf("minifs> ");
        if (!fgets(line, sizeof(line), stdin)) break;

        /* Reset arguments before each parse */
        cmd[0] = '\0'; arg1[0] = '\0'; arg2[0] = '\0';
        sscanf(line, "%15s %63s %127[^\n]", cmd, arg1, arg2);

        /* Skip blank lines */
        if (cmd[0] == '\0') continue;

        if (strcmp(cmd, "exit") == 0) break;
        else if (strcmp(cmd, "help") == 0) print_help();
        else if (strcmp(cmd, "format") == 0) {
            if (fs_format() == 0) printf("Disk formatted.\n");
            else printf("Format failed.\n");
        }
        else if (strcmp(cmd, "mount") == 0) {
            if (fs_mount() == 0) printf("Mounted successfully.\n");
            else printf("Mount failed.\n");
        }
        else if (strcmp(cmd, "unmount") == 0) {
            if (fs_unmount() == 0) printf("Unmounted.\n");
            else printf("Unmount failed (not mounted?).\n");
        }
        else if (strcmp(cmd, "ls") == 0) fs_ls();
        else if (strcmp(cmd, "create") == 0) {
            if (arg1[0] == '\0') { printf("Usage: create <name>\n"); }
            else if (fs_create(arg1) == 0) printf("File '%s' created.\n", arg1);
            else printf("Error: could not create '%s' (disk full or name taken).\n", arg1);
        }
        else if (strcmp(cmd, "delete") == 0) {
            if (arg1[0] == '\0') { printf("Usage: delete <name>\n"); }
            else if (fs_delete(arg1) == 0) printf("File '%s' deleted.\n", arg1);
            else printf("Error: could not delete '%s'.\n", arg1);
        }
        else if (strcmp(cmd, "open") == 0) {
            if (arg1[0] == '\0') { printf("Usage: open <name>\n"); }
            else {
                int fd = fs_open(arg1);
                if (fd >= 0) printf("Opened '%s'. FD: %d\n", arg1, fd);
                else printf("Error: could not open '%s' (file not found or table full).\n", arg1);
            }
        }
        else if (strcmp(cmd, "close") == 0) {
            if (arg1[0] == '\0') { printf("Usage: close <fd>\n"); }
            else {
                int fd = atoi(arg1);
                fs_close(fd);
                printf("FD %d closed.\n", fd);
            }
        }
        else if (strcmp(cmd, "write") == 0) {
            if (arg1[0] == '\0' || arg2[0] == '\0') { printf("Usage: write <fd> <data>\n"); }
            else {
                int result = fs_write(atoi(arg1), arg2, strlen(arg2));
                if (result >= 0) printf("Wrote %d bytes.\n", result);
                else printf("Write failed (file full or invalid FD).\n");
            }
        }
        else if (strcmp(cmd, "read") == 0) {
            if (arg1[0] == '\0' || arg2[0] == '\0') { printf("Usage: read <fd> <numBytes>\n"); }
            else {
                int len = atoi(arg2);
                if (len <= 0) { printf("Error: invalid byte count.\n"); }
                else {
                    char *buf = malloc(len + 1);
                    if (!buf) { printf("Error: out of memory.\n"); }
                    else {
                        int bytes = fs_read(atoi(arg1), buf, len);
                        if (bytes > 0) {
                            buf[bytes] = '\0';
                            printf("Read %d bytes: %s\n", bytes, buf);
                        } else {
                            printf("Read returned 0 bytes (end of file or invalid FD).\n");
                        }
                        free(buf);
                    }
                }
            }
        }
        /* ---- Extra Credit Commands ---- */
        else if (strcmp(cmd, "stat") == 0) {
            if (arg1[0] == '\0') printf("Usage: stat <name>\n");
            else fs_stat(arg1);
        }
        else if (strcmp(cmd, "rename") == 0) {
            if (arg1[0] == '\0' || arg2[0] == '\0') printf("Usage: rename <old> <new>\n");
            else {
                if (fs_rename(arg1, arg2) == 0)
                    printf("Renamed '%s' to '%s'.\n", arg1, arg2);
                else
                    printf("Error: rename failed.\n");
            }
        }
        else if (strcmp(cmd, "seek") == 0) {
            if (arg1[0] == '\0' || arg2[0] == '\0') printf("Usage: seek <fd> <pos>\n");
            else {
                if (fs_seek(atoi(arg1), atoi(arg2)) == 0)
                    printf("Seek to position %s on FD %s.\n", arg2, arg1);
                else
                    printf("Error: seek failed.\n");
            }
        }
        else if (strcmp(cmd, "bitmap") == 0) {
            fs_bitmap();
        }
        else {
            printf("Unknown command '%s'. Type 'help'.\n", cmd);
        }
    }
    return 0;
}
