#include <stdio.h>

extern int uart_send(int data);
extern int uart_recv(unsigned long timeout);

/* #define WITH_CRC32 */

#define PACKET_SEQNO_INDEX      (1)
#define PACKET_SEQNO_COMP_INDEX (2)

#define PACKET_HEADER           (3)     /* start, block, block-complement */
#define PACKET_TRAILER          (2)     /* CRC bytes */
#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE             (128)
#define PACKET_1K_SIZE          (1024)
/* count in ms */
#define PACKET_TIMEOUT          (200)

#define FILE_NAME_LENGTH (64)
#define FILE_SIZE_LENGTH (16)

/* ASCII control codes: */
#define SOH (0x01)      /* start of 128-byte data packet */
#define STX (0x02)      /* start of 1024-byte data packet */
#define EOT (0x04)      /* end of transmission */
#define ACK (0x06)      /* receive OK */
#define NAK (0x15)      /* receiver error; retry */
#define CAN (0x18)      /* two of these in succession aborts transfer */
#define CRC (0x43)      /* use in place of first NAK for CRC mode */

/* Number of consecutive receive errors before giving up: */
#define MAX_ERRORS    (5)

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

static unsigned long str_to_u32(char* str)
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

/* Returns 0 on success, 1 on corrupt packet, -1 on error (timeout): */
static int receive_packet(unsigned char *data, int *length)
{
	int i, c;
	unsigned int packet_size = PACKET_SIZE;

	*length = 0;

	c = uart_recv(PACKET_TIMEOUT);
	if (c < 0) {
		return -1;
	}

	switch(c) {
	case SOH:
		packet_size = PACKET_SIZE;
		break;
	case STX:
		packet_size = PACKET_1K_SIZE;
		break;
	case EOT:
		return 0;
	case CAN:
		c = uart_recv(PACKET_TIMEOUT);
		if (c == CAN) {
			*length = -1;
			return 0;
		}
	default:
		/* This case could be the result of corruption on the first octet
		* of the packet, but it's more likely that it's the user banging
		* on the terminal trying to abort a transfer. Technically, the
		* former case deserves a NAK, but for now we'll just treat this
		* as an abort case.
		*/
		*length = -1;
		return 0;
	}

	*data = (char)c;

	for(i = 1; i < (packet_size + PACKET_OVERHEAD); ++i) {
		c = uart_recv(PACKET_TIMEOUT);
		if (c < 0) {
			return -1;
		}
		data[i] = (char)c;
	}

	/* Just a sanity check on the sequence number/complement value.
	 * Caller should check for in-order arrival.
	 */
	if (data[PACKET_SEQNO_INDEX] != ((data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff)) {
		return 1;
	}

	if (crc16(data + PACKET_HEADER, packet_size + PACKET_TRAILER) != 0) {
		return 1;
	}
	*length = packet_size;

	return 0;
}

/* Returns the length of the file received, or 0 on error: */
unsigned long ymodem_receive(int (*save)(void *data, unsigned long len),
			     unsigned long length)
{
	unsigned char packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD];
	int packet_length, i, file_done, session_done, crc_nak;
	unsigned int packets_received, errors, first_try = 1;
	char __attribute__((unused)) file_name[FILE_NAME_LENGTH], file_size[FILE_SIZE_LENGTH], *file_ptr;
	unsigned long size = 0;

	// printf("Ymodem rcv:\n");
	file_name[0] = 0;

	for (session_done = 0, errors = 0; ; ) {
		crc_nak = 1;
		if (!first_try) {
			uart_send(CRC);
		}
		first_try = 0;
		for (packets_received = 0, file_done = 0; ; ) {
			switch (receive_packet(packet_data, &packet_length)) {

			case 0:
				errors = 0;
				switch (packet_length) {
					case -1:  /* abort */
						uart_send(ACK);
						return 0;
					case 0:   /* end of transmission */
						uart_send(ACK);
						/* Should add some sort of sanity check on the number of
						 * packets received and the advertised file length.
						 */
						file_done = 1;
						break;
					default:  /* normal packet */
					if ((packet_data[PACKET_SEQNO_INDEX] & 0xff) != (packets_received & 0xff)) {
						uart_send(NAK);
					} else {
						if (packets_received == 0) {
							/* The spec suggests that the whole data section should
							 * be zeroed, but I don't think all senders do this. If
							 * we have a NULL filename and the first few digits of
							 * the file length are zero, we'll call it empty.
							 */
							for (i = PACKET_HEADER; i < PACKET_HEADER + 4; i++) {
								if (packet_data[i] != 0) {
									break;
								}
							}
							if (i < PACKET_HEADER + 4) {  /* filename packet has data */
								for (file_ptr = (char *)packet_data + PACKET_HEADER, i = 0; *file_ptr && i < FILE_NAME_LENGTH; ) {
									file_name[i++] = *file_ptr++;
								}
								file_name[i++] = '\0';
								for (++file_ptr, i = 0; *file_ptr != ' ' && i < FILE_SIZE_LENGTH; ) {
									file_size[i++] = *file_ptr++;
								}
								file_size[i++] = '\0';
								size = str_to_u32(file_size);
								if (size > length) {
									uart_send(CAN);
									uart_send(CAN);
									printf("\nrcv buffer too small (0x%08lx vs 0x%08lx)\n", length, size);
									return 0;
								}
								uart_send(ACK);
								uart_send(crc_nak ? CRC : NAK);
								crc_nak = 0;
							} else {  /* filename packet is empty; end session */
								uart_send(ACK);
								file_done = 1;
								session_done = 1;
								break;
							}
						} else {
							if (save(packet_data + PACKET_HEADER, packet_length)) {
								uart_send(CAN);
								uart_send(CAN);
								printf("\nbuffer overflow: %ld\n", length);
								return 0;
							}
							uart_send(ACK);
						}
						++packets_received;
					}  /* sequence number ok */
				}
				break;

			default:
				if (packets_received != 0) {
					if (++errors >= MAX_ERRORS) {
						uart_send(CAN);
						uart_send(CAN);
						printf("\ntoo many errors - aborted.\n");
						return 0;
					}
				}
				uart_send(CRC);
			}
			if (file_done) {
				break;
			}
		}  /* receive packets */

		if (session_done)
			break;

	}  /* receive files */

#if 0
	printf("\n");
	if (size > 0) {
		printf("read:%s\n", file_name);
		printf("len:0x%08lx\n", size);
	}
#endif
	return size;
}
