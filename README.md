# MiniFS - Contiguous Allocation File System

MiniFS is a modular C implementation of a small file system that uses a virtual disk image (`disk.img`) for storage. It utilizes **contiguous allocation** rather than indexed allocation, meaning each file occupies a solid, unbroken physical region of the disk.

## 1. System Specifications
* **Virtual Disk:** 64 Blocks
* **Block Size:** 256 Bytes
* **Max Files:** 8
* **File Reservation:** 2 blocks (512 bytes) per file
* **Disk Size:** 16,384 bytes (16 KB)

## 2. Virtual Disk Layout
MiniFS divides the `disk.img` into the following blocks:
* **Block 0:** Superblock (Magic number and FS metadata)
* **Block 1:** Free-space Bitmap (Tracks used/free blocks)
* **Blocks 2–3:** File Table / Directory Entries
* **Blocks 4–63:** File Data Area

## 3. Project Files
* `minifs.h`: Header file containing structs for the Superblock, File Entries, and Open File Table.
* `minifs.c`: Core logic implementation (formatting, mounting, allocation, reading/writing).
* `main.c`: Interactive command-driven shell.
* `driver.c`: Automated demonstration program (meets Part 6 requirements).
* `Makefile`: Script to automate the compilation of both programs.

## 4. Compilation Instructions
To build both the interactive shell and the demonstration driver, run:
```bash
make all
