/*
 * PLUG417 sensor Specification of Serial Communication Protocol
 */
#ifndef _PLUG417SERIAL_H_
#define _PLUG417SERIAL_H_

#include <stdint.h>

#define PLUG417_FRAME_HEADER0		0x55
#define PLUG417_FRAME_HEADER1		0xaa
#define PLUG417_FRAME_END		0xf0

struct plug417_command {
	uint8_t functional;
	uint8_t page;
	uint8_t option;
	uint32_t command;
} __attribute__((packed));

struct plug417_handshake {
	uint8_t option;
} __attribute__((packed));

struct plug417_query {
	uint8_t functional;
	uint8_t page;
	uint8_t option[1];
} __attribute__((packed));

struct plug417_frame {
	uint8_t header[2];
	uint8_t length;
	union {
		struct plug417_command command;
		struct plug417_handshake handshake;
		struct plug417_query query;
		uint8_t raw[256];
	} __attribute__((packed));
	uint8_t cs;
	uint8_t end;
} __attribute__((packed));

#endif
