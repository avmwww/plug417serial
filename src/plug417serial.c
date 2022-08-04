/*
 * PLUG417 sensor Specification of Serial Communication Protocol
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <endian.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

#include "plug417serial.h"

#ifdef DEBUG
static int debug_level = 99;
void debug(int level, const char *fmt, ...)
{
	va_list ap;
	char tstr[200];
	time_t t;
	struct tm tm;

	if (level > debug_level)
		return;

	t = time(NULL);
	localtime_r(&t, &tm);

	strftime(tstr, sizeof(tstr), "%m-%d-%y %H:%M:%S", &tm);

	va_start(ap, fmt);
	printf("%s: ", tstr);
	vprintf(fmt, ap);
	va_end(ap);
}

void dump_buf(const void *buf, int size)
{
	int i = 0;
	unsigned int addr = 0;
	const uint8_t *p = (const uint8_t *)buf;

	printf("Buffer:\n%04x: ", addr);

	while (size-- > 0) {
		printf(" %02x", *p++);
		if (++i == 16 && size != 0) {
			addr += 16;
			i = 0;
			printf("\n%04x: ", addr);
		}
	}
	printf("\n");
}

#else
static inline void debug(int level, const char *fmt, ...) {}
static inline void dump_buf(const void *buf, int size) {}
#endif

/*
 *
 */
static uint8_t xor_checkout(const void *buf, unsigned int len)
{
	uint8_t cs = 0;
	const uint8_t *p = (uint8_t *)buf;

	while (len > 0) {
		cs ^= *p++;
		len--;
	}
	return cs;
}

/*
 *
 */
static int plug417_put_byte(struct plug417_serial *s, uint8_t b)
{
	if (s->size < 2) {
		/* Frame header */
		const uint8_t frame_header[2] = {
			PLUG417_FRAME_HEADER0,
			PLUG417_FRAME_HEADER1
		};

		if (b != frame_header[s->size])
			return -1;

		s->frame.header[s->size] = b;
		return 0;
	}

	if (s->size == 2) {
		/* Data length */
		s->frame.length = b;
		return 0;
	}

	if (s->size < (s->frame.length + 3)) {
		/* Command segment */
		s->frame.raw[s->size - 3] = b;
		return 0;
	}

	if (s->size == (s->frame.length + 3)) {
		/* XOR checkout */
		s->frame.cs = b;
		return 0;
	}

	/* Frame end */
	if (b != PLUG417_FRAME_END)
		return -1;

	s->frame.end = b;

	if (s->frame.cs != xor_checkout(&s->frame.length, s->frame.length + 1))
		return -1;

	return 1;
}

/*
 *
 */
int plug417_recv(struct plug417_serial *s, const void *buf, unsigned int len)
{
	int err = 0;
	const uint8_t *p = (uint8_t *)buf;

	while (len--) {
		if ((err = plug417_put_byte(s, *p++)) < 0) {
			s->size = 0;
			break;
		}

		if (err > 0) {
			/* Frame complete */
			debug(0, "Received buffer %d bytes\n", s->size);
			dump_buf(&s->frame, s->size - 4);
			dump_buf(&s->frame.cs, 2);
			s->size = 0;
			return 1;
		} else {
			s->size++;
		}
	}
	return err;
}

/*
 *
 */
int plug417_send(struct plug417_serial *s,
		uint8_t functional, uint8_t page, uint8_t option, uint32_t command)
{
	uint8_t buf[64];

	struct plug417_frame *f = (struct plug417_frame *)buf;

	f->header[0] = PLUG417_FRAME_HEADER0;
	f->header[1] = PLUG417_FRAME_HEADER1;
	f->length = sizeof(struct plug417_command);
	f->command.functional = functional;
	f->command.page = page;
	f->command.option = option;
	f->command.command = htobe32(command);

	buf[f->length + 3] = xor_checkout(&buf[2], f->length + 1);
	buf[f->length + 4] = PLUG417_FRAME_END;

	debug(0, "Send buffer %d bytes\n", f->length + 5);
	dump_buf(buf, f->length + 5);
	return write(s->fd, buf, f->length + 5);
}

/*
 *
 */
int plug417_receive(struct plug417_serial *s)
{
	int n;
	char buf[256];
	struct timeval tv;
	uint64_t start, cur;

	do {
		gettimeofday(&tv, NULL);
		start = tv.tv_sec * 1000000UL + tv.tv_usec;

		n = read(s->fd, buf, sizeof(buf));
		if (n > 0) {
			if (plug417_recv(s, buf, n) > 0)
				return 0;
		}
		gettimeofday(&tv, NULL);
		cur = tv.tv_sec * 1000000UL + tv.tv_usec;
	} while ((cur - start) < s->timeout);

	return -1;
}

/*
 *
 */
int plug417_get_status(struct plug417_serial *s, struct plug417_status *st)
{
	if (plug417_send(s, 0, PLUG417_STATUS_PAGE, 0x80, 0) < 0)
		return -1;

	if (plug417_receive(s) < 0)
		return -1;

	memcpy(st, &s->frame.status, sizeof(struct plug417_status));
	return 0;

}

/*
 *
 */
void plug417_print_status(struct plug417_serial *s, struct plug417_status *st)
{
	printf("PLUG417 status:\n");
	printf("ID number of module (%02x) ", st->module_id);
	switch (st->module_id) {
		case PLUG417_OBSERVATION_TYPE:
			printf("Observation type\n");
			break;
		case PLUG417_THERMOGRAPHY_TYPE:
			printf("Thermography type\n");
			break;
		default:
			printf("Unknown type\n");
			break;
	}

	printf("Program version %02d:%02d:%02d\n", st->year, st->month, st->day);
	printf("Focal spot temperature %f(%04x)\n", be16toh(st->focal_spot_temperature) * 0.01,
			be16toh(st->focal_spot_temperature));
	printf("Video system(%02x)\n", st->video_system);
	printf("Video resolution(%02x) ", st->video_resolution);
	switch (st->video_resolution) {
		case PLUG417_VIDEO_400_300:
			printf("400x300\n");
			break;
		case PLUG417_VIDEO_384_288:
			printf("384x288\n");
			break;
		case PLUG417_VIDEO_360_288:
			printf("360x288\n");
			break;
		case PLUG417_VIDEO_320_240:
			printf("320x240\n");
			break;
		case PLUG417_VIDEO_360_240:
			printf("360x240\n");
			break;
		case PLUG417_VIDEO_160_120:
			printf("160x120\n");
			break;
		default:
			printf("Unknown\n");
			break;
	}

	printf("Machine identification code %08x\n", be32toh(st->machine_id));
}

/*
 *
 */
static int plug417_setup(struct plug417_serial *s, const char *serial)
{
	int fd;
	struct termios termios;

	fd = open(serial, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0)
		return -1;

	tcgetattr(fd, &s->termios);

	memset(&termios, 0, sizeof(struct termios));

	cfmakeraw(&termios);

	termios.c_cflag &= ~(PARENB | PARODD);
	termios.c_cflag |= CREAD | CLOCAL;

	cfsetspeed(&termios, B115200);

	termios.c_cc[VTIME] = 1;
	termios.c_cc[VMIN] = 1;

	tcflush(fd, TCIOFLUSH);

	s->fd = fd;
	return tcsetattr(fd, TCSANOW, &termios);
}

/*
 *
 */
struct plug417_serial *plug417_open(const char *serial)
{
	struct plug417_serial *s;

	s = malloc(sizeof(struct plug417_serial));
	if (!s)
		return NULL;

	if (plug417_setup(s, serial) < 0) {
		free(s);
		return NULL;
	}
	return s;
}

/*
 *
 */
void plug417_close(struct plug417_serial *s)
{
	tcsetattr(s->fd, TCSANOW, &s->termios);
	close(s->fd);
	free(s);
}

