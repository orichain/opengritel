#pragma once
#include "types.h"

uint64_status_t *get_monotonic_time_ns(const char *label);
status_t *sleep_s(double seconds);
