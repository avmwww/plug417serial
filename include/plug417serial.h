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

#define PLUG417_ANALOG_VIDEO_PAGE		0
#define PLUG417_DIGITAL_VIDEO_PAGE		1
#define PLUG417_ALGORITHM_SETTING_PAGE		2

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

#define PLUG417_COMMAND_REAL_TIME_IMAGE			0
#define PLUG417_COMMAND_CHECKER_BOARD_PATTERN		1
#define PLUG417_COMMAND_ROW_GRADIENTS_PATTERN		2
#define PLUG417_COMMAND_LINE_GRADIENTS_PATTERN		3
#define PLUG417_COMMAND_TEST_SCREEN_MAX			PLUG417_COMMAND_LINE_GRADIENTS_PATTERN

/*
 * Video page
 */

/* Analog */
#define PLUG417_OPTION_ANALOG_VIDEO_SWITCH		1
#define PLUG417_OPTION_VIDEO_SYSTEM_SWITCHING		2
#define PLUG417_OPTION_FRAME_RATE_SETTING		3
#define PLUG417_OPTION_PSEUDO_COLOR			4
#define PLUG417_OPTION_MIRROR_IMAGE			5
#define PLUG417_OPTION_EZOOM				6
#define PLUG417_OPTION_COORDINATE_X_ZOOMED AREA		7
#define PLUG417_OPTION_COORDINATE_Y_ZOOMED AREA		8
#define PLUG417_OPTION_HOTSPOT_TRACK_SWITCH		9

#define PLUG417_COMMAND_COLOR_WHITE_HOT			0
#define PLUG417_COMMAND_COLOR_FULGURITE			1
#define PLUG417_COMMAND_COLOR_IRON_RED			2
#define PLUG417_COMMAND_COLOR_HOT_IRON			3
#define PLUG417_COMMAND_COLOR_MEDICAL			4
#define PLUG417_COMMAND_COLOR_ARCTIC			5
#define PLUG417_COMMAND_COLOR_RAINBOW_1			6
#define PLUG417_COMMAND_COLOR_RAINBOW_2			7
#define PLUG417_COMMAND_COLOR_TINT			8
#define PLUG417_COMMAND_COLOR_BLACK_HOT			9
#define PLUG417_COMMAND_COLOR_MAX			PLUG417_COMMAND_COLOR_BLACK_HOT

#define PLUG417_COMMAND_MIRROR_NA			0
#define PLUG417_COMMAND_MIRROR_X			1
#define PLUG417_COMMAND_MIRROR_Y			2
#define PLUG417_COMMAND_MIRROR_XY			3
#define PLUG417_COMMAND_MIRROR_MAX			PLUG417_COMMAND_MIRROR_XY

/* Digital */
#define PLUG417_OPTION_EXTERNAL_SYNCHRONIZATION_SWITCH	1
#define PLUG417_OPTION_DIGITAL_PORT_PARALLEL_TYPE	2
#define PLUG417_OPTION_CMOS_CONTENT_SELECTION		3
#define PLUG417_OPTION_CMOS_INTERFACE_TYPE		4
#define PLUG417_OPTION_DIGITAL_FRAME_RATE_SETTING	5
#define PLUG417_OPTION_MIPI_SWITCH			6
#define PLUG417_OPTION_SCENE_COMPENSATION		7
#define PLUG417_OPTION_SHUTTER_COMPENSATION		8

#define PLUG417_COMMAND_CMOS_CONTENT_YUV422			0
#define PLUG417_COMMAND_CMOS_CONTENT_YUV422_PARM_LINE		1
#define PLUG417_COMMAND_CMOS_CONTENT_YUV16			2
#define PLUG417_COMMAND_CMOS_CONTENT_YUV16_PARM_LINE		3
#define PLUG417_COMMAND_CMOS_CONTENT_Y16_YUV422			4
#define PLUG417_COMMAND_CMOS_CONTENT_YUV16_PARM_LINE_YUV422	5
#define PLUG417_COMMAND_CMOS_CONTENT_MAX			PLUG417_COMMAND_CMOS_CONTENT_YUV16_PARM_LINE_YUV422

#define PLUG417_COMMAND_CMOS_INTERFACE_CMOS16			0
#define PLUG417_COMMAND_CMOS_INTERFACE_CMOS8_MSB		1
#define PLUG417_COMMAND_CMOS_INTERFACE_CMOS8_LSB		2
#define PLUG417_COMMAND_CMOS_INTERFACE_MAX			PLUG417_COMMAND_CMOS_INTERFACE_CMOS8_LSB

/* Algorithm */
#define PLUG417_OPTION_BRIGHTNESS			0x0a
#define PLUG417_OPTION_CONTRAST				0x0b

#define PLUG417_COMMAND_BRIGHTNESS_MAX			100
#define PLUG417_COMMAND_CONTRAST_MAX			100

struct plug417_analog_video_page {
	uint8_t on;
	uint8_t video_system;
	uint8_t frame_rate;
	uint8_t pseudo_color;
	uint8_t mirror;
	uint16_t zoom_x;
	uint16_t zoom_y;
	uint16_t hotspot_track;
	uint8_t reserved[6];
} __attribute__((packed));

#define PLUG417_ANALOG_P_SYSTEM_384_288			0
#define PLUG417_ANALOG_N_SYSTEM_320_240			1
#define PLUG417_ANALOG_P_SYSTEM_360_288			2
#define PLUG417_ANALOG_N_SYSTEM_360_240			3

#define PLUG417_ANALOG_FRAME_RATE_50_60_HZ		0
#define PLUG417_ANALOG_FRAME_RATE_25_30_HZ		1
#define PLUG417_ANALOG_FRAME_RATE_9_HZ			2

struct plug417_digital_video_page {
	uint8_t external_sync;
	uint8_t port;
	uint8_t format;
	uint8_t interface;
	uint8_t frame_rate;
	uint8_t mipi;
	uint8_t reserved[11];
} __attribute__((packed));

#define PLUG417_DIGITAL_PORT_OFF			0
#define PLUG417_DIGITAL_PORT_BT_656			1
#define PLUG417_DIGITAL_PORT_CMOS			2

#define PLUG417_DIGITAL_FORMAT_YUV422			0
#define PLUG417_DIGITAL_FORMAT_YUV422_PARAM_LINE	1
#define PLUG417_DIGITAL_FORMAT_YUV16			2
#define PLUG417_DIGITAL_FORMAT_YUV16_PARAM_LINE		3
#define PLUG417_DIGITAL_FORMAT_Y16_YUV422		4
#define PLUG417_DIGITAL_FORMAT_Y16_PARAM_LINE_YUV422	5

#define PLUG417_DIGITAL_INTERFACE_CMOS16		0
#define PLUG417_DIGITAL_INTERFACE_CMOS8_MSB		1
#define PLUG417_DIGITAL_INTERFACE_CMOS8_LSB		2

#define PLUG417_DIGITAL_FRAME_RATE_50_60_HZ		0
#define PLUG417_DIGITAL_FRAME_RATE_25_30_HZ		1
#define PLUG417_DIGITAL_FRAME_RATE_9_HZ			2


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
		uint8_t raw[258];
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
	int frame_size;
};

int plug417serial_debug_level_set(int level);

int plug417_recv(struct plug417_serial *s, const void *buf, unsigned int len);

int plug417_send(struct plug417_serial *s,
		uint8_t functional, uint8_t page, uint8_t option, uint32_t command);

struct plug417_serial *plug417_open(const char *serial);

void plug417_close(struct plug417_serial *s);

int plug417_query_status(struct plug417_serial *s, struct plug417_status *st);

int plug417_query(struct plug417_serial *s, unsigned int func, unsigned int page);

int plug417_query_reply_print(struct plug417_serial *s);

void plug417_print_status(struct plug417_serial *s, struct plug417_status *st);

int plug417_set_pseaudo_color(struct plug417_serial *s, unsigned int color);

int plug417_set_mirror_image(struct plug417_serial *s, unsigned int mirror);

int plug417_set_test_screen(struct plug417_serial *s, unsigned int test);

int plug417_set_cmos_content(struct plug417_serial *s, unsigned int cmos);

int plug417_set_cmos_interface(struct plug417_serial *s, unsigned int cmos);

int plug417_set_brightness(struct plug417_serial *s, unsigned int brightness);

int plug417_set_contrast(struct plug417_serial *s, unsigned int contrast);


#define PLUG417_SERIAL_DEBUG			10
#define PLUG417_HANDSHAKE_DEBUG			9

#ifdef DEBUG
void debug(int level, const char *fmt, ...);
void dump_buf(int level, const void *buf, int size);
int plug417serial_debug_level_set(int level);
#else
static inline void debug(int level, const char *fmt, ...) {}
static inline void dump_buf(int level, const void *buf, int size) {}
static inline int plug417serial_debug_level_set(int level) { return 0; }
#endif

#ifdef __cplusplus
}
#endif

#endif

