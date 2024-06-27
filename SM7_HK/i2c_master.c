#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/rcc.h>
#include <tick.h>
#include <debug.h>
#include <timer.h>
#include "i2c_master.h"

void IIC_Start(void)
{
    //IIC_SDA_OUT();
    gpio_set_output_options(IIC_SDA_PORT, GPIO_OTYPE_PP,
                                        GPIO_OSPEED_VERYHIGH, IIC_SDA);
        //debug("\noutput_options done\n");
    gpio_mode_setup(IIC_SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
                                IIC_SDA);
        //debug("\nmode done\n");
    IIC_SDA_1;
    IIC_SCL_1;
    timer_udelay(5);//DelayUs(5);
    IIC_SDA_0;
    timer_udelay(5);//DelayUs(5);
    IIC_SCL_0;
}

void IIC_Stop(void)
{
    //IIC_SDA_OUT();
    gpio_set_output_options(IIC_SDA_PORT, GPIO_OTYPE_PP,
                                        GPIO_OSPEED_VERYHIGH, IIC_SDA);
        //debug("\noutput_options done\n");
    gpio_mode_setup(IIC_SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
                                IIC_SDA);
        //debug("\nmode done\n");
    IIC_SDA_0;
    IIC_SCL_1;
    timer_udelay(5);//DelayUs(5);
    IIC_SDA_1;
}

int IIC_WaitAck(void)
{
    int errCount = 0;
    int ack = 0;
		
    //IIC_SDA_IN();
    gpio_mode_setup(IIC_SDA_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE,
                                IIC_SDA);
    IIC_SDA_1;
    timer_udelay(5);//DelayUs(5);
    IIC_SCL_1;
    timer_udelay(5);//DelayUs(5);

    while(IIC_READ_SDA())
    {
        errCount++;

        if(errCount > 250)
        {
            ack = 1;
            break;
        }
    }

    IIC_SCL_0;
    return ack;
}

void IIC_Ack(void)
{
    IIC_SDA_0;
    //IIC_SDA_OUT();
    gpio_set_output_options(IIC_SDA_PORT, GPIO_OTYPE_PP,
                                        GPIO_OSPEED_VERYHIGH, IIC_SDA);
        //debug("\noutput_options done\n");
    gpio_mode_setup(IIC_SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
                                IIC_SDA);
        //debug("\nmode done\n");
    timer_udelay(5);//DelayUs(5);
    IIC_SCL_1;
    timer_udelay(5);//DelayUs(5);
    IIC_SCL_0;
    timer_udelay(5);//DelayUs(5);
    IIC_SDA_1;        
}

void IIC_NAck(void)
{
    IIC_SDA_1;
    //IIC_SDA_OUT();
    gpio_set_output_options(IIC_SDA_PORT, GPIO_OTYPE_PP,
                                        GPIO_OSPEED_VERYHIGH, IIC_SDA);
        //debug("\noutput_options done\n");
    gpio_mode_setup(IIC_SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
                                IIC_SDA);
        //debug("\nmode done\n");
    timer_udelay(5);//DelayUs(5);
    IIC_SCL_1;
    timer_udelay(5);//DelayUs(5);
    IIC_SCL_0;
    timer_udelay(5);//DelayUs(5);
}

void IIC_WriteByte(int txd)
{
    int i;
    //IIC_SDA_OUT();
    gpio_set_output_options(IIC_SDA_PORT, GPIO_OTYPE_PP,
                                        GPIO_OSPEED_VERYHIGH, IIC_SDA);
        //debug("\noutput_options done\n");
        gpio_mode_setup(IIC_SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
                                IIC_SDA);
        //debug("\nmode done\n");
    IIC_SCL_0;

    for(i = 0; i < 8; i++)
    {
        (txd & 0x80) ? IIC_SDA_1 : IIC_SDA_0;
        txd <<= 1;

        timer_udelay(1);//DelayUs(1);
        IIC_SCL_1;
        timer_udelay(5);//DelayUs(5);
        IIC_SCL_0;
        timer_udelay(1);//DelayUs(1);
    }
    IIC_SDA_1;
}

int IIC_ReadByte(int ack)
{
    int i, rcv = 0;
    //IIC_SDA_IN();
    gpio_mode_setup(IIC_SDA_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE,
                                IIC_SDA);
    for(i = 0; i < 8; i++)
    {
        rcv <<= 1;
        IIC_SCL_1;
        timer_udelay(5);//DelayUs(5);

        if(IIC_READ_SDA())
        {
            rcv++;
        }

        IIC_SCL_0;
        timer_udelay(5);//DelayUs(5);
    }

    ack ? IIC_Ack() : IIC_NAck();

    return rcv;
}

int i2c_master_init(int i2c)
{
        
        i2c_reset(i2c);
        i2c_peripheral_disable(i2c);
        //configure ANFOFF DNF[3:0] in CR1
        i2c_enable_analog_filter(i2c);
        i2c_set_digital_filter(i2c, 0);

        i2c_set_speed(i2c, i2c_speed_sm_100k, rcc_apb1_frequency / 1000 / 1000);

        //configure No-Stretch CR1 (only relevant in slave mode)
        i2c_enable_stretching(i2c);
        //addressing mode
        i2c_set_7bit_addr_mode(i2c);
        i2c_peripheral_enable(i2c);

	return 0;
       
}

int i2c2_master_init(int i2c)
{
        gpio_set_output_options(IIC_SDA_PORT, GPIO_OTYPE_OD,
					GPIO_OSPEED_VERYHIGH, IIC_SDA);
	gpio_mode_setup(IIC_SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP,
				IIC_SDA);
        gpio_set_output_options(IIC_SCL_PORT, GPIO_OTYPE_OD,
                                        GPIO_OSPEED_VERYHIGH, IIC_SCL);
	gpio_mode_setup(IIC_SCL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP,
                                IIC_SCL);	
	return 0;
}

int i2c_master_destroy(int i2c)
{
	i2c_peripheral_disable(i2c);
	return 0;
}

int i2c_transfer7_timeout(uint32_t i2c, uint8_t addr, unsigned int timeout,
			   uint8_t *w, size_t wn,
			   uint8_t *r, size_t rn)
{
	if (i2c == I2C2) {
		timer_start(timeout * 1000);

		if (wn) {
			IIC_Start();
			//debug("\nwn start done\n");
			IIC_WriteByte(addr << 1);//(addr << 1); // 7-bit address + write bit '0'
			//debug("\nwn write addr done\n");
			if (IIC_WaitAck()) goto i2c_timeout;
			//debug("\nwn wait ack done\n");

			while (wn--) {
				IIC_WriteByte(*w++);
				//debug("\nwn write data done\n");
				if (IIC_WaitAck()) goto i2c_timeout;
				//debug("\nwn wait ack done\n");
			}
			if (!rn) {
				IIC_Stop();
				//debug("\nwn stop done\n");
			}

		}

		if (rn) {
			IIC_Start();
			//debug("\nrn start done\n");
			IIC_WriteByte((addr << 1) | 1); // 7-bit address + read bit '1'
			//debug("\nrn write addr done\n");
			if (IIC_WaitAck()) goto i2c_timeout;
                	//debug("\nrn wait ack done\n");
			while (rn--) {
				*r = IIC_ReadByte(rn != 0);
				r++;
				//debug("\nrn read data done\n");
			}
			IIC_Stop();
		}

		timer_stop();
	}

	else{
		timer_start(timeout * 1000);

		/*  waiting for busy is unnecessary. read the RM */
		if (wn) {
			i2c_set_7bit_address(i2c, addr);
			i2c_set_write_transfer_dir(i2c);
			i2c_set_bytes_to_transfer(i2c, wn);
			if (rn) {
				i2c_disable_autoend(i2c);
			} else {
				i2c_enable_autoend(i2c);
			}
			i2c_send_start(i2c);

			while (wn--) {
				bool wait = true;
				while (wait) {
					if (i2c_transmit_int_status(i2c)) {
						wait = false;
					}
					if (timer_is_timeout())
						goto i2c_timeout;
					while (i2c_nack(i2c))
						if (timer_is_timeout())
							goto i2c_timeout;
				}
				i2c_send_data(i2c, *w++);
			}
			/* not entirely sure this is really necessary.
		 	* RM implies it will stall until it can write out the later bits
		 	*/
			if (rn) {
				while (!i2c_transfer_complete(i2c))
					if (timer_is_timeout())
						goto i2c_timeout;
			}
		}

		if (rn) {
			/* Setting transfer properties */
			i2c_set_7bit_address(i2c, addr);
			i2c_set_read_transfer_dir(i2c);
			i2c_set_bytes_to_transfer(i2c, rn);
			/* start transfer */
			i2c_send_start(i2c);
			/* important to do it afterwards to do a proper repeated start! */
			i2c_enable_autoend(i2c);

			for (size_t i = 0; i < rn; i++) {
				while (i2c_received_data(i2c) == 0)
					if (timer_is_timeout())
						goto i2c_timeout;
				r[i] = i2c_get_data(i2c);
			}
		}

		/* wait stop condition */
		while (i2c_busy(i2c))
			if (timer_is_timeout())
				goto i2c_timeout;

		timer_stop();
	}

	return 0;
i2c_timeout:
	if (i2c != I2C2){
		i2c_peripheral_disable(i2c);
		asm volatile ("nop; nop; nop; nop");
		i2c_peripheral_enable(i2c);
	} else{
		IIC_Stop();
		timer_stop();
	}
	return -1;
}

int i2c_master_trans(int i2c, unsigned char addr, unsigned long timeout,
		     void *w, unsigned int wn,
		     void *r, unsigned int rn)
{
	return i2c_transfer7_timeout(i2c, addr, timeout, w, wn, r, rn);
}

int i2c_master_smbus_read_byte(int i2c, unsigned char addr,
			       unsigned long timeout,
			       unsigned char cmd, unsigned char *data)
{
	return i2c_transfer7_timeout(i2c, addr, timeout, &cmd, 1, data, 1);
}

int i2c_master_smbus_write_byte(int i2c, unsigned char addr,
				unsigned long timeout,
				unsigned char cmd, unsigned char data)
{
	unsigned char tmp[2];

	tmp[0] = cmd;
	tmp[1] = data;
	return i2c_transfer7_timeout(i2c, addr, timeout, tmp, 2, NULL, 0);
}

int i2c_master_smbus_write(int i2c, unsigned char addr,
			   unsigned long timeout, unsigned char data)
{
	return i2c_transfer7_timeout(i2c, addr, timeout, &data, 1, NULL, 0);
}

int i2c_master_smbus_read(int i2c, unsigned char addr,
			  unsigned long timeout, unsigned char *data)
{
	return i2c_transfer7_timeout(i2c, addr, timeout, NULL, 0, data, 1);
}

int i2c_master_smbus_read_word(int i2c, unsigned char addr,
			       unsigned long timeout,
			       unsigned char cmd, uint16_t *data)
{
	return i2c_transfer7_timeout(i2c, addr, timeout,
				     &cmd, 1, (uint8_t *)data, 2);
}

/*
void IIC_Start(void)
{
    //IIC_SDA_OUT();
    IIC_SDA_1;
    IIC_SCL_1;
    timer_udelay(5);//DelayUs(5);
    IIC_SDA_0;
    timer_udelay(5);//DelayUs(5);
    IIC_SCL_0;
}

void IIC_Stop(void)
{
    //IIC_SDA_OUT();
    IIC_SDA_0;
    IIC_SCL_1;
    timer_udelay(5);//DelayUs(5);
    IIC_SDA_1;
}

u8 IIC_WaitAck(void)
{
    u8 errCount = 0;
    u8 ack = 0;
		
    //IIC_SDA_IN();
    IIC_SDA_1;
    timer_udelay(5);//DelayUs(5);
    IIC_SCL_1;
    timer_udelay(5);//DelayUs(5);

    while(IIC_READ_SDA())
    {
        errCount++;

        if(errCount > 250)
        {
            ack = 1;
            break;
        }
    }

    IIC_SCL_0;
    return ack;
}

void IIC_Ack(void)
{
    IIC_SDA_0;
    //IIC_SDA_OUT();
    timer_udelay(5);//DelayUs(5);
    IIC_SCL_1;
    timer_udelay(5);//DelayUs(5);
    IIC_SCL_0;
    timer_udelay(5);//DelayUs(5);
    IIC_SDA_1;        
}

void IIC_NAck(void)
{
    IIC_SDA_1;
    //IIC_SDA_OUT();
    timer_udelay(5);//DelayUs(5);
    IIC_SCL_1;
    timer_udelay(5);//DelayUs(5);
    IIC_SCL_0;
    timer_udelay(5);//DelayUs(5);
}

void IIC_WriteByte(u8 txd)
{
    u8 i;
    //IIC_SDA_OUT();
    IIC_SCL_0;

    for(i = 0; i < 8; i++)
    {
        (txd & 0x80) ? IIC_SDA_1 : IIC_SDA_0;
        txd <<= 1;

        timer_udelay(1);//DelayUs(1);
        IIC_SCL_1;
        timer_udelay(5);//DelayUs(5);
        IIC_SCL_0;
        timer_udelay(1);//DelayUs(1);
    }
    IIC_SDA_1;
}

u8 IIC_ReadByte(u8 ack)
{
    u8 i, rcv = 0;
    //IIC_SDA_IN();
    for(i = 0; i < 8; i++)
    {
        rcv <<= 1;
        IIC_SCL_1;
        timer_udelay(5);//DelayUs(5);

        if(IIC_READ_SDA())
        {
            rcv++;
        }

        IIC_SCL_0;
        timer_udelay(5);//DelayUs(5);
    }

    ack ? IIC_Ack() : IIC_NAck();

    return rcv;
}
*/
