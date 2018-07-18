#include "uart.h"
#include <pic.h> 
#include <string.h>
/*********************************************************
*         PIC24串口发送和接收    
*
* 文    件: main.c
* 作    者: ZhnJa
* 修改日期: 2013-4-6
* 说明： 9600波特率，暂时关闭发送。
************************************************************/
#define RELAY1  LATBbits.LATB14
#define RELAY2  LATBbits.LATB13
// Setup configuration bits
//_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF &  FWPSA_PR32 & WDTPS_PS2048 & ICS_PGx3)    // JTAG/Code Protect/Write Protect/Clip-on Emulation mode/ Watchdog Timer/ICD pins select
//_CONFIG2(FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_HS & FNOSC_PRI)  // Disable CLK switch and CLK monitor, OSCO or Fosc/2, HS oscillator,Primary oscillator
    
void uart_init(void);
void Delay(unsigned int t);
    
unsigned char Rev[6];//接收数组
unsigned char Send[10]={0x69,1,2,3,4,5,6,7,8,9};//发送数组
//unsigned char i=0;//发送位
unsigned char Index=0;//接收位
   /*
int main(void)
{
    TRISBbits.TRISB13 = 0;//IO方向初始化
    TRISBbits.TRISB14 = 0;
    TRISBbits.TRISB5 = 1;
    uart_init();
    while(1)
    {           
     //接收判断
        if(Rev[0]==0x68 && Rev[2]==0x69)
        {
            if(Rev[3]==0x01)
            {
                if(Rev[4] == 1)
                RELAY1 = 1;
                if(Rev[4] == 0)
                RELAY1 = 0;
            }   
            if(Rev[3]==0x02)
            {
                if(Rev[4] == 1)
                RELAY2 = 1;
                if(Rev[4] == 0)
                RELAY2 = 0;
            }   
        }
    }   
    return 0;
}*/
/*****************************************************************
** Name:    uart_init
** Function: 串口2初始化
** Input:    
** Output:   
** Time:     2013.4.05
** Attention: 11.0596MHz,9600bunds
******************************************************************/
void uart_init(void)
{
	RXPPS	= 0x17;
	RC6PPS	= 0x10;
	
	//TX1STAbits.CSRC		= 1;//Master mode (clock generated internally from BRG)
	TX1STAbits.TX9		= 0;//Selects 8-bit transmission
	TX1STAbits.SYNC		= 0;
	//TX1STAbits.SENDB	= 1;
	TX1STAbits.BRGH		= 1;
	TX1STAbits.TX9D		= 0;//Can be address/data bit or a parity bit.
	//TX1STAbits.TRMT//Transmit Shift Register Status bit
	
	
	RC1STAbits.RX9		= 0;//Selects 8-bit reception
	//RC1STAbits.SREN
	RC1STAbits.CREN		= 1;//Continuous Receive Enable bit
	RC1STAbits.ADDEN	= 0;// Address Detect Enable bit
	//RC1STAbits.FERR	//Framing Error bit
	//RC1STAbits.OERR	//Overrun Error bi
	//RC1STAbits.RX9D

	BAUD1CONbits.ABDOVF		= 0;
	//BAUD1CONbits.RCIDL		//1 = Receiver is Idle
	BAUD1CONbits.SCKP		= 0;//1 Idle state for clock (CK) is a high level
	BAUD1CONbits.BRG16		= 1;//0 = 8-bit Baud Rate Generator is used
	BAUD1CONbits.WUE		= 0;
	BAUD1CONbits.ABDEN		= 0;

	
	//TX1REG
	//RC1REG
	// SYNC 0 BRG16 0 BRGH 1    
	//Baud Rate Formula = FOSC/[16 (n+1)]

	// SYNC 0 BRG16 1 BRGH 1    
	//Baud Rate Formula = FOSC/[4 (n+1)]
	
	SP1BRGL				= 34;//16M/4/115200-1
	//SP1BRGL 			= 35;//16M/4/111111-1

	//SP1BRGL				= 8;//16M/16/115200-1
	SP1BRGH				= 0;

	RC1STAbits.SPEN		= 1;//Serial port enabled
	
	TX1STAbits.TXEN = 1;

	/*
    TRISCbits.TRISC5 = 1;//IO口方向
    TRISCbits.TRISC4 = 0;
    RPINR19bits.U2RXR = 21;//RP21
    RPOR10bits.RP20R = 5;//RP20
        
    U2MODEbits.BRGH = 1;//高速
    U2BRG = 143;    //波特率
    U2STAbits.UTXISEL0 = 0;//当最后一个字符被移出发送移位寄存器时产生中断
    U2STAbits.UTXISEL1 = 0;
    U2MODEbits.UARTEN = 1;
    SRbits.IPL = 3; //CPU优先级3
    IPC7bits.U2TXIP = 4; //发送中断优先级4
    IEC1bits.U2TXIE = 1; //允许发送中断
    IFS1bits.U2TXIF = 0; //清标志位
        
    IPC7bits.U2RXIP=5; //接收中断优先级
    IEC1bits.U2RXIE=1; //允许接收中断
    IFS1bits.U2RXIF=0; //清标志位
            
//  U2STAbits.UTXEN = 1; //使能发送，同时产生发送中断
    */
}   
    
/*****************************************************************
** Name:     _U2TXInterrupt
** Function: 串口2接收中断
** Input:    
** Output:   
** Time:     2013.4.05
** Attention: 
******************************************************************/
/*
void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void)
{
    unsigned char temp; 
    IFS1bits.U2RXIF=0; //清标志位
    temp=U2RXREG;
    if(temp == 0x68)    //判断是否为第一位
        Index = 0;
    if(Index>=sizeof(Rev)) Index=0;
    Rev[Index]=temp;
    Index++;
}//*/
    
    
/*****************************************************************
** Name:     _U2TXInterrupt
** Function: 串口2发送中断
** Input:    
** Output:   
** Time:     2013.4.05
** Attention: 
******************************************************************/
/*
void __attribute__((interrupt, no_auto_psv)) _U2TXInterrupt(void)
{
    
    IFS1bits.U2TXIF = 0; //清中断标志        
    U2TXREG = Send[i] ;  //发送数据 
    while(U2STAbits.TRMT == 0); 
    Nop();Nop();Nop();Nop();        
    i++;
    if(i==10)
    {
        i=0;    
    }       
}
    
    
void Delay(unsigned int t)
{
 unsigned int i,j;
    for(i=t;i>4;i--)
    for(j=1;j<10000;j++);
}

//*/


static unsigned char uart_recv_buf[UART_RECV_BUF_LEN];
static int beg	= 0;
static int end	= 0;
int g_uart_err	= 0;

#define UART_ERR_OK			0
#define UART_ERR_FERR		1
#define UART_ERR_OERR		2
#define UART_ERR_SORT_FERR	4

#define UART_ERR_PARAM		5

#define UART_ERR_PACKET_MAX		6
#define UART_ERR_PACKET_PROTO	7
#define UART_ERR_PACKET_DROP	8


#define UART_SOH	0x01
#define UART_EOT	0x04
#define UART_ESC	0x1B
int uart_isr()
{
    unsigned char v;
	int handle = 0;
	if (RC1STAbits.OERR)// process overrun error
	{
		RC1STAbits.CREN		= 0;
		RC1STAbits.CREN		= 1;
		g_uart_err			|= UART_ERR_OERR;
	}
	if (PIR3bits.RCIF)
	{
		if (RC1STAbits.FERR)// Framing Error
		{
			g_uart_err		|= UART_ERR_FERR;
		}
		
		if (((beg+1)&UART_RECV_BUF_MASK) == end)
		{
			g_uart_err |= UART_ERR_SORT_FERR;
            v = RC1REG;
		}
		else
		{
			uart_recv_buf[beg] = RC1REG;
			beg = (beg+1)&UART_RECV_BUF_MASK;
		}
		
		handle = 1;
	}
	return handle;
}

static inline void uart_send_byte(unsigned char value)
{
	//UART send
	while (!TX1STAbits.TRMT)
	{
	}
	TX1REG = value;
}

static inline int uart_recv_byte(unsigned char *value)
{
	int getrecv = 0;
	if (beg == end)
	{
		return 0;
	}
	else
	{
		*value  = uart_recv_buf[end];
		end = (end+1)&UART_RECV_BUF_MASK;
		return 1;
	}
}




void uart_send_bytes(unsigned char* psend,int len)
{
	while(len--)
	{
		uart_send_byte(*psend++);
	}
	
}
int uart_recv_bytes(unsigned char* precv,int buflen)
{
	int ret	= 1;
	int nr	= 0;
	do
	{
		ret = uart_recv_byte(&precv[nr]);
		nr += ret;
	}
	while ( ret && (nr < buflen) );
	
	return nr;
}

enum uart_recv_status
{
	st_idle = 0,
	st_run,
	st_finish,
};

void uart_send_packet(unsigned char* psend,int len)
{
	int i;
	unsigned char c;
	uart_send_byte(UART_SOH);
	for (i = 0; i < len; ++i)
	{
		c = *psend;
		psend++;
		if ((c == UART_SOH) || (c == UART_EOT) || (c == UART_ESC))
		{
			uart_send_byte(UART_ESC);
		}
		uart_send_byte(c);
	}
	uart_send_byte(UART_EOT);
}

int uart_recv_packet(unsigned char* precv,int buflen)
{
	int ret	= 1;
	
	static int nr	= 0;
	static int inesc = 0;
	static enum uart_recv_status status = st_idle;
	static unsigned char buffer[UART_PACKET_MAX];
	
	unsigned char value;
	
	while (1)
	{
		ret = uart_recv_byte(&value);
		if (ret <= 0)
		{
			return 0;
		}
		
		if (inesc)
		{
			if ((value == UART_SOH) || (value == UART_EOT) || (value == UART_ESC))
			{
				inesc = 0;
			}
			else
			{
				// handle err
				ret = UART_ERR_PACKET_PROTO;
				goto handle_err;
			}
		}
		else
		{
			if (value == UART_SOH)
			{
				if ((status == st_idle) || (status == st_run))
				{
					nr = 0;
					status = st_run;
					continue;
				}
				else
				{
					// not ready drop a packet
					ret = UART_ERR_PACKET_DROP;
					goto handle_err;
				}
				
			}
			else if (value == UART_EOT)
			{
				if (status == st_run)
				{
					status = st_finish;
					break;
				}
				// drop
				continue;
			}
			else if (value == UART_ESC)
			{
				inesc = 1;
				continue;
			}
		}

		if (status == st_run)
		{
			if (nr == UART_PACKET_MAX)
			{
				ret = UART_ERR_PACKET_MAX;
				goto handle_err;
			}
			buffer[nr++] = value;
		}		
		
	}
	if (nr > buflen)
	{
		return UART_ERR_PARAM;
	}
	
	memcpy(precv,buffer,nr);
	status = st_idle;
	return nr;
handle_err:
	nr		= 0;
	inesc	= 0;
	status	= st_idle;
	return ret;
}

int asc2hex(unsigned char *phex,unsigned char *pasc,int len)
{
	int i;
	unsigned char asc;
	for (i = 0; i < len; ++i,++pasc)
	{
		asc = ((*pasc)>>4)&0x0F;
		*phex++ = (asc <= 9) ? asc+'0' : asc-0x0a+'A';
		
		asc = (*pasc)&0x0F;
		*phex++ = (asc <= 9) ? asc+'0' : asc-0x0a+'A';
	}
	return len*2;
}
unsigned char  hex2asc(unsigned char *pasc,unsigned char *phex,int len)
{
	int ret;
	int i;
	unsigned char hex;
	ret = len/2;
	if (len%2)
	{
		ret++;
		hex = *phex++;
		len--;
		if ((hex >= '0') && (hex <= '9'))
		{
			*pasc =  hex - '0';
		}
		else if ((hex >= 'A') && (hex <= 'F'))
		{
			*pasc =  hex - 'A' + 0x0A;
		}
		else if ((hex >= 'a') && (hex <= 'f'))
		{
			*pasc =  hex - 'a' + 0x0A;
		}
		else
		{
		}
		pasc++;
		
	}
	while (len)
	{
		hex = *phex++;
		len--;
		if ((hex >= '0') && (hex <= '9'))
		{
			*pasc =  hex - '0';
			*pasc <<= 4;
		}
		else if ((hex >= 'A') && (hex <= 'F'))
		{
			*pasc =  hex - 'A' + 0x0A;
			*pasc <<= 4;
		}
		else if ((hex >= 'a') && (hex <= 'f'))
		{
			*pasc =  hex - 'a' + 0x0A;
			*pasc <<= 4;
		}
		else
		{
		}

		hex = *phex++;
		len--;
		if ((hex >= '0') && (hex <= '9'))
		{
			*pasc |=  hex - '0';
		}
		else if ((hex >= 'A') && (hex <= 'F'))
		{
			*pasc |=  hex - 'A' + 0x0A;
		}
		else if ((hex >= 'a') && (hex <= 'f'))
		{
			*pasc |=  hex - 'a' + 0x0A;
		}
		else
		{
		}
		pasc++;
	}
	return ret;
}
//BTMAIN
static unsigned char sBM[] = "BTMAIN";
static unsigned char sBMR[] = "BTMAIR";

#define SBM_LEN	6
#define isxdigit(c)	(('0' <= (c) && (c) <= '9') \
			 || ('a' <= (c) && (c) <= 'f') \
			 || ('A' <= (c) && (c) <= 'F'))

void uart_send_bmcmd(unsigned char* psend,int len)
{
	int ret;
	unsigned char buf[UART_PACKET_MAX];
	uart_send_bytes(sBMR, SBM_LEN);
	
	ret = asc2hex(buf,psend,len);	
	uart_send_bytes(buf, ret);
}

int uart_recv_bmcmd(unsigned char* precv,int buflen)
{
	int i = 0;
	int ret;
	static gettag = 0;
	static int nr = 0;

	unsigned char value;
	unsigned char buf[UART_PACKET_MAX];
	while (1)
	{
		if (gettag)
		{
			do
			{
				ret = uart_recv_byte(&value);
			}
			while (ret <= 0);
			
			if (isxdigit(value))
			{
				buf[i++] = value;
			}
			else
			{
				gettag = 0;
				nr = 0;
				if ( i/2 + i%2 > buflen)
				{
					return UART_ERR_PARAM;
				}
				ret = hex2asc(precv, buf, i);
				return ret;
			}
		}
		else
		{
			// recv
			ret = uart_recv_byte(&value);
			if (ret <= 0)
			{
				return ret;
			}

			if (value == sBM[nr])
			{
				nr++;
			}
			else
			{
				nr = 0;
			}
			
			if (nr == SBM_LEN)
			{
				gettag = 1;
				nr = 0;
				continue;
			}
		}
	}
	return 0;
}
