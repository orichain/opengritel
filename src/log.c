#include "types.h"
#include "utilities.h"
#include "globals.h"
#include <dirent.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

char *get_log_filename() {
  time_t t = time(NULL);
  struct tm tm_info;
  localtime_r(&t, &tm_info);
  size_t len = snprintf(NULL, 0, "logs/%04d-%02d-%02d.log", tm_info.tm_year + 1900, tm_info.tm_mon + 1, tm_info.tm_mday);
  len++;
  char *buf = (char *)calloc(1, len);
  snprintf(buf, len, "logs/%04d-%02d-%02d.log", tm_info.tm_year + 1900, tm_info.tm_mon + 1, tm_info.tm_mday);
  return buf;
}

void log_init() {
  struct stat st = {0};
  if (stat("logs", &st) == -1) {
    mkdir("logs", 0755);
  }
  current_log_filename = get_log_filename();
  log_fp = fopen(current_log_filename, "w+");
}

void log_close() {
  if (log_fp) {
    fclose(log_fp);
    free(current_log_filename);
    log_fp = NULL;
  }
}

char *get_time_str() {
  time_t t = time(NULL);
  struct tm tm_info;
  localtime_r(&t, &tm_info);
  size_t len = snprintf(NULL, 0, "%04d-%02d-%02d %02d:%02d:%02d", 
                      tm_info.tm_year + 1900, 
                      tm_info.tm_mon + 1, 
                      tm_info.tm_mday, 
                      tm_info.tm_hour, 
                      tm_info.tm_min, 
                      tm_info.tm_sec);
  len++;
  char *buf = (char *)calloc(1, len);
  snprintf(buf, len, "%04d-%02d-%02d %02d:%02d:%02d", 
                      tm_info.tm_year + 1900, 
                      tm_info.tm_mon + 1, 
                      tm_info.tm_mday, 
                      tm_info.tm_hour, 
                      tm_info.tm_min, 
                      tm_info.tm_sec);
  return buf;
}

void cleanup_old_logs(int max_age_days) {
  DIR *dir = opendir("logs");
  if (!dir) {
    fprintf(stderr, "Failed to open logs directory: %s\n", strerror(errno));
    return;
  }
  struct dirent *entry;
  time_t now = time(NULL);
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type != DT_REG) continue;
    if (strlen(entry->d_name) != 14) continue;
    if (strstr(entry->d_name, ".log") == NULL) continue;
    if (entry->d_name[0] < '0' || entry->d_name[0] > '9') continue;
    int year, month, day;
    if (sscanf(entry->d_name, "%4d-%2d-%2d.log", &year, &month, &day) != 3)
      continue;
    struct tm file_tm = {0};
    file_tm.tm_year = year - 1900;
    file_tm.tm_mon  = month - 1;
    file_tm.tm_mday = day;
    file_tm.tm_hour = 0;
    file_tm.tm_min  = 0;
    file_tm.tm_sec  = 0;
    time_t file_time = mktime(&file_tm);
    if (file_time == -1) {
      fprintf(stderr, "Invalid mktime() for: %s\n", entry->d_name);
      continue;
    }
    double diff_days = difftime(now, file_time) / (60 * 60 * 24);
    if (diff_days >= max_age_days) {
      char filepath[128];
      snprintf(filepath, sizeof(filepath), "logs/%s", entry->d_name);
      if (remove(filepath) == 0) {
        fprintf(stderr, "Deleted old log: %s\n", filepath);
      } else {
        fprintf(stderr, "Failed to delete old log: %s - %s\n", filepath, strerror(errno));
      }
    }
  }
  closedir(dir);
}

void *log_cleaner_thread(void *arg) {
  uint64_status_t *rs = get_monotonic_time_ns("[LOG]: ");
  if (rs->status == SUCCESS) {
    uint64_t current_time = rs->result;
    uint64_t start_time = current_time;
    uint64_t clean_every = (uint64_t)86400 * 1000000000ULL;
    cleanup_old_logs(7);
    while (!shutdown_requested) {
      free(rs);
      rs = get_monotonic_time_ns("[LOG]: ");
      if (rs->status == SUCCESS) {
        current_time = rs->result;
        if ((current_time - start_time) > clean_every) {
          if (!shutdown_requested) cleanup_old_logs(7);
          start_time = current_time;
        }
        status_t *slp = sleep_s(1);
        if (*slp != SUCCESS) {
          free(slp);
          continue;
        }
        free(slp);
      } else {
        fprintf(stderr, "[LOG]: Log cleaner failed to get current_time.\n");
      }
    }
    free(rs);
  } else {
    fprintf(stderr, "[LOG]: Log cleaner failed to start %s\n", strerror(errno));
  }
  return NULL;
}

void log_write(const char *level, const char *file, const char *func, int line, const char *fmt, ...) {
  char *filename = get_log_filename();
  if (strcmp(filename, current_log_filename) != 0) {
    log_close();
    current_log_filename = get_log_filename();
    log_fp = fopen(current_log_filename, "a");
    if (!log_fp) {
      fprintf(stderr, "Failed to open log file: %s\n", strerror(errno));free(filename);
      free(filename);
      return;
    }
  }
  free(filename);
  char *timebuf = get_time_str();
  fprintf(log_fp, "[%s] [%s] (%s:%s:%d)\n", timebuf, level, file, func, line);
  free(timebuf);
  va_list args;
  va_start(args, fmt);
  vfprintf(log_fp, fmt, args);
  va_end(args);
  fprintf(log_fp, "\n");
  fflush(log_fp);
}
