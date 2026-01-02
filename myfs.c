#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ext2.h"

typedef struct {
    int fd;                             /* File descriptor for disk image */
    ext2_superblock_t superblock;      /* Superblock */
    ext2_group_desc_t *group_descs;    /* Group descriptors */
    int num_groups;                     /* Number of block groups */
} ext2_fs_t;

/* Function prototypes */
int ext2_open(const char *img_path, ext2_fs_t *fs);
void ext2_close(ext2_fs_t *fs);
int ext2_read_superblock(ext2_fs_t *fs);
int ext2_read_group_descriptors(ext2_fs_t *fs);
int ext2_read_inode(ext2_fs_t *fs, uint32_t ino, ext2_inode_t *inode);
int ext2_read_block(ext2_fs_t *fs, uint32_t block_num, void *buffer);
int ext2_ls(ext2_fs_t *fs, const char *path);
int ext2_cp(ext2_fs_t *fs, const char *src, const char *dst);
uint32_t ext2_find_inode(ext2_fs_t *fs, const char *path);

/*
 * Opens the EXT2 disk image
 */
int ext2_open(const char *img_path, ext2_fs_t *fs) {
    fs->fd = open(img_path, O_RDONLY);
    if (fs->fd < 0) {
        perror("Error opening disk image");
        return -1;
    }
    
    if (ext2_read_superblock(fs) != 0) {
        close(fs->fd);
        return -1;
    }
    
    if (ext2_read_group_descriptors(fs) != 0) {
        close(fs->fd);
        return -1;
    }
    
    return 0;
}

/*
 * Closes the EXT2 disk image
 */
void ext2_close(ext2_fs_t *fs) {
    if (fs->group_descs) {
        free(fs->group_descs);
    }
    if (fs->fd >= 0) {
        close(fs->fd);
    }
}

/*
 * Reads the superblock from the disk image
 * Superblock is located at offset 1024 bytes
 */
int ext2_read_superblock(ext2_fs_t *fs) {
    if (lseek(fs->fd, 1024, SEEK_SET) != 1024) {
        perror("Error seeking to superblock");
        return -1;
    }
    
    if (read(fs->fd, &fs->superblock, sizeof(ext2_superblock_t)) != sizeof(ext2_superblock_t)) {
        perror("Error reading superblock");
        return -1;
    }
    
    /* Verify magic number */
    if (fs->superblock.s_magic != EXT2_MAGIC) {
        fprintf(stderr, "Invalid EXT2 magic number: 0x%x\n", fs->superblock.s_magic);
        return -1;
    }
    
    int block_size = 1024 << fs->superblock.s_log_block_size;
    
    printf("EXT2 Filesystem loaded successfully\n");
    printf("s_log_block_size: %u\n", fs->superblock.s_log_block_size);
    printf("Block size: %u bytes\n", block_size);
    printf("Total inodes: %u\n", fs->superblock.s_inodes_count);
    printf("Total blocks: %u\n", fs->superblock.s_blocks_count);
    
    return 0;
}

/*
 * Reads block group descriptors
 */
int ext2_read_group_descriptors(ext2_fs_t *fs) {
    int block_size = 1024 << fs->superblock.s_log_block_size;
    int group_desc_size = sizeof(ext2_group_desc_t);
    
    /* Calculate number of block groups */
    fs->num_groups = (fs->superblock.s_blocks_count + fs->superblock.s_blocks_per_group - 1) / 
                     fs->superblock.s_blocks_per_group;
    
    /* Allocate space for group descriptors */
    fs->group_descs = (ext2_group_desc_t *)malloc(fs->num_groups * group_desc_size);
    if (!fs->group_descs) {
        perror("Error allocating memory for group descriptors");
        return -1;
    }
    
    /* Group descriptors start at block 1 (after superblock) */
    uint32_t group_desc_block = (block_size == 1024) ? 2 : 1;
    
    if (lseek(fs->fd, group_desc_block * block_size, SEEK_SET) < 0) {
        perror("Error seeking to group descriptors");
        free(fs->group_descs);
        return -1;
    }
    
    ssize_t bytes_read = read(fs->fd, fs->group_descs, fs->num_groups * group_desc_size);
    if (bytes_read != fs->num_groups * group_desc_size) {
        perror("Error reading group descriptors");
        free(fs->group_descs);
        return -1;
    }
    
    return 0;
}

/*
 * Reads an inode from the disk
 */
int ext2_read_inode(ext2_fs_t *fs, uint32_t ino, ext2_inode_t *inode) {
    if (ino < 1 || ino > fs->superblock.s_inodes_count) {
        fprintf(stderr, "Invalid inode number: %u\n", ino);
        return -1;
    }
    
    int inode_size = fs->superblock.s_inode_size;
    int block_size = 1024 << fs->superblock.s_log_block_size;
    int inodes_per_group = fs->superblock.s_inodes_per_group;
    
    /* Calculate which group this inode belongs to */
    int group = (ino - 1) / inodes_per_group;
    int index_in_group = (ino - 1) % inodes_per_group;
    
    /* Get inode table location from group descriptor */
    uint32_t inode_table_block = fs->group_descs[group].bg_inode_table;
    
    /* Calculate offset */
    off_t offset = (off_t)inode_table_block * block_size + index_in_group * inode_size;
    
    if (lseek(fs->fd, offset, SEEK_SET) != offset) {
        perror("Error seeking to inode");
        return -1;
    }
    
    /* Read the inode - only read the fixed 128-byte part */
    if (read(fs->fd, inode, 128) != 128) {
        perror("Error reading inode");
        return -1;
    }
    
    /* If the inode is larger than 128 bytes, we need to skip past the extra bytes */
    /* But for now we only care about the first 128 bytes */
    
    return 0;
}

/*
 * Reads a block from the disk
 */
/*
 * Reads a block from the disk
 */
int ext2_read_block(ext2_fs_t *fs, uint32_t block_num, void *buffer) {
    int block_size = 1024 << fs->superblock.s_log_block_size;
    off_t offset = (off_t)block_num * (off_t)block_size;
    
    if (lseek(fs->fd, offset, SEEK_SET) != offset) {
        perror("Error seeking to block");
        return -1;
    }
    
    ssize_t bytes_read = read(fs->fd, buffer, block_size);
    if (bytes_read != block_size) {
        perror("Error reading block");
        return -1;
    }
    
    return 0;
}

/*
 * Lists files in a directory (ls implementation)
 */
int ext2_ls(ext2_fs_t *fs, const char *path) {
    uint32_t inode_num = ext2_find_inode(fs, path);
    if (inode_num == 0) {
        fprintf(stderr, "Directory not found: %s\n", path);
        return -1;
    }
    
    ext2_inode_t inode;
    if (ext2_read_inode(fs, inode_num, &inode) != 0) {
        return -1;
    }
    
    /* Check if it's a directory */
    if ((inode.i_mode & EXT2_S_IFMT) != EXT2_S_IFDIR) {
        fprintf(stderr, "Not a directory: %s\n", path);
        return -1;
    }
    
    int block_size = 1024 << fs->superblock.s_log_block_size;
    uint8_t *dir_block = (uint8_t *)malloc(block_size);
    if (!dir_block) {
        perror("Error allocating memory for directory block");
        return -1;
    }
    
    printf("Contents of '%s':\n", path);
    printf("%-30s %-10s %-10s\n", "Name", "Type", "Inode");
    printf("----------------------------------------------\n");
    
    /* Read directory entries from all data blocks */
    uint32_t blocks_read = 0;
    uint32_t dir_size = inode.i_size;
    
    for (int i = 0; i < 12 && blocks_read * block_size < dir_size; i++) {
        if (inode.i_block[i] == 0) break;
        
        if (ext2_read_block(fs, inode.i_block[i], dir_block) != 0) {
            free(dir_block);
            return -1;
        }
        
            /* Parse directory entries */
            uint32_t offset = 0;
            while (offset < (uint32_t)block_size && offset < dir_size) {
                if (offset + sizeof(ext2_dir_entry_t) > (uint32_t)block_size) {
                    break;  /* Not enough space for even the header */
                }
                
                ext2_dir_entry_t *entry = (ext2_dir_entry_t *)(dir_block + offset);
                
                if (entry->rec_len == 0) {
                    break;  /* Prevent infinite loop */
                }
                
                if (entry->inode == 0) {
                    offset += entry->rec_len;
                    continue;
                }
                
                /* Bounds check: name_len should not exceed rec_len - header size */
                uint16_t max_name_len = entry->rec_len - sizeof(ext2_dir_entry_t);
                if (entry->name_len > max_name_len) {
                    entry->name_len = max_name_len;
                }
                
                if (entry->name_len > 255) {
                    entry->name_len = 255;
                }
                
                char name[256];
                char *name_ptr = (char *)(entry + 1);  /* Name starts right after the header */
                strncpy(name, name_ptr, entry->name_len);
                name[entry->name_len] = '\0';
            
            const char *type_str = "unknown";
            switch (entry->file_type) {
                case EXT2_FT_REG_FILE: type_str = "file"; break;
                case EXT2_FT_DIR: type_str = "dir"; break;
                case EXT2_FT_SYMLINK: type_str = "link"; break;
            }
            
            printf("%-30s %-10s %-10u\n", name, type_str, entry->inode);
            
            offset += entry->rec_len;
            if (entry->rec_len == 0) break;
        }
        
        blocks_read++;
    }
    
    free(dir_block);
    return 0;
}

int ext2_cp(ext2_fs_t *fs, const char *src, const char *dst) {
    uint32_t inode_num = ext2_find_inode(fs, src);
    if (inode_num == 0) {
        fprintf(stderr, "File not found: %s\n", src);
        return -1;
    }
    
    ext2_inode_t inode;
    if (ext2_read_inode(fs, inode_num, &inode) != 0) {
        return -1;
    }
    
    /* Check if it's a regular file */
    if ((inode.i_mode & EXT2_S_IFMT) != EXT2_S_IFREG) {
        fprintf(stderr, "Not a regular file: %s\n", src);
        return -1;
    }
    
    /* Create output file */
    int out_fd = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd < 0) {
        perror("Error creating output file");
        return -1;
    }
    
    int block_size = 1024 << fs->superblock.s_log_block_size;
    
    /* Allocate buffer for reading blocks */
    uint8_t *file_block = (uint8_t *)malloc(block_size);
    if (!file_block) {
        perror("Error allocating memory for file block");
        close(out_fd);
        return -1;
    }
    
    uint32_t bytes_written = 0;
    uint32_t file_size = inode.i_size;
    
    /* Read direct blocks (first 12 blocks) */
    for (int i = 0; i < 12 && bytes_written < file_size; i++) {
        if (inode.i_block[i] == 0) break;
        
        if (ext2_read_block(fs, inode.i_block[i], file_block) != 0) {
            perror("Error reading file block");
            close(out_fd);
            free(file_block);
            return -1;
        }
        
        /* Calculate how many bytes to write from this block */
        uint32_t bytes_remaining = file_size - bytes_written;
        uint32_t bytes_to_write = (bytes_remaining < (uint32_t)block_size) ? bytes_remaining : block_size;
        
        if (write(out_fd, file_block, bytes_to_write) != (int)bytes_to_write) {
            perror("Error writing to output file");
            close(out_fd);
            free(file_block);
            return -1;
        }
        
        bytes_written += bytes_to_write;
    }
    
    free(file_block);
    close(out_fd);
    
    printf("File copied: %s -> %s (%u bytes)\n", src, dst, file_size);
    return 0;
}

/*
 * Finds an inode by path
 */
uint32_t ext2_find_inode(ext2_fs_t *fs, const char *path) {
    /* Start with root inode */
    uint32_t current_inode = EXT2_ROOT_INODE;
    
    /* Handle root directory */
    if (strcmp(path, "/") == 0) {
        return current_inode;
    }
    
    /* Make a copy of path to avoid modifying the original */
    char *path_copy = strdup(path);
    if (!path_copy) {
        return 0;
    }
    
    /* Parse path components */
    char *saveptr;
    char *component = strtok_r(path_copy, "/", &saveptr);
    
    while (component != NULL) {
        ext2_inode_t inode;
        if (ext2_read_inode(fs, current_inode, &inode) != 0) {
            free(path_copy);
            return 0;
        }
        
        /* Check if current inode is a directory */
        if ((inode.i_mode & EXT2_S_IFMT) != EXT2_S_IFDIR) {
            free(path_copy);
            return 0;
        }
        
        /* Search for component in directory */
        int block_size = 1024 << fs->superblock.s_log_block_size;
        uint8_t *dir_block = (uint8_t *)malloc(block_size);
        if (!dir_block) {
            free(path_copy);
            return 0;
        }
        
        int found = 0;
        uint32_t dir_size = inode.i_size;
        
        for (int i = 0; i < 12 && !found; i++) {
            if (inode.i_block[i] == 0) break;
            
            if (ext2_read_block(fs, inode.i_block[i], dir_block) != 0) {
                free(dir_block);
                free(path_copy);
                return 0;
            }
            
            /* Parse directory entries */
            uint32_t offset = 0;
            while (offset < (uint32_t)block_size && offset < dir_size && !found) {
                if (offset + sizeof(ext2_dir_entry_t) > (uint32_t)block_size) {
                    break;  /* Not enough space for even the header */
                }
                
                ext2_dir_entry_t *entry = (ext2_dir_entry_t *)(dir_block + offset);
                
                if (entry->rec_len == 0) {
                    break;  /* Prevent infinite loop */
                }
                
                if (entry->inode == 0) {
                    offset += entry->rec_len;
                    continue;
                }
                
                /* Bounds check: name_len should not exceed rec_len - header size */
                uint16_t max_name_len = entry->rec_len - sizeof(ext2_dir_entry_t);
                if (entry->name_len > max_name_len) {
                    entry->name_len = max_name_len;
                }
                
                if (entry->name_len > 255) {
                    entry->name_len = 255;
                }
                
                char name[256];
                char *name_ptr = (char *)(entry + 1);  /* Name starts right after the header */
                strncpy(name, name_ptr, entry->name_len);
                name[entry->name_len] = '\0';
                
                if (strcmp(name, component) == 0) {
                    current_inode = entry->inode;
                    found = 1;
                }
                
                offset += entry->rec_len;
                if (entry->rec_len == 0) break;
            }
        }
        
        free(dir_block);
        
        if (!found) {
            free(path_copy);
            return 0;
        }
        
        component = strtok_r(NULL, "/", &saveptr);
    }
    
    free(path_copy);
    return current_inode;
}

/*
 * Main function
 */
int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <disk_image> <command> [args]\n", argv[0]);
        fprintf(stderr, "Commands:\n");
        fprintf(stderr, "  ls [path]          - List files in directory\n");
        fprintf(stderr, "  cp <src> <dst>     - Copy file from image to host\n");
        return 1;
    }
    
    const char *img_path = argv[1];
    const char *command = argv[2];
    
    ext2_fs_t fs;
    memset(&fs, 0, sizeof(fs));
    
    if (ext2_open(img_path, &fs) != 0) {
        fprintf(stderr, "Failed to open EXT2 image\n");
        return 1;
    }
    
    int result = 0;
    
    if (strcmp(command, "ls") == 0) {
        const char *path = (argc > 3) ? argv[3] : "/";
        result = ext2_ls(&fs, path);
    } else if (strcmp(command, "cp") == 0) {
        if (argc < 5) {
            fprintf(stderr, "Usage: %s <disk_image> cp <src> <dst>\n", argv[0]);
            ext2_close(&fs);
            return 1;
        }
        const char *src = argv[3];
        const char *dst = argv[4];
        result = ext2_cp(&fs, src, dst);
    } else {
        fprintf(stderr, "Unknown command: %s\n", command);
        result = 1;
    }
    
    ext2_close(&fs);
    return result;
}
