#ifndef PRECONFIG_H
#define PRECONFIG_H

#define SHARE_MEMORY_ALLOC          _IOWR('m', 1, unsigned long)
#define SHARE_MEMORY_ALIGN_ALLOC    _IOWR('m', 2, unsigned long)
#define SHARE_MEMORY_FREE           _IOWR('m', 3, unsigned long)
#define SHARE_MEMORY_SHOW           _IOWR('m', 4, unsigned long)

struct share_memory_alloc_align_args {
    uint32_t size;
    uint32_t alignment;
    uint32_t phyAddr;
};

#define MEMORY_TEST_BLOCK_ALIGN 4096        /* align 4k for mmap */

#define DEV_NAME_H264 "/dev/h264-codec"
#define DEV_NAME_DDR "/dev/mem"
#define SHARE_MEMORY_DEV "/dev/k510-share-memory"
#define DEV_NAME_SYSCTL "/dev/sysctl"

int fd_h264 = 0, fd_mem = 0, fd_share_memory = 0;

#endif