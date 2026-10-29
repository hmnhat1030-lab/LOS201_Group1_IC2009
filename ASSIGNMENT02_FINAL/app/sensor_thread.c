#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <pthread.h>
#include "sensor_thread.h"
#include "circular_buffer.h"

extern circular_buffer_t shared_buf;
extern volatile int keep_running;
extern int sensor_interval_ms;

void *sensor_thread_func(void *arg) {
    struct sched_param param;
    param.sched_priority = 50;
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

    int fd = open(SMS_DEVICE_PATH, O_RDONLY);
    if (fd < 0) {
        perror("SensorThread: Failed to open driver");
        return NULL;
    }

    char buf[128];
    while (keep_running) {
        ssize_t bytes = read(fd, buf, sizeof(buf) - 1);
        if (bytes > 0) {
            buf[bytes] = '\0';
            struct sms_sample sample;
            int temp_i, temp_f, hum_i, hum_f;
            
            /* Parse chuoi CSV tu Kernel Driver tra ve */
            if (sscanf(buf, "%llu,%d.%d,%d.%d", &sample.timestamp_ms, &temp_i, &temp_f, &hum_i, &hum_f) == 5) {
                sample.temperature = temp_i + temp_f / 10.0f;
                sample.humidity = hum_i + hum_f / 10.0f;
                cb_push(&shared_buf, sample);
            }
        }
        /* Reset vi tri doc de goi lan sau khong bi EOF */
        lseek(fd, 0, SEEK_SET);
        usleep(sensor_interval_ms * 1000);
    }
    close(fd);
    return NULL;
}
