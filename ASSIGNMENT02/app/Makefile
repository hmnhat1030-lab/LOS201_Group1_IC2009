CC_ARM = arm-linux-gnueabihf-gcc
CC_HOST = gcc
CFLAGS = -Wall -Wextra -pthread
LDFLAGS_ARM = -lrt -lm -static
LDFLAGS_HOST = -lrt -lm

SRCS = sms_app.c circular_buffer.c sensor_thread.c processor_thread.c alert_thread.c logger_thread.c
TARGET_ARM = sms_app
TARGET_HOST = sms_app_x86

all: arm host

arm:
	$(CC_ARM) $(CFLAGS) $(SRCS) -o $(TARGET_ARM) $(LDFLAGS_ARM)

host:
	$(CC_HOST) $(CFLAGS) $(SRCS) -o $(TARGET_HOST) $(LDFLAGS_HOST)

clean:
	rm -f $(TARGET_ARM) $(TARGET_HOST) *.o
