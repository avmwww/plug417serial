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

static const char *plug417_pseudo_color[] = {
	"White hot",
	"Fulgurite",
	"Iron Red",
	"Hot Iron",
	"Medical",
	"Arctic",
	"Rainbow 1",
	"Rainbow 2",
	"Tint",
	"Black hot",
};

static const char *plug417_frame_rate[] = {
	"50/60 Hz",
	"25/30 Hz",
	"9 Hz",
};

static const char *plug417_video_system[] = {
	"384x288",
	"320x240",
	"360x288",
	"360x240",
};

static const char *plug417_mirror[] = {
	"No",
	"in X direction",
	"in Y direction",
	"in X and Y direction",
};

static const char *plug417_digital_port[] ={
	"OFF",
	"BT.656",
	"CMOS",
};

static const char *plug417_digital_format[] ={
	"YUV422",
	"YUV422 parameter line",
	"YUV16",
	"YUV16 parameter line",
	"Y16 YUV422",
	"Y16 parameter line YUV422",
};

static const char *plug417_digital_interface[] = {
	"CMOS16",
	"CMOS8 MSB",
	"CMOS8 LSB",
};

static const char *plug417_alorithm_dimming_mode[] ={
	"Linear",
	"Platform",
	"Hybrid",
};

static const char *plug417_alorithm_ide_log[] = {
	"IDE",
	"LOG"
};

static const char *plug417_temperature_mode[] = {
	"Minimum + maximum temperature of current analysis object",
	"Cross cursor spot+ maximum temperature",
	"minimum + Cross cursor spot temperature",
};

static const char *plug417_temperature_unit[] = {
	"°C",
	"°F",
	"°K",
};

static const char *plug417_analisys_area[] = {
	"Disable",
	"Full screen",
	"Area 1",
	"Area 2",
	"Area 3",
};


/*
 *
 */
static void plug417_print_member(const char *fmt, unsigned int member,
		unsigned int max, const char **tab)
{
	printf("%s: %s\n", fmt, member > max ?
			"UNKNOWN" : tab[member]);
}

/*
 *
 */
static void plug417_print_member_on_off(const char *fmt, unsigned int member)
{
	printf("%s: %s\n", fmt, member ? "ON" : "OFF");
}

/*
 *
 */
static void plug417_print_digit(const char *fmt, unsigned int member)
{
	printf("%s: %d\n", fmt, member);
}

/*
 *
 */
static void plug417_print_digit16(const char *fmt, unsigned int member)
{
	printf("%s: %d\n", fmt, be16toh(member));
}

/*
 *
 */
static void plug417_print_digit24(const char *fmt, unsigned int member)
{
	printf("%s: %d\n", fmt, be32toh(member) >> 8);
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

	plug417_print_member_on_off("Analog video", a->on);

	plug417_print_member("Video system",
			a->video_system, PLUG417_ANALOG_VIDEO_SYSTEM_MAX,
			plug417_video_system);

	plug417_print_member("Frame rate",
			a->frame_rate, PLUG417_ANALOG_FRAME_RATE_MAX,
			plug417_frame_rate);

	plug417_print_member("Pseudo collor",
			a->pseudo_color, PLUG417_COMMAND_COLOR_MAX,
			plug417_pseudo_color);

	plug417_print_member("Mirror image",
			a->mirror, PLUG417_ANALOG_MIRROR_MAX,
			plug417_mirror);

	plug417_print_digit("EZOOM zoom factor", a->ezoom);

	plug417_print_digit("Coordinate X the center of zoomed area", be16toh(a->zoom_x));
	plug417_print_digit("Coordinate Y the center of zoomed area", be16toh(a->zoom_y));
	plug417_print_member_on_off("Hotspot track", a->hotspot_track);
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

	plug417_print_member_on_off("External synchronization",
			d->external_sync);

	plug417_print_member("Digital port", d->port, PLUG417_DIGITAL_PORT_MAX,
			plug417_digital_port);

	plug417_print_member("Output contents", d->format,
			PLUG417_DIGITAL_FORMAT_MAX, plug417_digital_format);

	plug417_print_member("Interface type", d->interface,
			PLUG417_DIGITAL_INTERFACE_MAX, plug417_digital_interface);

	plug417_print_member("Frame rate", d->frame_rate,
			PLUG417_DIGITAL_FRAME_RATE_MAX, plug417_frame_rate);

	plug417_print_member_on_off("MIPI", d->mipi);
}

/*
 *
 */
static void plug417_print_algorithm_control_page_1(struct plug417_serial *s)
{
	struct plug417_frame *f = &s->frame;
	struct plug417_alorithm_control_page_1 *a;

	a = (struct plug417_alorithm_control_page_1 *)f->query.option;

	printf("Algorithm control page 1\n");

	plug417_print_member_on_off("Time-domain filtering", a->time_domain_filering);
	plug417_print_digit("Filtering strength", a->filtering_strength);
	plug417_print_member_on_off("Vertical strip removal", a->vertical_strip_removal);
	plug417_print_digit("Vertical strip strength", a->vertical_strip_strength);
	plug417_print_member_on_off("Sharpening", a->sharpening);
	plug417_print_digit("Sharpening strength", a->sharpening_strength);
	plug417_print_member("Dimming mode", a->dimming_mode,
			PLUG417_ALGORITHM_DIMMING_MODE_MAX, plug417_alorithm_dimming_mode);
	plug417_print_digit("Proportion of upper throwing point", a->proportion_upper_throwing_point);
	plug417_print_digit("Proportion of lower throwing point", a->proportion_lower_throwing_point);
	plug417_print_digit("Brightness", a->brightness);
	plug417_print_digit("Contrast", a->contrast);
	plug417_print_digit("Hybrid dimming mapping", a->hybrid_dimming_mapping);
}

/*
 *
 */
static void plug417_print_algorithm_control_page_2(struct plug417_serial *s)
{
	struct plug417_frame *f = &s->frame;
	struct plug417_alorithm_control_page_2 *a;

	a = (struct plug417_alorithm_control_page_2 *)f->query.option;
	printf("Algorithm control page 2\n");

	plug417_print_member_on_off("Y8 correction", a->y8_correction);
	plug417_print_member("IDE/LOG", a->ide_log, 1, plug417_alorithm_ide_log);
	plug417_print_member_on_off("IDE enhancement", a->ide_enhancement);
	plug417_print_digit("IDE filtering level", a->ide_filtering_level);
	plug417_print_digit("IDE detail gain", a->ide_detail_gain);
	plug417_print_member_on_off("LOG enhancement", a->log_enhancement);
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
		case PLUG417_ALGORITHM_SETTING_PAGE:
			plug417_print_algorithm_control_page_1(s);
			break;
		case PLUG417_ALGORITHM_SETTING_PAGE + 1:
			plug417_print_algorithm_control_page_2(s);
			break;
		default:
			printf("Unknown video page %d\n", f->query.page);
			break;
	}
}

/*
 *
 */
static void plug417_print_menu_function_page_1(struct plug417_serial *s)
{
	struct plug417_frame *f = &s->frame;
	struct plug417_menu_function_page_1 *m;
	int i;

	m = (struct plug417_menu_function_page_1 *)f->query.option;
	printf("Menu function page 1\n");
	for (i =0 ; i < 2; i++) {
		printf("Icon: %d\n", i);
		plug417_print_member_on_off("\tdisplay", m->small_icon[i].display);
		plug417_print_digit("\twidth", m->small_icon[i].width);
		plug417_print_digit16("\tlocation setting X", m->small_icon[i].x);
		plug417_print_digit16("\tlocation setting Y", m->small_icon[i].y);
	}
	plug417_print_digit("Small icon transparency setting", m->small_icon_transparency);
}

/*
 *
 */
static void plug417_print_menu_function_page_2(struct plug417_serial *s)
{
	struct plug417_frame *f = &s->frame;
	struct plug417_menu_function_page_2 *m;

	m = (struct plug417_menu_function_page_2 *)f->query.option;
	printf("Menu function page 2\n");
	plug417_print_member_on_off("Menu bar display", m->menu_bar_display);
	plug417_print_digit16("Menu bar location setting", m->menu_bar_location);
	plug417_print_digit("Menu bar transparency level", m->menu_bar_transparency_level);
	plug417_print_member_on_off("Layer display", m->layer_display);
	plug417_print_digit("Layer transparency setting", m->layer_transparency);
	plug417_print_member_on_off("Half pixel cursor", m->half_pixel_cursor);
	plug417_print_digit16("Half pixel cursor location setting X", m->half_pixel_cursor_lacation_x);
	plug417_print_digit16("Half pixel cursor location setting Y", m->half_pixel_cursor_lacation_y);
	plug417_print_digit24("Half pixel color label", m->half_pixel_color_label.value);
}

/*
 *
 */
static void plug417_print_area_analysis_page(struct plug417_serial *s)
{
	struct plug417_frame *f = &s->frame;
	struct plug417_area_analysis_page *a;

	a = (struct plug417_area_analysis_page *)f->query.option;
	printf("Area analysis page\n");
	plug417_print_member("Analysis", a->analysis, 4, plug417_analisys_area);
	plug417_print_digit16("Starting coordinate X", a->x);
	plug417_print_digit16("Starting coordinate Y", a->y);
	plug417_print_digit16("Area width", a->width);
	plug417_print_digit16("Area height", a->height);
	plug417_print_digit("Color Component R", a->r);
	plug417_print_digit("Color Component G", a->g);
	plug417_print_digit("Color Component B", a->b);
	plug417_print_member_on_off("High temperature alarm",
			a->high_temperature_alarm);
	plug417_print_digit16("High temperature alarm threshold",
			a->high_temperature_alarm_threshold);
	plug417_print_member_on_off("Temperature exceeds alarm threshold",
			a->temperature_exceeds_alarm_threshold);
	plug417_print_digit16("Coldest point X coordinate", a->coldest_x);
	plug417_print_digit16("Coldest point Y coordinate", a->coldest_y);
	plug417_print_digit16("Coldest point temperature measurement / observation Y16",
			a->coldest_temperature_y16);
	plug417_print_digit16("Hottest X coordinate", a->hottest_x);
	plug417_print_digit16("Hottest Y coordinate", a->hottest_y);
	plug417_print_digit16("Hottest temperature measurement / observation Y16",
			a->hottest_temperature_y16);
	plug417_print_digit16("Cursor X coordinate", a->cursor_x);
	plug417_print_digit16("Cursor Y coordinate", a->cursor_y);
	plug417_print_digit16("Cursor temperature measurement / observation Y16",
			a->cursor_temperature_y16);
	plug417_print_digit16("Regional average temperature measurement / observation Y16",
			a->regional_temperature_y16);
}

/*
 *
 */
static void plug417_print_hotspot_tracking_page(struct plug417_serial *s)
{
	struct plug417_frame *f = &s->frame;
	struct plug417_hotspot_tracking_page *h;

	h = (struct plug417_hotspot_tracking_page *)f->query.option;
	printf("Hotspot tracking page\n");
	plug417_print_member_on_off("Hottest cursor", h->cursor & 1);
	plug417_print_member_on_off("Coldest cursor", h->cursor & 2);
	plug417_print_digit16("Hotspot tracking upper limit", h->upper_limit);
	plug417_print_digit16("Hotspot tracking lower limit", h->lower_limit);
	plug417_print_digit("Hottest cursor color component R", h->r);
	plug417_print_digit("Hottest cursor color component G", h->g);
	plug417_print_digit("Hottest cursor color component B", h->b);
}

/*
 *
 */
static void plug417_print_application_page(struct plug417_serial *s)
{
	struct plug417_frame *f = &s->frame;

	switch (f->query.page) {
		case PLUG417_MENU_PAGE_1:
			plug417_print_menu_function_page_1(s);
			break;
		case PLUG417_MENU_PAGE_2:
			plug417_print_menu_function_page_2(s);
			break;
		case PLUG417_AREA_ANALYSIS_PAGE:
			plug417_print_area_analysis_page(s);
			break;
		case PLUG417_HOTSPOT_TRACKING_PAGE:
			plug417_print_hotspot_tracking_page(s);
			break;
		default:
			printf("Unknown menu function page %d\n", f->query.page);
			break;
	}
}

/*
 *
 */
static void plug417_print_measurement_page_1(struct plug417_serial *s)
{
	struct plug417_frame *f = &s->frame;
	struct plug417_measurement_page_1 *m;

	m = (struct plug417_measurement_page_1 *)f->query.option;
	printf("Temperature measurement page 1\n");
	plug417_print_digit("The value of distance setting", m->distance);
	plug417_print_digit("The value of emissivity setting", m->emissivity);
	plug417_print_member("Temperature mode", m->temperature_mode, 2, plug417_temperature_mode);
	plug417_print_member("Temperature unit", m->temperature_unit, 2, plug417_temperature_unit);
	plug417_print_digit16("Min Corresponding Coordinate X", m->min_x);
	plug417_print_digit16("Min Corresponding Coordinate Y", m->min_y);
	plug417_print_digit16("Min Corresponding temperature after calibration", m->min_temperature_calibrated);
	plug417_print_digit16("Max Corresponding Coordinate X", m->max_x);
	plug417_print_digit16("Max Corresponding Coordinate Y", m->max_y);
	plug417_print_digit16("Max Corresponding temperature after calibration", m->max_temperature_calibrated);
	plug417_print_digit16("Reflected temp", m->temperature_reflected);
	plug417_print_digit("Humidity value", m->humidity);
	plug417_print_digit("Temperature measurement range", m->temperature_range);
}

/*
 *
 */
static void plug417_print_measurement_page(struct plug417_serial *s)
{
	struct plug417_frame *f = &s->frame;

	switch (f->query.page) {
		case 0:
			plug417_print_measurement_page_1(s);
			break;
		case 1:
			break;
		default:
			printf("Unknown measurement page %d\n", f->query.page);
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
		case PLUG417_TEMPERATURE_MEASUREMENT_PAGE:
			plug417_print_measurement_page(s);
			break;
		case PLUG417_APPLICATION_PAGE:
			plug417_print_application_page(s);
			break;
		default:
			printf("Unknown query page returned %d\n", f->query.functional);
			return -1;
	}

	return 0;
}


