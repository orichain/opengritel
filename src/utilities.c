#include "types.h"
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

uint64_status_t *get_monotonic_time_ns(const char *label) {
  uint64_status_t *rs = (uint64_status_t *)calloc(1, sizeof(uint64_status_t));
  rs->status = FAILURE;
  rs->result = 0;
  struct timespec ts;
  int gtm = clock_gettime(CLOCK_MONOTONIC, &ts);
  while (gtm == -1 && errno == EINTR) {
    gtm = clock_gettime(CLOCK_MONOTONIC, &ts);
  }
  if (gtm == -1) {
    rs->status = FAILURE;
    rs->result = 0;
  } else {
    rs->result = (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
    rs->status = SUCCESS;
  }
  return rs;
}

status_t *sleep_ns(long nanoseconds) {
  status_t *stt = (status_t *)calloc(1, sizeof(status_t));
  if (nanoseconds < 0) {
    *stt = FAILURE;
    return stt;
  }
  struct timespec ts;
  ts.tv_sec = nanoseconds / 1000000000L;
  ts.tv_nsec = nanoseconds % 1000000000L;
  int nslp = nanosleep(&ts, &ts);
  while (nslp == -1 && errno == EINTR) {
    nslp = nanosleep(&ts, &ts);
  }
  if (nslp == -1) {
    *stt = FAILURE;
  } else {
    *stt = SUCCESS;
  }
  return stt;
}

status_t *sleep_us(long microseconds) {
  return sleep_ns(microseconds * 1000L);
}

status_t *sleep_ms(long milliseconds) {
  return sleep_ns(milliseconds * 1000000L);
}

status_t *sleep_s(double seconds) {
  long ns = (long)(fmin(seconds, 60 * 60 * 24) * 1e9);
  return sleep_ns(ns);
}

