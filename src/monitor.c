#include "monitor.h"
#define MONITOR_WIDTH 80
#define MONITOR_HEIGTH 25

static uint8 cursor_x = 0;
static uint8 cursor_y = 0;
static uint16* video_memory = (uint16*) 0xb8000;

static void move_cursor()
{
   // The screen is 80 characters wide...
   uint16 cursorLocation = cursor_y * MONITOR_WIDTH + cursor_x;
   outb(0x3D4, 14);                  // Tell the VGA board we are setting the high cursor byte.
   outb(0x3D5, cursorLocation >> 8); // Send the high cursor byte.
   outb(0x3D4, 15);                  // Tell the VGA board we are setting the low cursor byte.
   outb(0x3D5, cursorLocation);      // Send the low cursor byte.
}

static void scroll() 
{
	 uint8 attributeByte = (0 /*black*/ << 4) | (15 /*white*/ & 0x0f);
	 uint16 blank = 0x20 /* space */ | (attributeByte << 8);

	if (cursor_y >= MONITOR_HEIGTH) {
		int i;
		for (i = 0*MONITOR_WIDTH; i < (MONITOR_HEIGTH-1)*MONITOR_WIDTH; i++) {
			video_memory[i] = video_memory[i + MONITOR_WIDTH];
		}

		for (i = (MONITOR_HEIGTH-1)*MONITOR_WIDTH;i < MONITOR_WIDTH*MONITOR_HEIGTH; i++) {
			video_memory[i] = blank;
		}

		cursor_y = MONITOR_HEIGTH-1;
	}
}

void monitor_put(char c)
{
	uint8 background_color = 0;/*black*/
	uint8 font_color = 15; /*white*/

	uint8 attributeByte = (background_color << 4)|(font_color & 0x0f);
	uint16 attribute = attributeByte<<8;
	uint16 *location;

	if (c == 0x08 && cursor_x) {
		/*backspace*/
		cursor_x --;
	}
	else if (c == 0x09) {
		/*tab*/
		cursor_x = (cursor_x + 4)&(~(4 -1));
	}
	else if (c == '\r') {
		/*return*/
		cursor_x = 0;
	}
	else if (c == '\n') {
		cursor_y++;
		cursor_x = 0;
	}
	else if (c >= ' ') {
		location = video_memory + (cursor_y*MONITOR_WIDTH + cursor_x);
		*location = c|attribute;
		cursor_x ++;
	}

	if (cursor_x >= MONITOR_WIDTH) {
		cursor_y ++;
		cursor_x = 0;
	}

	//scroll();
	move_cursor();
}

void monitor_write(char *c)
{
	int i = 0;
	while (c[i]) {
		monitor_put(c[i++]);
	}
}

void monitor_clear()
{
	uint16 attribute = 0x0020;
	int i;
	for (i = 0; i < MONITOR_WIDTH*MONITOR_HEIGTH; i++) {
		video_memory[i] = attribute;
	}

	cursor_x = 0;
	cursor_y = 0;
	move_cursor();
}