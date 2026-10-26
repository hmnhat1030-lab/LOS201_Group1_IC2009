/* 
 * Tên: Hoàng Minh Nhật - MSSV: SE204273
 * Bài tập: ASSIGNMENT-02 (SMS Capstone)
 * File: sms_sensor.h
 * Mô tả: Header dùng chung giữa Kernel Driver và Userspace App, chứa định nghĩa struct, hằng số ioctl.
 */

#ifndef SMS_SENSOR_H
#define SMS_SENSOR_H

#ifdef __KERNEL__
#include <linux/ioctl.h>
#include <linux/types.h>
#else
#include <sys/ioctl.h>
#include <stdint.h>
#endif

#define SMS_DEVICE_NAME "sms_sensor"
#define SMS_DEVICE_PATH "/dev/sms_sensor"
#define SMS_IOC_MAGIC 'S'

/* Cấu trúc dữ liệu nhiệt độ, độ ẩm mô phỏng đọc từ /dev/sms_sensor */
struct sms_sample {
    uint64_t timestamp_ms;
    float temperature; /* 15.0 -> 45.0 */
    float humidity;    /* 30.0 -> 90.0 */
};

/* Cấu trúc dữ liệu ghi nhận cảnh báo để Alert Thread xử lý */
struct alert_msg {
    char type[32];      /* TEMP_HIGH, HUMID_HIGH, TEMP_SPIKE */
    float value;
    float threshold;
    uint64_t timestamp;
};

/* Lệnh cấu hình IOCTL (yêu cầu cấu hình khoảng đọc) */
#define SMS_SET_INTERVAL    _IOW(SMS_IOC_MAGIC, 1, int)

#endif /* SMS_SENSOR_H */
