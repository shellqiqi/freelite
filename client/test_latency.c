/*
 * Copyright (c) 2018 Yizhou Shan <ys@purdue.edu>. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#define _GNU_SOURCE
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <pthread.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <stdbool.h>
#include <malloc.h>
#include <getopt.h>
#include <sched.h>

#include "../inc/lite-lib.h"

struct thread_info
{
    int tid;
    int remote_nid;
};

static int nr_threads = 1;
static int remote_node = -1;
static double *per_rps;
static pthread_barrier_t thread_barrier;
static unsigned int pg_size;
static uint64_t test_key;

static inline void die(const char *str, ...)
{
    va_list args;
    va_start(args, str);
    vfprintf(stderr, str, args);
    fputc('\n', stderr);
    exit(1);
}

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define MAX_BUF_SIZE (1024 * 1024 * 32)

/*
 * How many runs each size will run.
 * Amortize random variation.
 */
#define NR_TESTS_PER_SIZE (1000 * 1000)
#define NSEC_PER_SEC (1000 * 1000 * 1000)

static inline long timespec_diff_ns(struct timespec end, struct timespec start)
{
    long e, s;

    e = end.tv_sec * NSEC_PER_SEC + end.tv_nsec;
    s = start.tv_sec * NSEC_PER_SEC + start.tv_nsec;
    return e - s;
}

static int bind_thread(int cpu_id)
{
    cpu_set_t cpu_set;

    CPU_ZERO(&cpu_set);
    CPU_SET(cpu_id, &cpu_set);

    return sched_setaffinity(0, sizeof(cpu_set), &cpu_set);
}

int testsize[] = {8, 64, 512, 1024, 1024 * 2, 1024 * 4, 1024 * 8};

static void print_rps(const char *who)
{
    int i, j;
    double total = 0;

    for (j = 0; j < ARRAY_SIZE(testsize); j++)
    {
        total = 0;
        for (i = 0; i < nr_threads; i++)
            total += per_rps[j * nr_threads + i];

        printf("Per thread: #%d %s, throughput: \033[31m %10lf \033[0m size=%#10x (%10d)\n",
               NR_TESTS_PER_SIZE, who, total / nr_threads, testsize[j],
               testsize[j]);
    }
}

static void *rdma_write_read(void *_info)
{
    int i, j;
    int password = 100;
    char *buf;
    char shm_name[18];
    void *remote_addr;
    struct thread_info *info = (struct thread_info *)_info;

    /*
     * XXX
     * Hard-coded distribution
     */
    bind_thread(55 - info->tid);
    printf("  thread[%d] running on CPU%2d\n", info->tid, sched_getcpu());

    // buf = (char *)aligned_alloc(pg_size, MAX_BUF_SIZE);
    sprintf(shm_name, "%s%05d", "test_latency", info->tid);
    if (userspace_liteapi_alloc_local_mem(shm_name, MAX_BUF_SIZE, (void **)&buf, &remote_addr) < 0)
    {
        fprintf(stderr, "userspace_liteapi_alloc_local_mem error\n");
        pthread_exit(NULL);
    }
    printf("alloc shm: %s\n", shm_name);
    if (!buf)
        die("oom");

    memset(buf, 'A', 1024 * 64);

    /*
     * Let one thread do the allocation
     * Rest of the team reuse the same buffer.
     */
    if (pthread_barrier_wait(&thread_barrier) == PTHREAD_BARRIER_SERIAL_THREAD)
    {
        test_key = userspace_liteapi_alloc_remote_mem(remote_node, MAX_BUF_SIZE, 0, password);
        printf("Finish remote mem alloc. Key: %#lx %ld\n", test_key, test_key);
        printf(" \033[32mTest RDMA Write (avg of # %d run)\033[0m\n", NR_TESTS_PER_SIZE);
    }

    for (i = 0; i < ARRAY_SIZE(testsize); i++)
    {
        struct timespec start, end;
        long diff_ns;
        double rps;

        clock_gettime(CLOCK_MONOTONIC, &start);
        for (j = 0; j < NR_TESTS_PER_SIZE; j++)
        {
            userspace_liteapi_rdma_write(test_key, remote_addr, testsize[i], 0, password);
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        diff_ns = timespec_diff_ns(end, start);

        rps = (double)NR_TESTS_PER_SIZE / ((double)diff_ns / (double)NSEC_PER_SEC);

        per_rps[i * nr_threads + info->tid] = rps;

#if 1
        printf("  [tid %d] CPU%2d  size = %#10x (%10d) avg_time = %15ld ns RPS: %15lf\n",
               info->tid,
               sched_getcpu(),
               testsize[i],
               testsize[i],
               diff_ns / NR_TESTS_PER_SIZE,
               rps);
#endif
    }

    if (pthread_barrier_wait(&thread_barrier) == PTHREAD_BARRIER_SERIAL_THREAD)
    {
        print_rps("write");
        printf(" \033[32mTest RDMA Sync Read (avg of # %d run)\033[0m\n", NR_TESTS_PER_SIZE);
    }

    for (i = 0; i < ARRAY_SIZE(testsize); i++)
    {
        struct timespec start, end;
        long diff_ns;
        double rps;

        clock_gettime(CLOCK_MONOTONIC, &start);
        for (j = 0; j < NR_TESTS_PER_SIZE; j++)
        {
            userspace_liteapi_rdma_read(test_key, remote_addr, testsize[i], 0, password);
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        diff_ns = timespec_diff_ns(end, start);

        rps = (double)NR_TESTS_PER_SIZE / ((double)diff_ns / (double)NSEC_PER_SEC);
        per_rps[i * nr_threads + info->tid] = rps;

#if 1
        printf("  CPU%2d  size = %#10x (%10d) avg_time = %15ld ns RPS: %15lf\n",
               sched_getcpu(),
               testsize[i],
               testsize[i],
               diff_ns / NR_TESTS_PER_SIZE,
               rps);
#endif
    }

    if (pthread_barrier_wait(&thread_barrier) == PTHREAD_BARRIER_SERIAL_THREAD)
    {
        print_rps("sync_read");
        printf(" \033[33mTest RDMA Async Read (avg of # %d run)\033[0m\n", NR_TESTS_PER_SIZE);
    }
    if (userspace_liteapi_free_local_mem(shm_name, MAX_BUF_SIZE, buf, remote_addr) < 0)
    {
        fprintf(stderr, "userspace_liteapi_free_local_mem error");
    }
    pthread_exit(NULL);
}

static struct option long_options[] = {
    {.name = "remote_nid", .has_arg = 1, .val = 'n'},
    {.name = "thread", .has_arg = 1, .val = 't'},
};

int main(int argc, char *argv[])
{
    int i;
    struct thread_info *info;
    pthread_t *threads;
    int cpu;

    while (1)
    {
        int c;

        c = getopt_long(argc, argv, "t:n:", long_options, NULL);

        if (c == -1)
            break;

        switch (c)
        {
        case 'n':
            remote_node = strtoul(optarg, NULL, 0);
            if (remote_node > 16)
            {
                return -1;
            }
            break;
        case 't':
            nr_threads = strtoul(optarg, NULL, 0);
            if (nr_threads > 56)
            {
                printf("Too many threads: %d\n", nr_threads);
                return -1;
            }
            break;
        default:
            return -1;
        };
    }

    if (remote_node < 1)
        return -1;

    printf("remote_nid: %d nr_threads: %d\n", remote_node, nr_threads);

    pg_size = sysconf(_SC_PAGESIZE);

    threads = (pthread_t *)malloc(sizeof(*threads) * nr_threads);
    per_rps = (double *)malloc(sizeof(*per_rps) * nr_threads * ARRAY_SIZE(testsize));
    info = (struct thread_info *)malloc(sizeof(*info) * nr_threads);
    memset(info, 0, sizeof(*info) * nr_threads);

    pthread_barrier_init(&thread_barrier, NULL, nr_threads);

    for (i = 0; i < nr_threads; i++)
    {
        info[i].tid = i;
        info[i].remote_nid = remote_node;
        pthread_create(&threads[i], NULL, rdma_write_read, info + i);
    }
    for (i = 0; i < nr_threads; i++)
        pthread_join(threads[i], NULL);

    return 0;
}
