#include <stdio.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <modem/sms.h>
#include <adp536x.h>

LOG_MODULE_REGISTER(battery, CONFIG_LOG_DEFAULT_LEVEL);

static void battery_charge_work_handler(struct k_work *work) {
    int ret;

    uint8_t percentage;
    adp536x_fg_soc(&percentage);
    LOG_INF("Battery percentage: %d", percentage);

    if (percentage < 20) {
        ret = sms_send_text(CONFIG_SMS_SEND_PHONE_NUMBER, "BATTERY LOW");
        if (ret) { 
            LOG_ERR("Sending SMS failed with error: %d\n", ret);
        }
    }
}

K_WORK_DEFINE(battery_work, battery_charge_work_handler);

static void battery_charge_handler(struct k_timer *timer) {
    k_work_submit(&battery_work);
}

K_TIMER_DEFINE(battery_timer, battery_charge_handler, NULL);

int battery_timer_init(void) {

    k_work_submit(&battery_work);

    k_timer_start(&battery_timer, K_SECONDS(3600), K_SECONDS(3600));
    LOG_INF("Battery timer started");
    return 0;
}

SYS_INIT(battery_timer_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
