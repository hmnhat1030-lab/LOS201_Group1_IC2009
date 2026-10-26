#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/random.h>
#include <linux/ktime.h>
#include "sms_sensor.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SE204273_HoangMinhNhat");
MODULE_DESCRIPTION("SMS Sensor Virtual Driver");

static int read_count = 0;
static int last_temp_x10 = 0;
static int last_humid_x10 = 0;
static ktime_t start_time;
static int sample_interval_ms = 100;

static ssize_t sms_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    char kbuf[128];
    int len;
    uint32_t rand_val = get_random_u32();
    
    /* Chỉ cho phép đọc 1 lần mỗi syscall để tránh vòng lặp vô tận khi dùng 'cat' */
    if (*ppos > 0) return 0;

    /* Random nhiệt độ (15.0 - 45.0) và độ ẩm (30.0 - 90.0) */
    last_temp_x10 = 150 + (rand_val % 301); 
    last_humid_x10 = 300 + ((rand_val >> 16) % 601); 
    read_count++;

    len = snprintf(kbuf, sizeof(kbuf), "%llu,%d.%d,%d.%d\n",
                   ktime_to_ms(ktime_get()),
                   last_temp_x10 / 10, last_temp_x10 % 10,
                   last_humid_x10 / 10, last_humid_x10 % 10);

    if (copy_to_user(buf, kbuf, len)) return -EFAULT;
    *ppos += len;
    return len;
}

static long sms_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    int val;
    if (cmd == SMS_SET_INTERVAL) {
        if (copy_from_user(&val, (int __user *)arg, sizeof(int))) return -EFAULT;
        sample_interval_ms = val;
        pr_info("sms_sensor: interval set to %d ms\n", val);
        return 0;
    }
    return -EINVAL;
}

static const struct file_operations sms_fops = {
    .owner = THIS_MODULE,
    .read = sms_read,
    .unlocked_ioctl = sms_ioctl,
};

static struct miscdevice sms_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = SMS_DEVICE_NAME,
    .fops = &sms_fops,
};

/* --- Phần Procfs hiển thị thống kê --- */
static int sms_stats_show(struct seq_file *m, void *v) {
    s64 uptime_s = ktime_divns(ktime_sub(ktime_get(), start_time), NSEC_PER_SEC);
    seq_printf(m, "read_count: %d\n", read_count);
    seq_printf(m, "last_temperature: %d.%d\n", last_temp_x10 / 10, last_temp_x10 % 10);
    seq_printf(m, "last_humidity: %d.%d\n", last_humid_x10 / 10, last_humid_x10 % 10);
    seq_printf(m, "uptime_seconds: %lld\n", uptime_s);
    return 0;
}

static int sms_stats_open(struct inode *inode, struct file *file) {
    return single_open(file, sms_stats_show, NULL);
}

static const struct proc_ops sms_proc_ops = {
    .proc_open = sms_stats_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init sms_init(void) {
    start_time = ktime_get();
    misc_register(&sms_misc);
    proc_create("sms_stats", 0, NULL, &sms_proc_ops);
    pr_info("sms_sensor: loaded successfully\n");
    return 0;
}

static void __exit sms_exit(void) {
    remove_proc_entry("sms_stats", NULL);
    misc_deregister(&sms_misc);
    pr_info("sms_sensor: unloaded\n");
}

module_init(sms_init);
module_exit(sms_exit);

