#include <common.h>
#include <project.h>
#include <gd32f4xx.h>
#include <i2c_slave.h>
#include <i2c_slave.h>
#include <dbgi2c.h>
#include <mcu.h>
#include <system.h>
#include <bmc_interface.h>

static struct i2c_slave_ctx i2c2_slave_ctx;
static struct i2c_slave_ctx i2c0_slave_ctx;

#define I2C2_SLAVE_OA2_base	0x60
#define I2C2_SLAVE_OA2M		3

#define I2C0_OA1		0x17
#define I2C0_OA2		0x68

#define DUMMY_SLAVE_OA1		0x38
#define DUMMY_SLAVE_OA2		-1
#define DUMMY_SLAVE_OA2M	0

void slave_init(void)
{
	i2c_slave_init(&i2c2_slave_ctx, (void *)I2C2, I2C2_SLAVE_OA2_base + (get_module_id() * 2), I2C2_SLAVE_OA2_base + (get_module_id() * 2) + 1);
	i2c_slave_init(&i2c0_slave_ctx, (void *)I2C0, I2C0_OA1, I2C0_OA2);
	bmc_init(&i2c2_slave_ctx);
	mcu_init(&i2c0_slave_ctx);

	i2c_slave_start(&i2c0_slave_ctx);
	i2c_slave_start(&i2c2_slave_ctx);

	nvic_irq_enable(DEBUG_I2C_IRQ, 0, 0);
	nvic_irq_enable(BM1690_I2C0_IRQ, 0, 0);
}

/* replace default isr */
void I2C2_EV_IRQHandler(void)
{
	i2c_slave_isr(&i2c2_slave_ctx);
}

void I2C2_ER_IRQHandler(void)
{
	i2c_slave_isr(&i2c2_slave_ctx);
}

/* replace default isr */
void I2C0_EV_IRQHandler(void)
{
	i2c_slave_isr(&i2c0_slave_ctx);
}

void I2C0_ER_IRQHandler(void)
{
	i2c_slave_isr(&i2c0_slave_ctx);
}

#if 0
void debug_check_vector_table(void) {
    // 1. 获取向量表偏移寄存器 VTOR 的值
    uint32_t vtor = SCB->VTOR;
    dbg_printf("VTOR Register = 0x%08X\r\n", vtor);

    // 2. 检查 I2C0 中断向量
    // I2C0_EV_IRQn = 56, 向量表索引 = 56 + 16 = 72
    uint32_t i2c0_vector_address = vtor + (I2C0_EV_IRQn + 16) * 4;
    uint32_t i2c0_handler_ptr = *((uint32_t *)i2c0_vector_address);

    dbg_printf("=== I2C0 DEBUG ===\r\n");
    dbg_printf("I2C0 Vector Addr = 0x%08X\r\n", i2c0_vector_address);
    dbg_printf("I2C0 Handler Ptr = 0x%08X\r\n", i2c0_handler_ptr);
    dbg_printf("I2C0 Expected    = 0x%08X\r\n", (uint32_t)I2C0_EV_IRQHandler);
    dbg_printf("I2C0 Offset      = 0x%X\r\n", (i2c0_handler_ptr > (uint32_t)I2C0_EV_IRQHandler) ? 
        (i2c0_handler_ptr - (uint32_t)I2C0_EV_IRQHandler) : ((uint32_t)I2C0_EV_IRQHandler - i2c0_handler_ptr));

    // 3. 检查 I2C2 中断向量
    // I2C2_EV_IRQn = 82, 向量表索引 = 82 + 16 = 98
    uint32_t i2c2_vector_address = vtor + (I2C2_EV_IRQn + 16) * 4;
    uint32_t i2c2_handler_ptr = *((uint32_t *)i2c2_vector_address);

    dbg_printf("=== I2C2 DEBUG ===\r\n");
    dbg_printf("I2C2 Vector Addr = 0x%08X\r\n", i2c2_vector_address);
    dbg_printf("I2C2 Handler Ptr = 0x%08X\r\n", i2c2_handler_ptr);
    dbg_printf("I2C2 Expected    = 0x%08X\r\n", (uint32_t)I2C2_EV_IRQHandler);
    dbg_printf("I2C2 Offset      = 0x%X\r\n", (i2c2_handler_ptr > (uint32_t)I2C2_EV_IRQHandler) ? 
        (i2c2_handler_ptr - (uint32_t)I2C2_EV_IRQHandler) : ((uint32_t)I2C2_EV_IRQHandler - i2c2_handler_ptr));
}
#endif