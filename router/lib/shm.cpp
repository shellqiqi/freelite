#include "shm.hpp"
#include "../../inc/log.h"

/**
 * Alloc shared memory with client
 * Input:
 *    name: shared memory name
 *    size: shared memory size
 * Output:
 *    addr: shared memory address of router
 * Return: error code
 */
int alloc_shm_mem(const char *name,
                  const size_t size,
                  void **addr)
{
    int fd;

    if ((fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) < 0)
    {
        LOG_WARN("/dev/shm/%s exist.\n", name);
        if ((fd = shm_open(name, O_RDWR, S_IRUSR | S_IWUSR)) < 0)
        {
            LOG_PERROR("SHM OPEN ERROR");
            return -1;
        }
    }

    if (ftruncate(fd, size) < 0)
    {
        LOG_PERROR("SHM EXTEND ERROR");
        return -1;
    }

    *addr = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (*addr == MAP_FAILED)
    {
        LOG_PERROR("MMAP FAILED");
        return -1;
    }

    return 0;
}

/**
 * Free shared memory with client
 * Input:
 *    name: shared memory name
 *    size: shared memory size
 *    addr: shared memory address of router
 * Return: error code
 */
int free_shm_mem(const char *name,
                 size_t size,
                 void *addr)
{
    if (munmap(addr, size) < 0)
    {
        LOG_PERROR("SHM MUNMAP ERROR");
        return -1;
    }

    if (shm_unlink(name) < 0)
    {
        LOG_PERROR("SHM UNLINK ERROR");
        return -1;
    }

    return 0;
}