#include <stdio.h>
#include <string.h>

extern int xmodem_uart_send(int data);
extern int xmodem_uart_recv(unsigned long timeout);

#define TIMEOUT          (200)

/* ASCII control codes: */
#define SOH (0x01)      /* start of 128-byte data packet */
#define STX (0x02)      /* start of 1024-byte data packet */
#define EOT (0x04)      /* end of transmission */
#define ACK (0x06)      /* receive OK */
#define NAK (0x15)      /* receiver error; retry */
#define CAN (0x18)      /* two of these in succession aborts transfer */
#define CRC (0x43)      /* use in place of first NAK for CRC mode */

#define TMO	(0xff)		/* no data */
#define UND	(0xfe)		/* undefined error */

/* Number of consecutive receive errors before giving up: */
#define MAX_ERRORS    (100)

/* http://www.ccsinfo.com/forum/viewtopic.php?t=24977 */
static unsigned short crc16(const unsigned char *buf, unsigned long count)
{
	unsigned short crc = 0;
	int i;

	while(count--) {
		crc = crc ^ *buf++ << 8;

		for (i=0; i<8; i++) {
			if (crc & 0x8000) {
				crc = crc << 1 ^ 0x1021;
			} else {
				crc = crc << 1;
			}
		}
	}
	return crc;
}

static unsigned long __attribute__((unused))
	str_to_u32(char* str)
{
	const char *s = str;
	unsigned long acc;
	int c;

	/* strip leading spaces if any */
	do {
		c = *s++;
	} while (c == ' ');

	for (acc = 0; (c >= '0') && (c <= '9'); c = *s++) {
		c -= '0';
		acc *= 10;
		acc += c;
	}
	return acc;
}

static int recv_pack(void *data)
{
	int ch = xmodem_uart_recv(TIMEOUT);
	int pack_len;

	if (ch < 0)
		return -TMO;

	switch (ch) {
		case SOH:
			pack_len = 128;
			break;
		case STX:
			pack_len = 1024;
			break;
		case EOT:
			return -EOT;
		default:
			return -UND;
	}

	int recv_len = pack_len + 4;
	uint8_t *pack = data;
	int i;

	pack[0] = ch;

	for (i = 0; i < recv_len; ++i) {
		pack[i + 1] = xmodem_uart_recv(TIMEOUT);
	}
	/* check first two bytes */
	if (pack[1] + pack[2] != 0xff)
		return -CRC;
	/* check crc16 */
	if (crc16(pack + 3, pack_len + 2) == 0)
		return pack_len;

	return -CRC;
}

long xmodem_receive(int (*save)(void *priv, void *data, unsigned long len),
		    void *priv)
{
	volatile char __attribute__((unused)) file[128];
	uint8_t pack[1024 + 8];
	uint8_t *data = pack + 3;
	int pack_len;

	/* receive data */
	long recv_len = 0;
	while (1) {

		/* first package */
		if (recv_len == 0)
			xmodem_uart_send(CRC);

		pack_len = recv_pack(pack);

		if (pack_len == -EOT) {
			xmodem_uart_send(ACK);
			return recv_len;
		}

		if (pack_len == -TMO) {
			if (recv_len == 0)
				continue;
			/* we shouldnot timeout here */
			xmodem_uart_send(CAN);
			xmodem_uart_send(CAN);
			return -1;
		} else if (pack_len == -UND || pack_len == -CRC) {
			/* i donnot know what happened */
			/* ok, let us try again */
			xmodem_uart_send(NAK);
			continue;
		}

		if (save(priv, data, pack_len)) {
			/* device error, cancel transmission */
			xmodem_uart_send(CAN);
			xmodem_uart_send(CAN);
			break;
		} else {
			xmodem_uart_send(ACK);
			recv_len += pack_len;
		}
	}
	return -1;
}
