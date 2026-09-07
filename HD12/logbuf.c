#include <logbuf.h>
#include <gd32f4xx.h>   /* CMSIS: __get_PRIMASK/__disable_irq/__set_PRIMASK */

#define LOGBUF_SIZE 32768                /* 2 的幂，掩码取模 */
#define LOGBUF_MASK (LOGBUF_SIZE - 1)

static uint8_t buf[LOGBUF_SIZE];
static volatile uint32_t seq_head;   /* 累计写入字节数 = 下一写入位的绝对序号 */
static volatile uint32_t count;      /* 缓冲内存活字节数，<= LOGBUF_SIZE */

void logbuf_push(uint8_t c)
{
	uint32_t pm = __get_PRIMASK();
	__disable_irq();
	buf[seq_head & LOGBUF_MASK] = c;
	++seq_head;
	if (count < LOGBUF_SIZE)
		++count;                     /* 满则不增：窗口整体前移，等效覆盖最旧 */
	__set_PRIMASK(pm);
}

int logbuf_read(uint32_t *cur, uint8_t *out)
{
	int ok = 0;
	uint32_t pm = __get_PRIMASK();
	__disable_irq();
	uint32_t base = seq_head - count;            /* 最旧存活字节的绝对序号 */
	if ((int32_t)(*cur - base) < 0)              /* 落后窗口：跳过已被覆盖的旧数据 */
		*cur = base;
	if ((int32_t)(*cur - seq_head) < 0) {        /* 尚有未读 */
		*out = buf[*cur & LOGBUF_MASK];
		++(*cur);
		ok = 1;
	}
	__set_PRIMASK(pm);
	return ok;
}

uint32_t logbuf_avail(uint32_t cur)
{
	uint32_t n;
	uint32_t pm = __get_PRIMASK();
	__disable_irq();
	uint32_t base = seq_head - count;
	if ((int32_t)(cur - base) < 0)
		cur = base;
	n = seq_head - cur;
	__set_PRIMASK(pm);
	return n;
}
