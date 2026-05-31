#pragma once
#include <stdint.h>
#include <stdio.h>

typedef enum {
  SUCCESS = 0x00,
  FAILURE = 0xff
} status_t;

typedef struct {
  uint64_t result;
  status_t status;
} uint64_status_t;

typedef struct {
  FILE *log_fp;
  char *current_log_filename;
} opengritel_context_t;
