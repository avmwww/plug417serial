/*
 * PLUG417 sensor Specification of Serial Communication Protocol
 */
#ifndef _PLUG417SERIAL_H_
#define _PLUG417SERIAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <termios.h>

#define PLUG417_FRAME_HEADER0			0x55
#define PLUG417_FRAME_HEADER1			0xaa
#define PLUG417_FRAME_END			0xf0

#define PLUG417_STATUS_PAGE			0
#define PLUG417_SETUP_PAGE			1
#define PLUG417_VIDEO_PAGE			2
#define PLUG417_APPLICATION_PAGE		3
#define PLUG417_TEMPERATURE_MEASUREMENT_PAGE	4
#define PLUG417_EXPERT_PAGE			5

/*
 * Setup page
 */
#define PLUG417_OPTION_AUTOMATIC_COMPENSATION_TIME		1
#define PLUG417_OPTION_IMAGE_FREEZING				2
#define PLUG417_OPTION_TEST_SCREEN_SWITCHING			3
#define PLUG417_OPTION_SAVE_SETTINGS				4
#define PLUG417_OPTION_RESTORE_TO_FACTORY_DEFAULT		5
#define PLUG417_OPTION_THE_RISING_OF_TEMPERATURE_CALIBRATION	7
#define PLUG417_OPTION_SHUTTER_CONTROL				8

#define PLUG417_COMMAND_NOT_FREEZING			0
#define PLUG417_COMMAND_FREEZING			1

#define PLUG417_COMMAND_REAL_TIME_IMAGE			0
#define PLUG417_COMMAND_CHECKER_BOARD_PATTERN		1
#define PLUG417_COMMAND_ROW_GRADIENTS_PATTERN		2
#define PLUG417_COMMAND_LINE_GRADIENTS_PATTERN		3

#define PLUG417_COMMAND_
#define PLUG417_COMMAND_
#define PLUG417_COMMAND_

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

#define PLUG417_OBSERVATION_TYPE	0
#define PLUG417_THERMOGRAPHY_TYPE	1

#define PLUG417_VIDEO_400_300		0
#define PLUG417_VIDEO_384_288		1
#define PLUG417_VIDEO_360_288		2
#define PLUG417_VIDEO_320_240		3
#define PLUG417_VIDEO_360_240		4
#define PLUG417_VIDEO_160_120		5

struct plug417_status {
	uint8_t functional;
	uint8_t page;
	uint8_t module_id;
	uint8_t communication_id;
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint16_t focal_spot_temperature;
	uint8_t video_system;
	uint8_t video_resolution;
	uint32_t machine_id;
	uint8_t reserved[4];
} __attribute__((packed));

struct plug417_frame {
	uint8_t header[2];
	uint8_t length;
	union {
		struct plug417_command command;
		struct plug417_handshake handshake;
		struct plug417_query query;
		struct plug417_status status;
		uint8_t raw[256];
	} __attribute__((packed));
	uint8_t cs;
	uint8_t end;
} __attribute__((packed));

struct plug417_serial {
	int fd;
	int size;
	long timeout;
	struct termios termios;
	struct plug417_frame frame;
};

int plug417_recv(struct plug417_serial *s, const void *buf, unsigned int len);

int plug417_send(struct plug417_serial *s,
		uint8_t functional, uint8_t page, uint8_t option, uint32_t command);

struct plug417_serial *plug417_open(const char *serial);

void plug417_close(struct plug417_serial *s);

int plug417_get_status(struct plug417_serial *s, struct plug417_status *st);

void plug417_print_status(struct plug417_serial *s, struct plug417_status *st);

#ifdef __cplusplus
}
#endif

#endif

