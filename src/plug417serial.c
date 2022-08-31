/*
 * PLUG417 sensor Specification of Serial Communication Protocol
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

#include "plug417serial.h"

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
		s->frame.raw[s->size - 3] = b;
		return 0;
	}

	/* Frame end */
	if (b != PLUG417_FRAME_END)
		return -1;

	s->frame.end = b;
	s->frame.raw[s->size - 3] = b;

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
			debug(PLUG417_SERIAL_DEBUG, "Received buffer %d bytes\n", s->size + 1);
			dump_buf(PLUG417_SERIAL_DEBUG, &s->frame, s->size + 1);
			s->frame_size = s->size + 1;
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

	debug(PLUG417_SERIAL_DEBUG, "Send buffer %d bytes\n", f->length + 5);
	dump_buf(PLUG417_SERIAL_DEBUG, buf, f->length + 5);
	return write(s->fd, buf, f->length + 5);
}

/*
 *
 */
static int plug417_handshake_decode(struct plug417_serial *s)
{
	if (s->frame.length != 1)
		return 0;

	/* Decode handshake */
	if (s->frame.handshake.option != 0) {
		debug(PLUG417_HANDSHAKE_DEBUG, "Handshake ERROR\n");
		return -1;
	}
	debug(PLUG417_HANDSHAKE_DEBUG, "Handshake OK\n");

	return 0;
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

	s->frame_size = 0;
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
static int plug417_request(struct plug417_serial *s, uint8_t functional,
		uint8_t page, uint8_t option, uint32_t command)
{
	if (plug417_send(s, functional, page, option, command) < 0)
		return -1;

	if (plug417_receive(s) < 0)
		return -1;

	return plug417_handshake_decode(s);

}

/*
 *
 */
int plug417_query(struct plug417_serial *s, unsigned int func, unsigned int page)
{
	if (func > PLUG417_PAGE_MAX)
		return -1;

	return plug417_request(s, func, page, 0x80, 0);
}

/*
 *
 */
int plug417_query_status(struct plug417_serial *s, struct plug417_status *st)
{

	if (plug417_query(s, PLUG417_STATUS_PAGE, 0) < 0)
		return -1;

	memcpy(st, &s->frame.status, sizeof(struct plug417_status));
	return 0;

}

/*
 *
 */
int plug417_set_pseaudo_color(struct plug417_serial *s, unsigned int color)
{
	if (color > PLUG417_COMMAND_COLOR_MAX)
		return -1;

	return plug417_request(s, PLUG417_VIDEO_PAGE, PLUG417_ANALOG_VIDEO_PAGE,
				PLUG417_OPTION_PSEUDO_COLOR, color);
}

/*
 *
 */
int plug417_set_mirror_image(struct plug417_serial *s, unsigned int mirror)
{
	if (mirror > PLUG417_COMMAND_MIRROR_MAX)
		return -1;

	return plug417_request(s, PLUG417_VIDEO_PAGE, PLUG417_ANALOG_VIDEO_PAGE,
				PLUG417_OPTION_MIRROR_IMAGE, mirror);
}

/*
 *
 */
int plug417_set_test_screen(struct plug417_serial *s, unsigned int test)
{
	if (test > PLUG417_COMMAND_TEST_SCREEN_MAX)
		return -1;

	return plug417_request(s, PLUG417_SETUP_PAGE, PLUG417_ANALOG_VIDEO_PAGE,
				PLUG417_OPTION_TEST_SCREEN_SWITCHING, test);
}

/*
 *
 */
int plug417_set_cmos_content(struct plug417_serial *s, unsigned int cmos)
{
	if (cmos > PLUG417_COMMAND_CMOS_CONTENT_MAX)
		return -1;

	return plug417_request(s, PLUG417_VIDEO_PAGE, PLUG417_DIGITAL_VIDEO_PAGE,
				PLUG417_OPTION_CMOS_CONTENT_SELECTION, cmos);
}

/*
 *
 */
int plug417_set_cmos_interface(struct plug417_serial *s, unsigned int cmos)
{
	if (cmos > PLUG417_COMMAND_CMOS_INTERFACE_MAX)
		return -1;

	return plug417_request(s, PLUG417_VIDEO_PAGE, PLUG417_DIGITAL_VIDEO_PAGE,
				PLUG417_OPTION_CMOS_INTERFACE_TYPE, cmos);
}

/*
 *
 */
int plug417_set_brightness(struct plug417_serial *s, unsigned int brightness)
{
	if (brightness > PLUG417_COMMAND_BRIGHTNESS_MAX)
		return -1;

	return plug417_request(s, PLUG417_VIDEO_PAGE, PLUG417_ALGORITHM_SETTING_PAGE,
				PLUG417_OPTION_BRIGHTNESS, brightness);
}

/*
 *
 */
int plug417_set_contrast(struct plug417_serial *s, unsigned int contrast)
{
	if (contrast > PLUG417_COMMAND_CONTRAST_MAX)
		return -1;

	return plug417_request(s, PLUG417_VIDEO_PAGE, PLUG417_ALGORITHM_SETTING_PAGE,
				PLUG417_OPTION_CONTRAST, contrast);
}

/*
 *
 */
int plug417_set_small_icon_on(struct plug417_serial *s, unsigned int num, unsigned int on)
{
	return plug417_request(s, PLUG417_APPLICATION_PAGE, PLUG417_MENU_PAGE_1,
				PLUG417_OPTION_SMALL_ICON_ON(num), on);
}

/*
 *
 */
int plug417_set_small_icon_width(struct plug417_serial *s, unsigned int num, unsigned int width)
{
	return plug417_request(s, PLUG417_APPLICATION_PAGE, PLUG417_MENU_PAGE_1,
				PLUG417_OPTION_SMALL_ICON_WIDTH(num), width);
}

/*
 *
 */
int plug417_set_small_icon_x(struct plug417_serial *s, unsigned int num, unsigned int x)
{
	return plug417_request(s, PLUG417_APPLICATION_PAGE, PLUG417_MENU_PAGE_1,
				PLUG417_OPTION_SMALL_ICON_X(num), x);
}

/*
 *
 */
int plug417_set_small_icon_y(struct plug417_serial *s, unsigned int num, unsigned int y)
{
	return plug417_request(s, PLUG417_APPLICATION_PAGE, PLUG417_MENU_PAGE_1,
				PLUG417_OPTION_SMALL_ICON_X(num), y);
}

/*
 *
 */
int plug417_set_small_icon_transparency(struct plug417_serial *s, unsigned int t)
{
	return plug417_request(s, PLUG417_APPLICATION_PAGE, PLUG417_MENU_PAGE_1,
				PLUG417_OPTION_SMALL_ICON_TRANSPARENCY, t);
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

