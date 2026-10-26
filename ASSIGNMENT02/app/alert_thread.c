#include <stdio.h>
#include <unistd.h>
#include "alert_thread.h"
#include "sms_sensor.h"

extern int alarm_pipe[2];
extern volatile int keep_running;
extern void log_message(const char *msg);

void *alert_thread_func(void *arg) {
    (void)arg;
    struct alert_msg alert;
    char msg_buf[256];

    while (keep_running) {
        ssize_t bytes = read(alarm_pipe[0], &alert, sizeof(alert));
        if (bytes > 0 && alert.timestamp != 0) {
            snprintf(msg_buf, sizeof(msg_buf), "[ALERT][%llu] TYPE: %s VALUE: %.2f THRESHOLD: %.2f\n",
                     (unsigned long long)alert.timestamp, alert.type, alert.value, alert.threshold);
            fprintf(stderr, "%s", msg_buf);
            log_message(msg_buf);
        } else if (bytes <= 0 || alert.timestamp == 0) {
            break;
        }
    }
    return NULL;
}
