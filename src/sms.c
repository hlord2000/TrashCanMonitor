#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <modem/sms.h>
#include <modem/nrf_modem_lib.h>
#include <modem/lte_lc.h>

LOG_MODULE_REGISTER(sms_init, CONFIG_LOG_DEFAULT_LEVEL);

static void sms_callback(struct sms_data *const data, void *context)
{
	if (data == NULL) {
		LOG_ERR("%s with NULL data\n", __func__);
		return;
	}

	if (data->type == SMS_TYPE_STATUS_REPORT) {
		LOG_INF("SMS status report received\n");
	} else {
		LOG_ERR("SMS protocol message with unknown type received\n");
	}
}

int sms_init(void) {
	int ret = 0;
	int handle = 0;

	LOG_INF("Oscar Online");

	ret = nrf_modem_lib_init();
	if (ret) {
		LOG_ERR("Modem library initialization failed, error: %d\n", ret);
		return 0;
	}

	ret = lte_lc_init_and_connect();
	if (ret) {
		LOG_ERR("Lte_lc failed to initialize and connect, err %d", ret);
		return 0;
	}

	handle = sms_register_listener(sms_callback, NULL);
	if (handle) {
		LOG_ERR("sms_register_listener returned err: %d\n", handle);
		return 0;
	}

	return 0;
}

SYS_INIT(sms_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);