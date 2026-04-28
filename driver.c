/*
 * driver.c - MiniFS Demonstration Program
 *
 * This program automatically demonstrates the MiniFS file system by
 * performing a scripted sequence of operations and clearly reporting
 * the success or failure of each one.
 *
 * It shows that the MiniFS implementation is modular and can be used
 * by a separate C program, not just through the interactive shell.
 */

#include "minifs.h"

/* Helper macro to print pass/fail based on a return value */
#define REPORT(op, result) \
    printf("   -> %s: %s\n", (op), ((result) >= 0) ? "SUCCESS" : "FAILED")

int main() {
    int fd, result, bytes;
    char buf[128];

    printf("==============================================\n");
    printf("   MiniFS Demonstration Program\n");
    printf("==============================================\n\n");

    /* ---- Step 1: Format ---- */
    printf("[1] Formatting disk (disk.img)...\n");
    result = fs_format();
    REPORT("format", result);

    /* ---- Step 2: Mount ---- */
    printf("\n[2] Mounting file system...\n");
    result = fs_mount();
    REPORT("mount", result);
    if (result < 0) {
        printf("Cannot continue without a mounted file system.\n");
        return 1;
    }

    /* ---- Step 3: Create two files ---- */
    printf("\n[3] Creating files 'notes.txt' and 'data.bin'...\n");
    result = fs_create("notes.txt");
    REPORT("create notes.txt", result);

    result = fs_create("data.bin");
    REPORT("create data.bin", result);

    /* ---- Step 4: Open a file and write to it ---- */
    printf("\n[4] Opening 'notes.txt' and writing data...\n");
    fd = fs_open("notes.txt");
    REPORT("open notes.txt", fd);

    if (fd >= 0) {
        char *msg = "Hello from MiniFS driver!";
        bytes = fs_write(fd, msg, strlen(msg));
        printf("   -> write \"%s\": %s (%d bytes written)\n",
               msg, (bytes >= 0) ? "SUCCESS" : "FAILED", bytes);
    }

    /* ---- Step 5: Close and reopen ---- */
    printf("\n[5] Closing 'notes.txt' (fd=%d) and reopening...\n", fd);
    fs_close(fd);
    printf("   -> close fd=%d: SUCCESS\n", fd);

    fd = fs_open("notes.txt");
    REPORT("reopen notes.txt", fd);

    /* ---- Step 6: Read data back ---- */
    printf("\n[6] Reading data back from 'notes.txt'...\n");
    if (fd >= 0) {
        memset(buf, 0, sizeof(buf));
        bytes = fs_read(fd, buf, sizeof(buf) - 1);
        if (bytes > 0) {
            buf[bytes] = '\0';
            printf("   -> read %d bytes: SUCCESS\n", bytes);
            printf("   -> content: [%s]\n", buf);
        } else {
            printf("   -> read: FAILED\n");
        }
        fs_close(fd);
        printf("   -> close fd=%d: SUCCESS\n", fd);
    }

    /* ---- Step 7: Write to second file ---- */
    printf("\n[7] Writing to 'data.bin'...\n");
    fd = fs_open("data.bin");
    REPORT("open data.bin", fd);
    if (fd >= 0) {
        char *binmsg = "BinaryData:12345";
        bytes = fs_write(fd, binmsg, strlen(binmsg));
        printf("   -> write \"%s\": %s (%d bytes written)\n",
               binmsg, (bytes >= 0) ? "SUCCESS" : "FAILED", bytes);
        fs_close(fd);
        printf("   -> close fd=%d: SUCCESS\n", fd);
    }

    /* ---- Step 8: List directory ---- */
    printf("\n[8] Listing directory contents...\n");
    fs_ls();

    /* ---- Step 9: Attempt to delete an open file (should fail) ---- */
    printf("\n[9] Testing delete-while-open protection...\n");
    fd = fs_open("notes.txt");
    printf("   -> opened 'notes.txt' as fd=%d\n", fd);
    result = fs_delete("notes.txt");
    printf("   -> delete 'notes.txt' while open: %s (expected FAILED)\n",
           (result < 0) ? "FAILED (correct!)" : "SUCCESS (BUG!)");
    fs_close(fd);
    printf("   -> closed fd=%d\n", fd);

    /* ---- Step 10: Delete one file ---- */
    printf("\n[10] Deleting 'data.bin'...\n");
    result = fs_delete("data.bin");
    REPORT("delete data.bin", result);

    /* ---- Step 11: Final directory listing ---- */
    printf("\n[11] Final directory listing (should show only 'notes.txt')...\n");
    fs_ls();

    /* ---- Step 12: Unmount ---- */
    printf("\n[12] Unmounting file system...\n");
    result = fs_unmount();
    REPORT("unmount", result);

    printf("\n==============================================\n");
    printf("   Demonstration complete.\n");
    printf("==============================================\n");

    return 0;
}
