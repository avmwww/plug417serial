/*
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "plug417serial.h"
#include "plug417cmd.h"
/*
 * a:b:c ...
 *
 * Example:
 *  n=1:w=10:h=22
 */
static const char *parse_elm(const char *buf, char *elm, char *val, int size)
{
	const char *p = buf;

	*val = '\0';
	while (*p != ':' && *p != '=' && *p != '\0' && size != 0) {
		*elm++ = *p++;
		size--;
	}
	*elm = '\0';

	if (*p == ':') {
		p++;
		return p;
	}
	if (*p == '=')
		p++;

	while (*p != ':' && *p != '=' && *p != '\0' && size != 0) {
		*val++ = *p++;
		size--;
	}
	*val = '\0';

	if (*p == ':' || *p == '=')
		p++;
	
	return p;
}

#define PARAM_PREFIX		"\n\t\t"
/*
 *
 */
struct plug417_sub_cmd {
	const char *cmd;
	const char *alias;
	int (*handler)(struct plug417_serial *, unsigned int);
	int (*handler_ext)(struct plug417_serial *, unsigned int, unsigned int);
	int set;
	const char *description;
};

static const struct plug417_sub_cmd plug417_cmd_digital_video[] = {
	{"extsync", "s", plug417_set_external_synchronization, NULL, -1, "External synchronization switch"
			PARAM_PREFIX"Off (0)"
			PARAM_PREFIX"Slave mode (1)"},
	{"port", "p", plug417_set_digital_port_parallel_type, NULL, -1, "Digital port parallel type"
			PARAM_PREFIX"Off (0)"
			PARAM_PREFIX"BT.656 (1)"
			PARAM_PREFIX"CMOS (2)"},
	{"cmos", "c", plug417_set_cmos_content, NULL, -1, "CMOS content selection"
			PARAM_PREFIX"YUV422 (0)"
			PARAM_PREFIX"YUV422_parameter line (1)"
			PARAM_PREFIX"YUV16 (2)"
			PARAM_PREFIX"YUV16_parameter line (3)"
			PARAM_PREFIX"Y16_YUV422 (4)"
			PARAM_PREFIX"Y16_parameter_line_ YUV422 (5)"},
	{"interface", "i", plug417_set_cmos_interface, NULL, -1, "CMOS interface type"
			PARAM_PREFIX"CMOS16 (0)"
			PARAM_PREFIX"CMOS8 MSB first (1)"
			PARAM_PREFIX"CMOS8 LSB first (2)"},
	{"rate", "r", plug417_set_digital_frame_rate, NULL, -1, "Frame rate setting"
			PARAM_PREFIX"P-system 50Hz, N-system 60Hz (0)"
			PARAM_PREFIX"P-system 25Hz, N-system 30Hz (1)"
			PARAM_PREFIX"9Hz (2)"},
	{"mipi", "m", plug417_set_mipi_on, NULL, -1, "MIPI switch"
			PARAM_PREFIX"Off (0)"
			PARAM_PREFIX"On (1)"},
	{"scene", "x", plug417_set_scene_compentation, NULL, -1, "Scene compensation"
			PARAM_PREFIX"Off (0)"
			PARAM_PREFIX"On (1)"},
	{"shutter", "z", plug417_set_shutter_compentation, NULL, -1, "Shutter compensation"
			PARAM_PREFIX"Off (0)"
			PARAM_PREFIX"On (1)"},
	{NULL},
};

static const struct plug417_sub_cmd plug417_cmd_small_icon[] = {
	{"num", "n", NULL, NULL, -1, "Small icon number"
			PARAM_PREFIX"0 or 1"},
	{"on", NULL, NULL, plug417_set_small_icon_on, 1, "Small icon on"},
	{"off", NULL, NULL, plug417_set_small_icon_on, 0, "Small icon off"},
	{"x", NULL, NULL, plug417_set_small_icon_x, -1, "Small icon coordinate X"
			PARAM_PREFIX"0~width-width"},
	{"y", NULL, NULL, plug417_set_small_icon_y, -1, "Small icon coordinate Y"
			PARAM_PREFIX"0~height-width"},
	{"width", "w", NULL, plug417_set_small_icon_width, -1, "Small icon width"},
	{"transparency", "t", plug417_set_small_icon_transparency, NULL, -1, "Small icon transparency setting"
			PARAM_PREFIX"0~4"},
	{NULL},
};


static const struct plug417_sub_cmd plug417_cmd_menu_bar[] = {
	{"on", NULL, plug417_set_menu_bar_on, NULL, 1, "Menu bar on"},
	{"off", NULL, plug417_set_menu_bar_on, NULL, 0, "Menu bar off"},
	{"location", "y", plug417_set_menu_bar_location, NULL, -1, "Menu bar location setting"
			PARAM_PREFIX"0~height-16"},
	{"transparency", "t", plug417_set_menu_bar_transparency, NULL, -1, "Menu bar transparency setting"
			PARAM_PREFIX"0~4"},
	{NULL},
};

static const struct plug417_sub_cmd plug417_cmd_layer[] = {
	{"on", NULL, plug417_set_layer_on, NULL, 1, "Layer on"},
	{"off", NULL, plug417_set_layer_on, NULL, 0, "Layer off"},
	{"transparency", "t", plug417_set_layer_transparency, NULL, -1, "Layer transparency setting"
			PARAM_PREFIX"0~8"},
	{NULL},
};

static const struct plug417_sub_cmd plug417_cmd_half_pixel_cursor[] = {
	{"on", NULL, plug417_set_half_pixel_cursor_on, NULL, 1, "Half pixel cursor on"},
	{"off", NULL, plug417_set_half_pixel_cursor_on, NULL, 0, "Half pixel cursor off"},
	{"x", NULL, plug417_set_half_pixel_x, NULL, -1, "Half pixel cursor coordinate X"
			PARAM_PREFIX"0~width"},
	{"y", NULL, plug417_set_half_pixel_y, NULL, -1, "Half pixel cursor coordinate Y"
			PARAM_PREFIX"0~height"},
	{"color", "c", plug417_set_half_pixel_color, NULL, -1, "Half pixel color setting"
			PARAM_PREFIX"RGB value (0xRRGGBB)"},
	{NULL},
};

static const struct plug417_sub_cmd plug417_cmd_hotspot_tracking[] = {
	{"on", NULL, plug417_set_hotspot_tracking_cursor_on, NULL, 1, "Cursor switch on"},
	{"off", NULL, plug417_set_hotspot_tracking_cursor_on, NULL, 0, "Cursor switch off"},
	{"upper", "u", plug417_set_hotspot_tracking_upper_limit, NULL, -1, "Hotspot tracking upper limit value"
			PARAM_PREFIX"Observation 0~65535"
			PARAM_PREFIX"Thermography -500~10000"},
	{"lower", "l", plug417_set_hotspot_tracking_lower_limit, NULL, -1, "Hotspot tracking lower limit value"
			PARAM_PREFIX"Observation 0~65535"
			PARAM_PREFIX"Thermography -500~10000"},
	{"r", NULL, plug417_set_hotspot_tracking_color_r, NULL, -1, "The hottest cursor point color R"
			PARAM_PREFIX"0~255"},
	{"g", NULL, plug417_set_hotspot_tracking_color_g, NULL, -1, "The hottest cursor point color G"
			PARAM_PREFIX"0~255"},
	{"b", NULL, plug417_set_hotspot_tracking_color_b, NULL, -1, "The hottest cursor point color B"
			PARAM_PREFIX"0~255"},
	{NULL},
};

static const struct plug417_sub_cmd plug417_cmd_temperature_measurement[] = {
	{"distance", "d", plug417_set_temperature_measurement_distance, NULL, -1, "Distance setting"
			PARAM_PREFIX"0~100"},
	{"emissivity", "e", plug417_set_temperature_measurement_emissivity, NULL, -1, "Measurement mode"
			PARAM_PREFIX"0~100"},
	{"show", "s", plug417_set_temperature_measurement_show, NULL, -1, "Temperature Show"
			PARAM_PREFIX"degree Celsius (0)"
			PARAM_PREFIX"degree Fahrenheit (1)"
			PARAM_PREFIX"degree Kelvin (2)"},
	{"calibration", "c", plug417_set_temperature_measurement_calibration, NULL, -1, "Temperature Calibration"
			PARAM_PREFIX"-32768~32767"},
	{"reset", "f", plug417_set_temperature_measurement_factory_reset, NULL, 1, "Factory reset"},
	{"reflected", "r", plug417_set_temperature_measurement_reflected_setting, NULL, -1, "Reflected setting"},
	{"save", "v", plug417_set_temperature_measurement_save_settings, NULL, -1, "Save settings"},
	{"humidity", "h", plug417_set_temperature_measurement_humidity_save_settings, NULL, -1, "Humidity save settings"},
	{"range", "g", plug417_set_temperature_measurement_range, NULL, -1, "Temperature measurement range"
			PARAM_PREFIX"-20°C~150°C  (0)"
			PARAM_PREFIX"-20°C~800°C  (1)"},
	{NULL},
};

static const struct plug417_sub_cmd plug417_cmd_area_analisys[] = {
	{"mode", "m", plug417_set_area_analisys_mode, NULL, -1, "Analysis mode, values: "
			PARAM_PREFIX"Disable (0)"
			PARAM_PREFIX"Full screen analysis (1)"
			PARAM_PREFIX"Area one (2)"
			PARAM_PREFIX"Area two (3)"
			PARAM_PREFIX"Area three (4)"},
	{"x", NULL, plug417_set_area_x, NULL, -1, "Area top left conner coordinate X"
			PARAM_PREFIX"0~383"},
	{"y", NULL, plug417_set_area_y, NULL, -1, "Area top left conner coordinate Y"
			PARAM_PREFIX"0~287"},
	{"width", "w", plug417_set_area_width, NULL, -1, "Area width W"
			PARAM_PREFIX"1~255"},
	{"height", "h", plug417_set_area_height, NULL, -1, "Area height H"
			PARAM_PREFIX"1~255"},
	{"r", NULL, plug417_set_area_color_r, NULL, -1, "The color R of rectangle"
			PARAM_PREFIX"0~255"},
	{"g", NULL, plug417_set_area_color_g, NULL, -1, "The color G of rectangle"
			PARAM_PREFIX"0~255"},
	{"b", NULL, plug417_set_area_color_b, NULL, -1, "The color B of rectangle"
			PARAM_PREFIX"0~255"},
	{"alarm", "a", plug417_set_area_high_temperature_alarm, NULL, -1, "High temperature alarm switch"
			PARAM_PREFIX"High temperature alarm off (0)"
			PARAM_PREFIX"High temperature alarm on (1)"},
	{"threshold", "t", plug417_set_area_high_temperature_alarm_threshold, NULL, -1, "High temperature alarm threshold"
			PARAM_PREFIX"Observation 0~65535"
			PARAM_PREFIX"Thermography -500~10000"},
	{NULL},
};

struct plug417_cmd {
	const char *name;
	const char *cmd;
	const struct plug417_sub_cmd *sub;
	const char *description;
};

static const struct plug417_cmd plug417_cmd[] = {
	{"Digital video", "digit", plug417_cmd_digital_video},
	{"Small icon", "icon", plug417_cmd_small_icon},
	{"Menu bar", "menu", plug417_cmd_menu_bar},
	{"Layer", "layer", plug417_cmd_layer},
	{"Half pixel", "hpcursor", plug417_cmd_half_pixel_cursor},
	{"Area Analysis", "area", plug417_cmd_area_analisys},
	{"Hotspot tracking", "hptrack", plug417_cmd_hotspot_tracking},
	{"Temperature measurement", "temp", plug417_cmd_temperature_measurement},
	{NULL},
};

/*
 *
 */
static int plug417_cmd_handler(struct plug417_serial *s, const char *name,
		const struct plug417_sub_cmd *cmd, const char *c)
{
	char parm[64];
	char val[64];
	int err = 0;
	int set[16];
	int i;
	const struct plug417_sub_cmd *sub;

	for (i = 0; i < sizeof(set) / sizeof(set[0]); i++)
		set[i] = -1;

	while (*c != '\0') {
		c = parse_elm(c, parm, val, sizeof(parm));
		debug(PLUG417_CMD_PARSE_DEBUG, "P = %s, V = %s\n", parm, val);

		sub = cmd;
		i = 0;
		while (sub->cmd) {
			if (!strcmp(parm, sub->cmd) || (sub->alias && !strcmp(parm, sub->alias))) {
				if (sub->set < 0)
					set[i] = strtol(val, NULL, 0);
				else
					set[i] = sub->set;
			}
			sub++;
			i++;
		}
	}
	debug(PLUG417_CMD_PARSE_DEBUG, "%s\n", name);

	i = 0;
	sub = cmd;
	while (sub->cmd) {
		if (set[i] >= 0) {
			if (sub->handler) {
				if ((err = sub->handler(s, set[i])) < 0)
					return err;
			} else if (sub->handler_ext) {
				if ((err = sub->handler_ext(s, set[0], set[i])) < 0)
					return err;
			}
		}
		i++;
	}
	return err;
}

/*
 *
 */
static void plug417_set_help(struct plug417_serial *s, const char *parm)
{
	const struct plug417_cmd *c = plug417_cmd;
	const struct plug417_sub_cmd *sub;
	int n = strlen(parm);

	printf("Command specified to plug417 sensor\n\n");
	while (c->name) {
		if (!n || !strcmp(parm, c->cmd) || !strcmp(parm, "cmd")) {
			printf("%s: %s\n", c->name, c->cmd);
			if (c->description)
				printf("Description: %s\n", c->description);

			if (strcmp(parm, "cmd")) {
				sub = c->sub;
				printf("Sub commands:\n");
				while (sub->cmd) {
					printf("\t%s", sub->cmd);
					if (sub->alias)
						printf(", %s", sub->alias);
					if (sub->description)
						printf("\n\t %s", sub->description);
					printf("\n");
					sub++;
				}
			}
		}
		c++;
	}
}

/*
 *
 */
int plug417_set_command(struct plug417_serial *s, const char *cmd)
{
	char parm[64];
	char val[64];
	int err;
	const struct plug417_cmd *c = plug417_cmd;

	cmd = parse_elm(cmd, parm, val, sizeof(parm));
	debug(PLUG417_CMD_PARSE_DEBUG, "P = %s, V = %s, cmd = %s\n", parm, val, cmd);

	if (!strcmp(parm, "help")) {
		plug417_set_help(s, cmd);
		return 0;
	}

	while (c->name) {
		if (!strcmp(parm, c->cmd))
			err = plug417_cmd_handler(s, c->name, c->sub, cmd);

		c++;
	}

	return err;
}


