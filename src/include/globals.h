#pragma once
#include <signal.h>
#include <stdio.h>

extern volatile sig_atomic_t shutdown_requested;
extern FILE *log_fp;
extern char *current_log_filename;
