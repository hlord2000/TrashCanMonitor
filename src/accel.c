#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/sensor.h>
#include <modem/sms.h>
#include <math.h>

LOG_MODULE_REGISTER(accel, CONFIG_LOG_DEFAULT_LEVEL);

const struct device *const accel_dev = DEVICE_DT_GET_ONE(adi_adxl362);

static struct sensor_trigger accel_trigger;

extern struct k_mutex sms_mutex;

static void trigger_handler(const struct device *dev, const struct sensor_trigger *trig) {
    struct sensor_value accel[3];

    int ret;

    ret = sensor_sample_fetch(dev);
    if (ret < 0) {
        LOG_ERR("sensor_sample_fetch failed ret %d", ret);
        return;
    }

    for (int i = 0; i < 3; i++) {
        ret = sensor_channel_get(dev, SENSOR_CHAN_ACCEL_X + i, &accel[i]);
        if (ret < 0) {
            LOG_ERR("sensor_channel_get failed ret %d", ret);
            return;
        }
    }
    // Convert to polar coordinates
    float x = sensor_value_to_double(&accel[0]);
    float y = sensor_value_to_double(&accel[1]);
    float z = sensor_value_to_double(&accel[2]);

    float a = sqrt(x * x + y * y + z * z);
    float inc_x = asin(x / a);
    float inc_y = asin(y / a);
    float inc_z = atan2(y, x);

    // Conver to degrees
    inc_x = inc_x * 180 / 3.1415;
    inc_y = inc_y * 180 / 3.1415;
    inc_z = inc_z * 180 / 3.1415;

    LOG_INF("Acceleration: x=%f y=%f z=%f", x, y, z);
    LOG_INF("Polar coordinates: a=%f inc_x=%f inc_y=%f inc_z=%f", a, inc_x, inc_y, inc_z);
    if (inc_y < 0) {
        ret = sms_send_text(CONFIG_SMS_SEND_PHONE_NUMBER, "PICKED UP");
        if (ret) { 
            LOG_ERR("Sending SMS failed with error: %d\n", ret);
        }
    }
    else if (inc_y > 95) {
        // Send a ZBUS message to calendar.c
    }
}

static int accel_thread(void) {
    int ret;

    if (!device_is_ready(accel_dev)) {
        LOG_ERR("ADXL362 device is not ready");
        return -ENODEV;
    }

    accel_trigger.type = SENSOR_TRIG_MOTION;
    accel_trigger.chan = SENSOR_CHAN_ACCEL_XYZ;

    ret = sensor_trigger_set(accel_dev, &accel_trigger, trigger_handler);
    if (ret < 0) {
        LOG_ERR("sensor_trigger_set failed ret %d", ret);
        return ret;
    }

    return 0;
}

K_THREAD_DEFINE(accel_thread_id, 1024, accel_thread, NULL, NULL, NULL, 10, 0, 0);