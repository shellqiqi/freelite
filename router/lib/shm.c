#include "shm.h"

int alloc_shm_mem(const char *name,
                  const size_t size,
                  void **addr)
{
    int fd;

    if ((fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) < 0)
    {
        printf("WARN: /dev/shm/%s exist.\n", name);
        if ((fd = shm_open(name, O_RDWR, S_IRUSR | S_IWUSR)) < 0)
        {
            perror("SHM OPEN ERROR\n");
            return -1;
        }
    }

    if (ftruncate(fd, size) < 0)
    {
        perror("SHM EXTEND ERROR\n");
        return -1;
    }

    *addr = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (*addr == MAP_FAILED)
    {
        perror("MMAP FAILED\n");
        return -1;
    }

    return 0;
}

int free_shm_mem(const char *name,
                 size_t size,
                 void *addr)
{
    if (munmap(addr, size) < 0)
    {
        perror("SHM MUNMAP ERROR\n");
        return -1;
    }

    if (shm_unlink(name) < 0)
    {
        perror("SHM UNLINK ERROR\n");
        return -1;
    }

    return 0;
}