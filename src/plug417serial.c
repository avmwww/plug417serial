/*
 * PLUG417 sensor Specification of Serial Communication Protocol
 */

#include <unistd.h>
#include <endian.h>

#include "plug417serial.h"


struct plug417_serial {
	int fd;
	int size;
	struct plug417_frame frame;
};

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
int plug417_recv(struct plug417_serial *s, uint8_t b)
{
	int err;

	if ((err = plug417_put_byte(s, b)) < 0) {
		s->size = 0;
		return -1;
	}

	if (err > 0) {
		/* Frame complete */
		s->size = 0;
	} else {
		s->size++;
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

	return write(s->fd, buf, f->length + 5);
}

