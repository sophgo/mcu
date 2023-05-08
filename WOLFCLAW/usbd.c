#include <usbd_hw.h>
#include <cdc_acm_core.h>
#include <i2c.h>
#include <common.h>
#include <console.h>

usb_dev usbd_cdc;

static void usbd_cdc_config(void)
{
    /* configure the IRC48M clocks */
    rcu_usbd_config();

    /* configure USB pull-up pin */
    gpio_usbd_config();

    /* configure interrupt priority */
    nvic_usbd_config();
}

void usbd_progress(void)
{
	uint8_t slave_addr;

	slave_addr = get_default_addr();
	usbd_cdc_config();

	usbd_init(&usbd_cdc, &cdc_desc, &cdc_class);

	usbd_connect(&usbd_cdc);

	printf("usbd configured is %d\n",USBD_CONFIGURED);
	printf("usbd cdc current status is %d\n",usbd_cdc.cur_status);

	while (USBD_CONFIGURED != usbd_cdc.cur_status) {
		if (is_sdcard_inserted())
			goto quit;
		console_poll();
	}
	printf("usbd has ready\n");

	while(!is_sdcard_inserted()) {
		if (0U == cdc_acm_check_ready(&usbd_cdc)) {
			usb_cdc_handler *cdc = (usb_cdc_handler *)usbd_cdc.class_data[CDC_COM_INTERFACE];
			int rec_data_len = cdc->data[0];

			if (rec_data_len > 0) {
				if (rec_data_len == 99) {
					printf("start burning\n");
					for (int i = 0; i < 4; ++i) {
						printf("%02x ", cdc->data[i]);
					}
					printf(" start received\n");
					printf("\n");
				} else if (rec_data_len == 100) {
					printf("finish burning\n");
					for (int i = 0; i < 4; ++i) {
						printf("%02x ", cdc->data[i]);
					}
					printf(" finish received\n");
				} else {
					printf("receive data length is %d\n", rec_data_len);
					uint8_t write_buf[rec_data_len-1];
					printf("write data is ");
					for (int h = 0; h < rec_data_len - 1; ++h) {
						write_buf[h] = cdc->data[h+2];
						printf("%02x ", write_buf[h]);
					}
					printf("\n");

					for (int i = 0; i < rec_data_len + 1; ++i) {
						printf("%02x ", cdc->data[i]);
					}
					printf(" received\n");

					unsigned char command_i2c = cdc->data[1];
					unsigned char length_i2c = rec_data_len -1;

					printf("command is %02x\n", command_i2c);
					printf("length is %u\n", length_i2c);

					printf("\n");
					i2c_smbus_write_i2c_block_data(SLAVE_I2C, slave_addr, command_i2c, length_i2c, write_buf);
				}
			}

            cdc_acm_data_receive(&usbd_cdc);
        } else
            cdc_acm_data_send(&usbd_cdc);

		console_poll();
	}

quit:
	return;
}
