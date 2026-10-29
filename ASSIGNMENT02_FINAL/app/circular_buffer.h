#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <pthread.h>
#include "sms_sensor.h"

#define BUFFER_SIZE 64

typedef struct {
    struct sms_sample buffer[BUFFER_SIZE];
    int head;
    int tail;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond_nonempty;
    pthread_cond_t cond_nonfull;
} circular_buffer_t;

void cb_init(circular_buffer_t *cb);
void cb_push(circular_buffer_t *cb, struct sms_sample item);
struct sms_sample cb_pop(circular_buffer_t *cb);

#endif
