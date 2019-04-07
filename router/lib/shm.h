#ifndef SHM_H
#define SHM_H

#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int alloc_shm_mem(const char *name,
                  const size_t size,
                  void *addr);

int free_shm_mem(const char *name,
                 size_t size,
                 void *addr);

#endif