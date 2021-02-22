#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <sys/ioctl.h>
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <uart.h>
#include <log.h>

int uart_open(struct uart_ctx *ctx, char *name)
{
	int fd;
	struct termios opt;

	fd = open(name, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0) {
		error("unable to open %s\n", name);
		return -1;
	}

	fcntl(fd, F_SETFL, 0);

	/* setup uart */

	/* get the current opt for the port */
	tcgetattr(fd, &opt);

	/* set the baud rates to 115200 */
	cfsetispeed(&opt, B115200);
	cfsetospeed(&opt, B115200);

	/* enable the receiver and set local mode */
	opt.c_cflag |= (CLOCAL | CREAD);

	/* set data bits to 8 */
	opt.c_cflag &= ~CSIZE;
	opt.c_cflag |= CS8;

	/* set no parity, one stop bit */
	opt.c_cflag &= ~PARENB;
	opt.c_cflag &= ~CSTOPB;

	/* no hardware or software flow control */
	opt.c_cflag &= ~CRTSCTS;
	opt.c_iflag &= ~(IXON | IXOFF | IXANY);

	/* set raw input, receive data as is */
	opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHONL | ISIG);
	/* set raw output, send data as is */
	opt.c_oflag &= ~OPOST;

	/* set no wait mode */
	opt.c_cc[VMIN] = 0;
	opt.c_cc[VTIME] = 0;

	/* Set the new opt for the port */
	tcsetattr(fd, TCSAFLUSH, &opt);

	ctx->fd = fd;

	return 0;
}

int uart_getc(struct uart_ctx *ctx)
{
	unsigned char ch;
	int err;

	err = read(ctx->fd, &ch, 1);

	return err == 1 ? ch : -1;
}

int uart_putc(struct uart_ctx *ctx, unsigned char ch)
{
	int err;

	err = write(ctx->fd, &ch, 1);

	return err == 1 ? ch : -1;
}

void uart_close(struct uart_ctx *ctx)
{
	close(ctx->fd);
}

