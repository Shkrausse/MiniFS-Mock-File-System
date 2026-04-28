# MiniFS – Mock File System

A simplified block-based file system written in C that simulates core OS file-system
concepts using a virtual disk image (`disk.img`).

---

## Files

| File | Purpose |
|------|---------|
| `minifs.h` | Shared header: structs, constants, and function prototypes |
| `minifs.c` | Core file-system implementation |
| `main.c` | Interactive MiniFS shell |
| `driver.c` | Automated demonstration program |
| `Makefile` | Build script |

---

## Disk Layout

| Block(s) | Contents |
|----------|---------|
| 0 | Superblock (magic number, block size, total blocks, max files) |
| 1 | Free-space bitmap (1 byte per block; 1 = allocated, 0 = free) |
| 2–3 | File table / directory entries (up to 8 FileEntry structs) |
| 4–63 | File data area (available for file content) |

- **Block size**: 256 bytes  
- **Total blocks**: 64  
- **Max files**: 8  
- **Max file size**: 512 bytes (2 contiguous blocks per file)

---

## How to Compile

```bash
make
```

This produces two executables:
- `minifs_shell` – the interactive command-line shell
- `minifs_driver` – the automated demonstration program

To clean build artifacts:

```bash
