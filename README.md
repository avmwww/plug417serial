PLUG417 general-purpose uncooled infrared thermal module serial command interface  
Usage: plug417ctrl &lt;options&gt;  
&emsp;-d --device &lt;path&gt;	Serial device name, default /dev/ttyACM0  
&emsp;-r --command &lt;command&gt;	Send command to sensor, use help or help:cmd or help:&lt;command&gt; to usage help  
&emsp;-g --get &lt;0..5	Query page, default action if parameters not specified print sensor status  
&emsp;-s --set &lt;0..5&gt;	Set functional classification  
&emsp;-p --page &lt;0..5&gt;	Page  
&emsp;-e --cmos_i &lt;0..2&gt;	Set CMOS interface type  
&emsp;-f --cmos_c &lt;0..5&gt;	Set CMOS content  
&emsp;-c --color &lt;0..9&gt;	Set pseudo color  
&emsp;-m --mirror &lt;0..3&gt;	Set/Reset image mirroring  
&emsp;-t --test &lt;0..3&gt;	Set test screen  
&emsp;-r --command &lt;command&gt;	Type help for extended help usage for this switch  
&emsp;-v --verbose &lt;0..99&gt;	Print verbose debug information  
&emsp;-h --help	Usage help  
  
Extended help:  
plug417ctrl --command help  
Command specified to plug417 sensor  
  
Analog video: analog  
Sub commands:  
&emsp;on  
&emsp; Analog video on  
&emsp;off  
&emsp; Analog video on  
&emsp;system, s  
&emsp; Video system  
&emsp;&emsp;PAL  384x288 (0)  
&emsp;&emsp;NTSC 320x240 (1)  
&emsp;&emsp;PAL  360x288 (2)  
&emsp;&emsp;NTSC 360x240 (3)  
&emsp;fps, f  
&emsp; Frame rate  
&emsp;&emsp;PAL 50Hz, NTSC 60Hz (0)  
&emsp;&emsp;PAL 25Hz, NTSC 30Hz (1)  
&emsp;&emsp;9Hz  
&emsp;color, c  
&emsp; Pseaudo color  
&emsp;&emsp;White hot (0)  
&emsp;&emsp;Fulgurite (1)  
&emsp;&emsp;Iron Red  (2)  
&emsp;&emsp;Hot Iron  (3)  
&emsp;&emsp;Medical   (4)  
&emsp;&emsp;Arctic    (5)  
&emsp;&emsp;Rainbow 1 (6)  
&emsp;&emsp;Rainbow 2 (7)  
&emsp;&emsp;Tint      (8)  
&emsp;&emsp;Black hot (9)  
&emsp;mirror, m  
&emsp; Mirror image  
&emsp;&emsp;N/A (0)  
&emsp;&emsp;in X direction (1)  
&emsp;&emsp;in Y direction (2)  
&emsp;&emsp;n X and Y directions (3)  
&emsp;zoom, z  
&emsp; EZOOM  
&emsp;&emsp;8 ~ 64 (the effective value range 1 to 8)  
&emsp;zoom_x, x  
&emsp; Coordinate X of the center of zoomed area  
&emsp;&emsp;0 ~ width-1  
&emsp;zoom_y, y  
&emsp; Coordinate Y of the center of zoomed area  
&emsp;&emsp;0 ~ height-1  
&emsp;track, t  
&emsp; Hotspot track  
&emsp;&emsp;Off (0)  
&emsp;&emsp;On  (1)  
Digital video: digit  
Sub commands:  
&emsp;extsync, s  
&emsp; External synchronization switch  
&emsp;&emsp;Off (0)  
&emsp;&emsp;Slave mode (1)  
&emsp;port, p  
&emsp; Digital port parallel type  
&emsp;&emsp;Off (0)  
&emsp;&emsp;BT.656 (1)  
&emsp;&emsp;CMOS (2)  
&emsp;cmos, c  
&emsp; CMOS content selection  
&emsp;&emsp;YUV422 (0)  
&emsp;&emsp;YUV422_parameter line (1)  
&emsp;&emsp;YUV16 (2)  
&emsp;&emsp;YUV16_parameter line (3)  
&emsp;&emsp;Y16_YUV422 (4)  
&emsp;&emsp;Y16_parameter_line_ YUV422 (5)  
&emsp;interface, i  
&emsp; CMOS interface type  
&emsp;&emsp;CMOS16 (0)  
&emsp;&emsp;CMOS8 MSB first (1)  
&emsp;&emsp;CMOS8 LSB first (2)  
&emsp;rate, r  
&emsp; Frame rate setting  
&emsp;&emsp;P-system 50Hz, N-system 60Hz (0)  
&emsp;&emsp;P-system 25Hz, N-system 30Hz (1)  
&emsp;&emsp;9Hz (2)  
&emsp;mipi, m  
&emsp; MIPI switch  
&emsp;&emsp;Off (0)  
&emsp;&emsp;On (1)  
&emsp;scene, x  
&emsp; Scene compensation  
&emsp;&emsp;Off (0)  
&emsp;&emsp;On (1)  
&emsp;shutter, z  
&emsp; Shutter compensation  
&emsp;&emsp;Off (0)  
&emsp;&emsp;On (1)  
Small icon: icon  
Sub commands:  
&emsp;num, n  
&emsp; Small icon number  
&emsp;&emsp;0 or 1  
&emsp;on  
&emsp; Small icon on  
&emsp;off  
&emsp; Small icon off  
&emsp;x  
&emsp; Small icon coordinate X  
&emsp;&emsp;0&#126;width-width  
&emsp;y  
&emsp; Small icon coordinate Y  
&emsp;&emsp;0&#126;height-width  
&emsp;width, w  
&emsp; Small icon width  
&emsp;transparency, t  
&emsp; Small icon transparency setting  
&emsp;&emsp;0&#126;4  
Menu bar: menu  
Sub commands:  
&emsp;on  
&emsp; Menu bar on  
&emsp;off  
&emsp; Menu bar off  
&emsp;location, y  
&emsp; Menu bar location setting  
&emsp;&emsp;0&#126;height-16  
&emsp;transparency, t  
&emsp; Menu bar transparency setting  
&emsp;&emsp;0&#126;4  
Layer: layer  
Sub commands:  
&emsp;on  
&emsp; Layer on  
&emsp;off  
&emsp; Layer off  
&emsp;transparency, t  
&emsp; Layer transparency setting  
&emsp;&emsp;0&#126;8  
Half pixel: hpcursor  
Sub commands:  
&emsp;on  
&emsp; Half pixel cursor on  
&emsp;off  
&emsp; Half pixel cursor off  
&emsp;x  
&emsp; Half pixel cursor coordinate X  
&emsp;&emsp;0&#126;width  
&emsp;y  
&emsp; Half pixel cursor coordinate Y  
&emsp;&emsp;0&#126;height  
&emsp;color, c  
&emsp; Half pixel color setting  
&emsp;&emsp;RGB value (0xRRGGBB)  
Area Analysis: area  
Sub commands:  
&emsp;mode, m  
&emsp; Analysis mode, values:  
&emsp;&emsp;Disable (0)  
&emsp;&emsp;Full screen analysis (1)  
&emsp;&emsp;Area one (2)  
&emsp;&emsp;Area two (3)  
&emsp;&emsp;Area three (4)  
&emsp;x  
&emsp; Area top left conner coordinate X  
&emsp;&emsp;0&#126;383  
&emsp;y  
&emsp; Area top left conner coordinate Y  
&emsp;&emsp;0&#126;287  
&emsp;width, w  
&emsp; Area width W  
&emsp;&emsp;1&#126;255  
&emsp;height, h  
&emsp; Area height H  
&emsp;&emsp;1&#126;255  
&emsp;r  
&emsp; The color R of rectangle  
&emsp;&emsp;0&#126;255  
&emsp;g  
&emsp; The color G of rectangle  
&emsp;&emsp;0&#126;255  
&emsp;b  
&emsp; The color B of rectangle  
&emsp;&emsp;0&#126;255  
&emsp;alarm, a  
&emsp; High temperature alarm switch  
&emsp;&emsp;High temperature alarm off (0)  
&emsp;&emsp;High temperature alarm on (1)  
&emsp;threshold, t  
&emsp; High temperature alarm threshold  
&emsp;&emsp;Observation 0&#126;65535  
&emsp;&emsp;Thermography -500&#126;10000  
Hotspot tracking: hptrack  
Sub commands:  
&emsp;on  
&emsp; Cursor switch on  
&emsp;off  
&emsp; Cursor switch off  
&emsp;upper, u  
&emsp; Hotspot tracking upper limit value  
&emsp;&emsp;Observation 0&#126;65535  
&emsp;&emsp;Thermography -500&#126;10000  
&emsp;lower, l  
&emsp; Hotspot tracking lower limit value  
&emsp;&emsp;Observation 0&#126;65535  
&emsp;&emsp;Thermography -500&#126;10000  
&emsp;r  
&emsp; The hottest cursor point color R  
&emsp;&emsp;0&#126;255  
&emsp;g  
&emsp; The hottest cursor point color G  
&emsp;&emsp;0&#126;255  
&emsp;b  
&emsp; The hottest cursor point color B  
&emsp;&emsp;0&#126;255  
Temperature measurement: temp  
Sub commands:  
&emsp;distance, d  
&emsp; Distance setting  
&emsp;&emsp;0&#126;100  
&emsp;emissivity, e  
&emsp; Measurement mode  
&emsp;&emsp;0&#126;100  
&emsp;show, s  
&emsp; Temperature Show  
&emsp;&emsp;degree Celsius (0)  
&emsp;&emsp;degree Fahrenheit (1)  
&emsp;&emsp;degree Kelvin (2)  
&emsp;calibration, c  
&emsp; Temperature Calibration  
&emsp;&emsp;-32768&#126;32767  
&emsp;reset, f  
&emsp; Factory reset  
&emsp;reflected, r  
&emsp; Reflected setting  
&emsp;save, v  
&emsp; Save settings  
&emsp;humidity, h  
&emsp; Humidity save settings  
&emsp;range, g  
&emsp; Temperature measurement range  
&emsp;&emsp;-20°C&#126;150°C  (0)  
&emsp;&emsp;-20°C&#126;800°C  (1)  
