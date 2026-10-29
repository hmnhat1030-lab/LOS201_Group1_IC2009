#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "processor_thread.h"
#include "circular_buffer.h"

extern circular_buffer_t shared_buf;
extern volatile int keep_running;
extern float temp_threshold;
extern float humid_threshold;
extern int alarm_pipe[2];

void *processor_thread_func(void *arg) {
    (void)arg;
    float last_temp = 0.0f;
    int count = 0;

    while (keep_running) {
        struct sms_sample sample = cb_pop(&shared_buf);
        if (!keep_running || sample.timestamp_ms == 0) break;

        int is_anomaly = 0;
        struct alert_msg alert;
        memset(&alert, 0, sizeof(alert));
        alert.timestamp = sample.timestamp_ms;

        if (sample.temperature > temp_threshold) {
            is_anomaly = 1;
            strcpy(alert.type, "TEMP_HIGH");
            alert.value = sample.temperature;
            alert.threshold = temp_threshold;
        } else if (sample.humidity > humid_threshold) {
            is_anomaly = 1;
            strcpy(alert.type, "HUMID_HIGH");
            alert.value = sample.humidity;
            alert.threshold = humid_threshold;
        } else if (count > 0 && fabs(sample.temperature - last_temp) > 5.0f) {
            is_anomaly = 1;
            strcpy(alert.type, "TEMP_SPIKE");
            alert.value = fabs(sample.temperature - last_temp);
            alert.threshold = 5.0f;
        }

        if (is_anomaly) {
            write(alarm_pipe[1], &alert, sizeof(alert));
        }

        last_temp = sample.temperature;
        count++;
    }
    return NULL;
}
