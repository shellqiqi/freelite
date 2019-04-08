#ifndef LOG_H
#define LOG_H

#include "stdio.h"

#ifdef LOG_LEVEL
#define LOG_INFO(fmt, args...)         \
    if (LOG_LEVEL > 0)                 \
    {                                  \
        printf("[INFO] " fmt, ##args); \
    }
#else
#define LOG_INFO(fmt, args...)
#endif

#define LOG_WARN(fmt, args...)         \
    {                                  \
        printf("[WARN] " fmt, ##args); \
    }

#endif