#include "circular_buffer.h"

void cb_init(circular_buffer_t *cb) {
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
    pthread_mutex_init(&cb->mutex, NULL);
    pthread_cond_init(&cb->cond_nonempty, NULL);
    pthread_cond_init(&cb->cond_nonfull, NULL);
}

void cb_push(circular_buffer_t *cb, struct sms_sample item) {
    pthread_mutex_lock(&cb->mutex);
    while (cb->count == BUFFER_SIZE) {
        pthread_cond_wait(&cb->cond_nonfull, &cb->mutex);
    }
    cb->buffer[cb->head] = item;
    cb->head = (cb->head + 1) % BUFFER_SIZE;
    cb->count++;
    pthread_cond_signal(&cb->cond_nonempty);
    pthread_mutex_unlock(&cb->mutex);
}

struct sms_sample cb_pop(circular_buffer_t *cb) {
    struct sms_sample item;
    pthread_mutex_lock(&cb->mutex);
    while (cb->count == 0) {
        pthread_cond_wait(&cb->cond_nonempty, &cb->mutex);
    }
    item = cb->buffer[cb->tail];
    cb->tail = (cb->tail + 1) % BUFFER_SIZE;
    cb->count--;
    pthread_cond_signal(&cb->cond_nonfull);
    pthread_mutex_unlock(&cb->mutex);
    return item;
}
