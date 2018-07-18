#ifndef _UART_H_
#define _UART_H_

//UART_RECV_BUF_LEN must be 2^n
#define UART_RECV_BUF_LEN	32
#define UART_RECV_BUF_MASK	(UART_RECV_BUF_LEN-1)

#define UART_PACKET_MAX		32

void uart_init(void);
int uart_isr();
void uart_send_bytes(unsigned char* psend,int len);
int uart_recv_bytes(unsigned char* precv,int buflen);
void uart_send_packet(unsigned char* psend,int len);
int uart_recv_packet(unsigned char* precv,int buflen);

void uart_send_bmcmd(unsigned char* psend,int len);
int uart_recv_bmcmd(unsigned char* precv,int buflen);


#endif
