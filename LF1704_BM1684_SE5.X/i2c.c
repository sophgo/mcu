#include "mcc_generated_files/mcc.h"
#include "power.h"
#include "common.h"
#include "tick.h"
#include "i2c.h"

#define REG_BOARD_TYPE      0
#define REG_SW_VERSION      1
#define REG_HW_VERSION      2
#define REG_CTRL            3
#define REG_SOC_TEMP        4
#define REG_HEATER_CTRL     5
#define REG_REQUEST         6
#define REG_MASK            7

#define BOARD_TYPE_SE5      3
#define SW_VERSION          2
#define HW_VERSION          2

#define REQ_NONE            0
#define REQ_POWER_OFF       1
#define REQ_REBOOT          2
#define REQ_FACTORY_RESET   3

#define DEFAULT_SOC_TEMP    20

volatile uint8_t cmd;
volatile uint32_t cmd_time_stamp;

#define REQ_QUEUE_SIZE      8
#define REQ_QUEUE_MASK      (REQ_QUEUE_SIZE - 1)
volatile static uint8_t req_queue[REQ_QUEUE_SIZE];
volatile static int8_t req_head;
volatile static int8_t req_tail;

static inline void req_queue_init(void)
{
    req_head = req_tail = 0;
}

static inline int req_queue_is_empty(void)
{
    return req_head == req_tail;
}
static inline int req_queue_is_full(void)
{
    return ((req_head + 1) & REQ_QUEUE_MASK) == req_tail;
}

static inline void req_add(uint8_t req)
{
    di();
    if (!req_queue_is_full()) {
        req_queue[req_head] = req;
        req_head = (req_head + 1) & REQ_QUEUE_MASK;
    }
    smb_alert_set();
    ei();
}

static inline void req_ack(void)
{
    if (!req_queue_is_empty())
        req_tail = (req_tail + 1) & REQ_QUEUE_MASK;
    if (req_queue_is_empty())
        smb_alert_clr();
}

static inline uint8_t req_cur(void)
{
    uint8_t tmp;

    if (req_queue_is_empty())
        tmp = 0;
    else
        tmp = req_queue[req_tail];
    return tmp;
}

void req_init(void)
{
    smb_alert_clr();
    req_queue_init();
}

volatile static uint8_t set_idx;
volatile static uint8_t idx;
volatile static uint8_t soc_temp = DEFAULT_SOC_TEMP;

extern volatile uint8_t i2cRdData;

static inline bool i2c_dir_is_read()
{
    return (SSP1STATbits.R_nW);
}

static inline void ack(void)
{
    idx = (idx + 1) & REG_MASK;
    I2C_SendAck();
}

/* called when i2c read transaction received */
static void read_isr(void)
{
    uint8_t ret;

    switch (idx) {
    case REG_BOARD_TYPE:
        ret = BOARD_TYPE_SE5;
        break;
    case REG_SW_VERSION:
        ret = SW_VERSION;
        break;
    case REG_HW_VERSION:
        ret = HW_VERSION;
        break;
    case REG_CTRL:
        ret = 0;
        break;
    case REG_SOC_TEMP:
        ret = soc_temp;
        break;
    case REG_HEATER_CTRL:
        ret = is_heater_on;
        break;
    case REG_REQUEST:
        ret = req_cur();
        break;
    default:
        ret = 0xff;
        break;
    }

    I2C_Write(ret);
    ack();
}


void board_ctrl(void)
{
    if (!cmd)
        return;

    if (tick_get() - cmd_time_stamp < 10) {
        /* 10ms should enough for an i2c transaction finished */
        return;
    }

    switch (cmd) {
    case CMD_REBOOT:
        power_off();
        __delay_ms(1000);
        power_on();
        break;
    case CMD_POWER_OFF:
        power_off();
        break;
    default:
        break;
    }
    /* clear received command */
    cmd = 0;
}

/* called when i2c write transaction received */
static void write_isr(void)
{
    uint8_t data;

    data = i2cRdData;

    if (set_idx) {
        idx = data & REG_MASK;
        set_idx = false;
        I2C_SendAck();
        return;
    }

    switch (idx) {
    case REG_BOARD_TYPE:
    case REG_SW_VERSION:
    case REG_HW_VERSION:
        break;
    case REG_CTRL:
        cmd = data;
        cmd_time_stamp = tick;
        break;
    case REG_SOC_TEMP:
        soc_temp = data;
        break;
    case REG_HEATER_CTRL:
        if (data)
            heater_on();
        else
            heater_off();
        break;
    case REG_REQUEST:
        req_ack();
        break;
    default:
        break;
    }
    ack();
}

/* address match */
static void match_isr(void)
{
    if (!i2c_dir_is_read()) {
        set_idx = true;
    }
    I2C_SendAck();
}

void i2c_slave_init(void)
{
    I2C_Open();
    I2C_SlaveSetWriteIntHandler(read_isr);
    I2C_SlaveSetReadIntHandler(write_isr);
    I2C_SlaveSetAddrIntHandler(match_isr);
}

void i2c_slave_destroy(void)
{
    I2C_Close();
}

void req_factory_reset(void)
{
    req_add(REQ_FACTORY_RESET);
}

void req_reboot(void)
{
    req_add(REQ_REBOOT);
}

void req_power_off(void)
{
    req_add(REQ_POWER_OFF);
}
