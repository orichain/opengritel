#pragma once
#include "types.h"

#define LOG_DEBUG(opengritel, fmt, ...) log_write(opengritel, "DEBUG", __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(opengritel, fmt, ...)  log_write(opengritel, "INFO",  __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(opengritel, fmt, ...)  log_write(opengritel, "WARN",  __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(opengritel, fmt, ...) log_write(opengritel, "ERROR", __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)

void log_init(opengritel_context_t *opengritel);
void log_close(opengritel_context_t *opengritel);
void log_write(opengritel_context_t *opengritel, const char *level, const char *file, const char *func, int line, const char *fmt, ...);
void *log_cleaner_thread(void *arg);
