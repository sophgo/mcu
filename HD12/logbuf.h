#ifndef __LOGBUF_H__
#define __LOGBUF_H__
#include <stdint.h>

void logbuf_push(uint8_t c);
/* 从绝对游标 *cur 读下一字节（不消费缓冲），推进 *cur；返回 1 取到字节，0 无新数据 */
int logbuf_read(uint32_t *cur, uint8_t *out);
/* 游标 cur 之后仍可读的字节数（游标落后窗口时按窗口起点计） */
uint32_t logbuf_avail(uint32_t cur);
#endif
