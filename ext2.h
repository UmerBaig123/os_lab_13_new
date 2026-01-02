#ifndef EXT2_H
#define EXT2_H

#include <stdint.h>
#include <time.h>

#define EXT2_BLOCK_SIZE 1024
#define EXT2_INODE_SIZE 128
#define EXT2_ROOT_INODE 2

/* EXT2 Superblock (1024 bytes, located at offset 1024) */
typedef struct {
    uint32_t s_inodes_count;           /* Total inodes in file system */
    uint32_t s_blocks_count;           /* Total blocks in file system */
    uint32_t s_r_blocks_count;         /* Reserved blocks for root */
    uint32_t s_free_blocks_count;      /* Free blocks available */
    uint32_t s_free_inodes_count;      /* Free inodes available */
    uint32_t s_first_data_block;       /* Block number of first data block */
    uint32_t s_log_block_size;         /* log2(block size) - 10 = 1024 bytes */
    uint32_t s_log_frag_size;          /* log2(fragment size) */
    uint32_t s_blocks_per_group;       /* Blocks per block group */
    uint32_t s_frags_per_group;        /* Fragments per block group */
    uint32_t s_inodes_per_group;       /* Inodes per block group */
    uint32_t s_mtime;                  /* Mount time */
    uint32_t s_wtime;                  /* Last write time */
    uint16_t s_mnt_count;              /* Number of mounts */
    uint16_t s_max_mnt_count;          /* Max mounts before check */
    uint16_t s_magic;                  /* Magic number (0xEF53) */
    uint16_t s_state;                  /* File system state */
    uint16_t s_errors;                 /* Error handling method */
    uint16_t s_minor_rev_level;        /* Minor revision level */
    uint32_t s_lastcheck;              /* Last check time */
    uint32_t s_checkinterval;          /* Check interval */
    uint32_t s_creator_os;             /* Creator OS */
    uint32_t s_rev_level;              /* Revision level */
    uint16_t s_def_resuid;             /* Default reserved uid */
    uint16_t s_def_resgid;             /* Default reserved gid */
    uint32_t s_first_ino;              /* First non-reserved inode */
    uint16_t s_inode_size;             /* Inode size */
    uint16_t s_block_group_nr;         /* Block group number */
    uint32_t s_feature_compat;         /* Compatible features */
    uint32_t s_feature_incompat;       /* Incompatible features */
    uint32_t s_feature_ro_compat;      /* Read-only compatible features */
    char s_uuid[16];                   /* Volume UUID */
    char s_volume_name[16];            /* Volume name */
    char s_last_mounted[64];           /* Last mounted on */
    uint32_t s_algo_usage_bitmap;      /* For compression */
    uint8_t s_prealloc_blocks;         /* Prealloc blocks for files */
    uint8_t s_prealloc_dir_blocks;     /* Prealloc blocks for dirs */
    uint16_t s_padding;
    char s_journal_uuid[16];           /* Journal UUID */
    uint32_t s_journal_inum;           /* Journal inode */
    uint32_t s_journal_dev;            /* Journal device */
    uint32_t s_last_orphan;            /* Last orphan inode */
    uint32_t s_reserved[197];          /* Reserved space */
} ext2_superblock_t;

/* EXT2 Block Group Descriptor */
typedef struct {
    uint32_t bg_block_bitmap;          /* Block number of block bitmap */
    uint32_t bg_inode_bitmap;          /* Block number of inode bitmap */
    uint32_t bg_inode_table;           /* Block number of inode table */
    uint16_t bg_free_blocks_count;     /* Free blocks in group */
    uint16_t bg_free_inodes_count;     /* Free inodes in group */
    uint16_t bg_used_dirs_count;       /* Number of directories in group */
    uint16_t bg_pad;
    uint32_t bg_reserved[3];
} ext2_group_desc_t;

/* EXT2 Inode (128 bytes) */
typedef struct {
    uint16_t i_mode;                   /* File mode: permissions, file type */
    uint16_t i_uid;                    /* Owner UID */
    uint32_t i_size;                   /* File size in bytes */
    uint32_t i_atime;                  /* Last access time */
    uint32_t i_ctime;                  /* Creation time */
    uint32_t i_mtime;                  /* Last modification time */
    uint32_t i_dtime;                  /* Deletion time */
    uint16_t i_gid;                    /* Owner GID */
    uint16_t i_links_count;            /* Hard link count */
    uint32_t i_blocks;                 /* Number of 512-byte blocks */
    uint32_t i_flags;                  /* File flags */
    uint32_t i_osd1;                   /* OS dependent 1 */
    uint32_t i_block[15];              /* Direct & indirect block pointers */
    uint32_t i_generation;             /* File generation (for NFS) */
    uint32_t i_file_acl;               /* File ACL */
    uint32_t i_dir_acl;                /* Directory ACL */
    uint32_t i_faddr;                  /* Fragment address */
    uint8_t i_osd2[12];                /* OS dependent 2 */
} ext2_inode_t;

/* EXT2 Directory Entry (variable length) */
typedef struct {
    uint32_t inode;                    /* Inode number */
    uint16_t rec_len;                  /* Entry length in bytes */
    uint8_t name_len;                  /* Name length in bytes */
    uint8_t file_type;                 /* File type */
    /* name follows immediately after, variable length */
} ext2_dir_entry_t;

/* File type constants */
#define EXT2_FT_UNKNOWN 0
#define EXT2_FT_REG_FILE 1
#define EXT2_FT_DIR 2
#define EXT2_FT_CHRDEV 3
#define EXT2_FT_BLKDEV 4
#define EXT2_FT_FIFO 5
#define EXT2_FT_SOCK 6
#define EXT2_FT_SYMLINK 7

/* Inode mode constants */
#define EXT2_S_IFMT 0xF000              /* File type mask */
#define EXT2_S_IFREG 0x8000             /* Regular file */
#define EXT2_S_IFDIR 0x4000             /* Directory */
#define EXT2_S_IFLNK 0xA000             /* Symbolic link */
#define EXT2_S_IRUSR 0x0100             /* Owner read */
#define EXT2_S_IWUSR 0x0080             /* Owner write */
#define EXT2_S_IXUSR 0x0040             /* Owner execute */
#define EXT2_S_IRGRP 0x0020             /* Group read */
#define EXT2_S_IWGRP 0x0010             /* Group write */
#define EXT2_S_IXGRP 0x0008             /* Group execute */
#define EXT2_S_IROTH 0x0004             /* Others read */
#define EXT2_S_IWOTH 0x0002             /* Others write */
#define EXT2_S_IXOTH 0x0001             /* Others execute */

/* Magic number */
#define EXT2_MAGIC 0xEF53

#endif
