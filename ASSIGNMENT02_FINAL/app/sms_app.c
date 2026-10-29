#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include "sms_sensor.h"
#include "circular_buffer.h"
#include "sensor_thread.h"
#include "processor_thread.h"
#include "alert_thread.h"
#include "logger_thread.h"

circular_buffer_t shared_buf;
volatile int keep_running = 1;
int sensor_interval_ms = 500;
float temp_threshold = 40.0f;
float humid_threshold = 80.0f;
int alarm_pipe[2];
int verbose_mode = 0;

void handle_sigint(int sig) {
    (void)sig;
    keep_running = 0;
    pthread_cond_broadcast(&shared_buf.cond_nonempty);
    pthread_cond_broadcast(&shared_buf.cond_nonfull);
}

int main(int argc, char *argv[]) {
    int duration_sec = 60;
    int opt;
    while ((opt = getopt(argc, argv, "i:t:h:d:v")) != -1) {
        switch (opt) {
            case 'i': sensor_interval_ms = atoi(optarg); break;
            case 't': temp_threshold = atof(optarg); break;
            case 'h': humid_threshold = atof(optarg); break;
            case 'd': duration_sec = atoi(optarg); break;
            case 'v': verbose_mode = 1; break;
            default: exit(EXIT_FAILURE);
        }
    }
    signal(SIGINT, handle_sigint);
    cb_init(&shared_buf);

    /* Dung Pipe thay cho POSIX MQ */
    if (pipe(alarm_pipe) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    pthread_t t_logger, t_alert, t_processor, t_sensor;
    pthread_create(&t_logger, NULL, logger_thread_func, NULL);
    pthread_create(&t_alert, NULL, alert_thread_func, NULL);
    pthread_create(&t_processor, NULL, processor_thread_func, NULL);
    pthread_create(&t_sensor, NULL, sensor_thread_func, NULL);

    if (duration_sec > 0) {
        sleep(duration_sec);
        keep_running = 0;
        pthread_cond_broadcast(&shared_buf.cond_nonempty);
        pthread_cond_broadcast(&shared_buf.cond_nonfull);
    }

    pthread_join(t_sensor, NULL);
    
    struct sms_sample dummy = {0, 0.0, 0.0};
    cb_push(&shared_buf, dummy);
    pthread_join(t_processor, NULL);

    struct alert_msg dummy_alert;
    memset(&dummy_alert, 0, sizeof(dummy_alert));
    dummy_alert.timestamp = 0;
    write(alarm_pipe[1], &dummy_alert, sizeof(dummy_alert));
    pthread_join(t_alert, NULL);

    log_message("[Main] System shutdown.\n");
    usleep(100000); 
    pthread_join(t_logger, NULL);

    close(alarm_pipe[0]);
    close(alarm_pipe[1]);
    return 0;
}
