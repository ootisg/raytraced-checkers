#include <stdio.h>

#include "termbox.h"
#include "3d.h"

int main () {
#ifdef D3D_H
	//tb_init ();
	//tb_select_output_mode (TB_OUTPUT_256);
	d3d_window_x = 0;
	d3d_window_y = 0;
	d3d_window_width = tb_width ();
	d3d_window_height = tb_height () * 2;
	d3d_setup ();
	d3d_draw_window ();
	for (;;) {
		tb_present ();
		struct tb_event event;
		tb_poll_event (&event);
		if (TB_KEY_ESC == event.key) {
			break;
		}
		if (TB_KEY_ARROW_LEFT == event.key) {
			turn_left ();
			render ();
		}
		if (TB_KEY_ARROW_RIGHT == event.key) {
			turn_right ();
			render ();
		}
		if (TB_KEY_ARROW_UP == event.key) {
			forward ();
			render ();
		}
		if (TB_KEY_ARROW_DOWN == event.key) {
			backward ();
			render ();
		}
	}
	//tb_shutdown ();
	printf ("HELLO\n");
#endif
}
