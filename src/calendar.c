#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/posix/time.h>
#include <modem/sms.h>

LOG_MODULE_REGISTER(calendar, CONFIG_LOG_DEFAULT_LEVEL);

volatile static bool moved_this_week = false;

static int get_day_of_week(void) {
    struct timespec tp;
    struct tm ts;
    clock_gettime(CLOCK_REALTIME, &tp);

    ts = *localtime(&tp.tv_sec);

    return ts.tm_wday;
}

static void battery_charge_work_handler(struct k_work *work) {
    int ret;

    int day_of_week = get_day_of_week();
    if (day_of_week == 2)
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
