PLUG417 general-purpose uncooled infrared thermal module serial command interface
Usage: plug417ctrl <options>
	-d --device <path>	Serial device name, default /dev/ttyACM0
	-r --command <command>	Send command to sensor, use help or help:cmd or help:<command> to usage help
	-g --get <0..5	Query page, default action if parameters not specified print sensor status
	-s --set <0..5>	Set functional classification
	-p --page <0..5>	Page
	-e --cmos_i <0..2>	Set CMOS interface type
	-f --cmos_c <0..5>	Set CMOS content
	-c --color <0..9>	Set pseudo color
	-m --mirror <0..3>	Set/Reset image mirroring
	-t --test <0..3>	Set test screen
	-r --command <command>	Type help for extended help usage for this switch
	-v --verbose <0..99>	Print verbose debug information
	-h --help	Usage help

Extended help:
plug417ctrl --command help
Command specified to plug417 sensor

Analog video: analog
Sub commands:
	on
	 Analog video on
	off
	 Analog video on
	system, s
	 Video system
		PAL  384x288 (0)
		NTSC 320x240 (1)
		PAL  360x288 (2)
		NTSC 360x240 (3)
	fps, f
	 Frame rate
		PAL 50Hz, NTSC 60Hz (0)
		PAL 25Hz, NTSC 30Hz (1)
		9Hz
	color, c
	 Pseaudo color
		White hot (0)
		Fulgurite (1)
		Iron Red  (2)
		Hot Iron  (3)
		Medical   (4)
		Arctic    (5)
		Rainbow 1 (6)
		Rainbow 2 (7)
		Tint      (8)
		Black hot (9)
	mirror, m
	 Mirror image
		N/A (0)
		in X direction (1)
		in Y direction (2)
		n X and Y directions (3)
	zoom, z
	 EZOOM
		8 ~ 64 (the effective value range 1 to 8) 
	zoom_x, x
	 Coordinate X of the center of zoomed area
		0 ~ width-1
	zoom_y, y
	 Coordinate Y of the center of zoomed area
		0 ~ height-1
	track, t
	 Hotspot track
		Off (0)
		On  (1)
Digital video: digit
Sub commands:
	extsync, s
	 External synchronization switch
		Off (0)
		Slave mode (1)
	port, p
	 Digital port parallel type
		Off (0)
		BT.656 (1)
		CMOS (2)
	cmos, c
	 CMOS content selection
		YUV422 (0)
		YUV422_parameter line (1)
		YUV16 (2)
		YUV16_parameter line (3)
		Y16_YUV422 (4)
		Y16_parameter_line_ YUV422 (5)
	interface, i
	 CMOS interface type
		CMOS16 (0)
		CMOS8 MSB first (1)
		CMOS8 LSB first (2)
	rate, r
	 Frame rate setting
		P-system 50Hz, N-system 60Hz (0)
		P-system 25Hz, N-system 30Hz (1)
		9Hz (2)
	mipi, m
	 MIPI switch
		Off (0)
		On (1)
	scene, x
	 Scene compensation
		Off (0)
		On (1)
	shutter, z
	 Shutter compensation
		Off (0)
		On (1)
Small icon: icon
Sub commands:
	num, n
	 Small icon number
		0 or 1
	on
	 Small icon on
	off
	 Small icon off
	x
	 Small icon coordinate X
		0~width-width
	y
	 Small icon coordinate Y
		0~height-width
	width, w
	 Small icon width
	transparency, t
	 Small icon transparency setting
		0~4
Menu bar: menu
Sub commands:
	on
	 Menu bar on
	off
	 Menu bar off
	location, y
	 Menu bar location setting
		0~height-16
	transparency, t
	 Menu bar transparency setting
		0~4
Layer: layer
Sub commands:
	on
	 Layer on
	off
	 Layer off
	transparency, t
	 Layer transparency setting
		0~8
Half pixel: hpcursor
Sub commands:
	on
	 Half pixel cursor on
	off
	 Half pixel cursor off
	x
	 Half pixel cursor coordinate X
		0~width
	y
	 Half pixel cursor coordinate Y
		0~height
	color, c
	 Half pixel color setting
		RGB value (0xRRGGBB)
Area Analysis: area
Sub commands:
	mode, m
	 Analysis mode, values: 
		Disable (0)
		Full screen analysis (1)
		Area one (2)
		Area two (3)
		Area three (4)
	x
	 Area top left conner coordinate X
		0~383
	y
	 Area top left conner coordinate Y
		0~287
	width, w
	 Area width W
		1~255
	height, h
	 Area height H
		1~255
	r
	 The color R of rectangle
		0~255
	g
	 The color G of rectangle
		0~255
	b
	 The color B of rectangle
		0~255
	alarm, a
	 High temperature alarm switch
		High temperature alarm off (0)
		High temperature alarm on (1)
	threshold, t
	 High temperature alarm threshold
		Observation 0~65535
		Thermography -500~10000
Hotspot tracking: hptrack
Sub commands:
	on
	 Cursor switch on
	off
	 Cursor switch off
	upper, u
	 Hotspot tracking upper limit value
		Observation 0~65535
		Thermography -500~10000
	lower, l
	 Hotspot tracking lower limit value
		Observation 0~65535
		Thermography -500~10000
	r
	 The hottest cursor point color R
		0~255
	g
	 The hottest cursor point color G
		0~255
	b
	 The hottest cursor point color B
		0~255
Temperature measurement: temp
Sub commands:
	distance, d
	 Distance setting
		0~100
	emissivity, e
	 Measurement mode
		0~100
	show, s
	 Temperature Show
		degree Celsius (0)
		degree Fahrenheit (1)
		degree Kelvin (2)
	calibration, c
	 Temperature Calibration
		-32768~32767
	reset, f
	 Factory reset
	reflected, r
	 Reflected setting
	save, v
	 Save settings
	humidity, h
	 Humidity save settings
	range, g
	 Temperature measurement range
		-20°C~150°C  (0)
		-20°C~800°C  (1)
