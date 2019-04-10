#ifndef LOG_H
#define LOG_H

#include "stdio.h"

#ifdef LOG_LEVEL
#define LOG_INFO(fmt, args...)         \
    if (LOG_LEVEL >= 2)                \
    {                                  \
        printf("[INFO] " fmt, ##args); \
    }
#define LOG_DEBUG(fmt, args...)         \
    if (LOG_LEVEL >= 1)                 \
    {                                   \
        printf("[DEBUG] " fmt, ##args); \
    }
#else
#define LOG_INFO(fmt, args...)
#define LOG_DEBUG(fmt, args...)
#endif

#define LOG_NORMAL(fmt, args...)         \
    {                                    \
        printf("[NORMAL] " fmt, ##args); \
    }

#define LOG_WARN(fmt, args...)             \
    {                                      \
        printf("[WARNING] " fmt, ##args); \
    }

#define LOG_ERROR(fmt, args...)                  \
    {                                            \
        fprintf(stderr, "[ERROR] " fmt, ##args); \
    }

#define LOG_PERROR(fmt)          \
    {                            \
        perror("[ERROR] " fmt); \
    }

#endif