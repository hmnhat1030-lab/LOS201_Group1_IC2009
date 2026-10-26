#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "logger_thread.h"

#define LOG_FILE "/tmp/sms.log"

extern volatile int keep_running;

static char log_buffer[1024];
static int log_ready = 0;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t log_cond = PTHREAD_COND_INITIALIZER;

void log_message(const char *msg) {
    pthread_mutex_lock(&log_mutex);
    strncpy(log_buffer, msg, sizeof(log_buffer) - 1);
    log_ready = 1;
    pthread_cond_signal(&log_cond);
    pthread_mutex_unlock(&log_mutex);
}

void *logger_thread_func(void *arg) {
    FILE *fp = fopen(LOG_FILE, "w");
    if (!fp) {
        perror("LoggerThread: Không thể mở file log");
        return NULL;
    }

    while (keep_running) {
        pthread_mutex_lock(&log_mutex);
        while (!log_ready && keep_running) {
            pthread_cond_wait(&log_cond, &log_mutex);
        }
        if (log_ready) {
            fprintf(fp, "%s", log_buffer);
            fflush(fp); /* Xả đệm để ghi ngay vào file */
            log_ready = 0;
        }
        pthread_mutex_unlock(&log_mutex);
    }
    
    fclose(fp);
    return NULL;
}
