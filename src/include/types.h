#pragma once
#include <stdint.h>

typedef enum {
  SUCCESS = 0x00,
  FAILURE = 0xff
} status_t;

typedef struct {
  uint64_t result;
  status_t status;
} uint64_status_t;
