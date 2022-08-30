/*
 * plug417 print info
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
static int debug_level = 0;

int plug417serial_debug_level_set(int level)
{
	debug_level = level;
	if (level < 0)
		level = 0;
	else if (level > 99)
		level = 99;
	return debug_level;
}

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

void dump_buf(int level, const void *buf, int size)
{
	int i = 0;
	unsigned int addr = 0;
	const uint8_t *p = (const uint8_t *)buf;

	if (level > debug_level)
		return;

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

#endif

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
static void plug417_print_analog_video_page(struct plug417_serial *s)
{
	struct plug417_frame *f = &s->frame;
	struct plug417_analog_video_page *a;
	a = (struct plug417_analog_video_page *)f->query.option;

	printf("Analog video page\n");
	printf("Analog video: %s\n", a->on ? "ON" : "OFF");
	printf("Video system: ");
	switch (a->video_system) {
		case PLUG417_ANALOG_P_SYSTEM_384_288:
			printf("384x288\n");
			break;
		case PLUG417_ANALOG_N_SYSTEM_320_240:
			printf("320x240\n");
			break;
		case PLUG417_ANALOG_P_SYSTEM_360_288:
			printf("360x288\n");
			break;
		case PLUG417_ANALOG_N_SYSTEM_360_240:
			printf("360x240\n");
			break;
		default:
			printf("UNKNOWN\n");
			break;
	}
	printf("Frame rate: ");
	switch (a->frame_rate) {
		case PLUG417_ANALOG_FRAME_RATE_50_60_HZ:
			printf("50/60 HZ\n");
			break;
		case PLUG417_ANALOG_FRAME_RATE_25_30_HZ:
			printf("25/30 HZ\n");
			break;
		case PLUG417_ANALOG_FRAME_RATE_9_HZ:
			printf("9 HZ\n");
			break;
		default:
			printf("UNKNOWN\n");
			break;
	}
}

/*
 *
 */
static void plug417_print_digital_video_page(struct plug417_serial *s)
{
	struct plug417_frame *f = &s->frame;
	struct plug417_digital_video_page *d;

	d = (struct plug417_digital_video_page *)f->query.option;

	printf("Digital video page\n");

	printf("External synchronization: %s\n", d->external_sync ? "ON" : "OFF");
	printf("Digital port: ");
	switch (d->port) {
		case PLUG417_DIGITAL_PORT_OFF:
			printf("OFF\n");
			break;
		case PLUG417_DIGITAL_PORT_BT_656:
			printf("BT.656\n");
			break;
		case PLUG417_DIGITAL_PORT_CMOS:
			printf("CMOS\n");
			break;
		default:
			printf("UNKNOWN\n");
			break;
	}
	printf("Output contents: ");
	switch (d->format) {
		case PLUG417_DIGITAL_FORMAT_YUV422:
			printf("YUV422\n");
			break;
		case PLUG417_DIGITAL_FORMAT_YUV422_PARAM_LINE:
			printf("YUV422 parameter line\n");
			break;
		case PLUG417_DIGITAL_FORMAT_YUV16:
			printf("YUV16\n");
			break;
		case PLUG417_DIGITAL_FORMAT_YUV16_PARAM_LINE:
			printf("YUV16 parameter line\n");
			break;
		case PLUG417_DIGITAL_FORMAT_Y16_YUV422:
			printf("Y16 YUV422\n");
			break;
		case PLUG417_DIGITAL_FORMAT_Y16_PARAM_LINE_YUV422:
			printf("Y16 parameter line YUV422\n");
			break;
		default:
			printf("UNKNOWN\n");
			break;
	}
	printf("Interface type: ");
	switch (d->interface) {
		case PLUG417_DIGITAL_INTERFACE_CMOS16:
			printf("CMOS16\n");
			break;
		case PLUG417_DIGITAL_INTERFACE_CMOS8_MSB:
			printf("CMOS8 MSB\n");
			break;
		case PLUG417_DIGITAL_INTERFACE_CMOS8_LSB:
			printf("CMOS8 LSB\n");
			break;
		default:
			printf("UNKNOWN\n");
			break;
	}
	printf("Frame rate: ");
	switch (d->frame_rate) {
		case PLUG417_DIGITAL_FRAME_RATE_50_60_HZ:
			printf("50/60 Hz\n");
			break;
		case PLUG417_DIGITAL_FRAME_RATE_25_30_HZ:
			printf("25/30 Hz\n");
			break;
		case PLUG417_DIGITAL_FRAME_RATE_9_HZ:
			printf("9 Hz\n");
			break;
		default:
			printf("UNKNOWN\n");
			break;
	}
	printf("MIPI: %s\n", d->mipi ? "ON" : "OFF");
}

/*
 *
 */
static void plug417_print_video_page(struct plug417_serial *s)
{
	struct plug417_frame *f = &s->frame;

	switch (f->query.page) {
		case PLUG417_ANALOG_VIDEO_PAGE:
			plug417_print_analog_video_page(s);
			break;
		case PLUG417_DIGITAL_VIDEO_PAGE:
			plug417_print_digital_video_page(s);
			break;
		default:
			break;
	}
}

/*
 *
 */
int plug417_query_reply_print(struct plug417_serial *s)
{
	struct plug417_frame *f = &s->frame;

	if (s->frame_size == 0)
		return -1;

	switch (f->query.functional) {
		case PLUG417_VIDEO_PAGE:
			plug417_print_video_page(s);
			break;
		default:
			printf("Unknown query page returned %d\n", f->query.functional);
			return -1;
	}

	return 0;
}


