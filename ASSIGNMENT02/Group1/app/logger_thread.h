#ifndef LOGGER_THREAD_H
#define LOGGER_THREAD_H

void *logger_thread_func(void *arg);
void log_message(const char *msg);

#endif
