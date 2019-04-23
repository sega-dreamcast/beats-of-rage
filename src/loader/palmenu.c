/* KallistiOS Git revision v2.0.0-301-ga449e93

   palmenu.c
   Copyright (C) 2015 Lawrence Sebald

   Every once in a while, a question pops up on one of the various Dreamcast-
   related forums of how to deal with 50Hz TVs. Well, the standard tactic is to
   detect if the console is European and display a simple menu if so. That's
   exactly what this example does.

   For those on a US or Japanese console, this example is pretty boring.
*/

#include <kos.h>
#include <kos/thread.h>
#include <assert.h>

#include <dc/biosfont.h>
#include <dc/flashrom.h>
#include <dc/maple.h>
#include <dc/video.h>
#include <dc/maple/controller.h>

#define USE_50HZ 0
#define USE_60HZ 1

#define BINARY_NTSC "/cd/bor_ntsc.bin"
#define BINARY_PAL "/cd/bor_pal.bin"

#define false (1 == 0)

/* Draw a very simple "menu" on the screen to pick between 50Hz and 60Hz mode...
   This could obviously be spruced up a bit with a real menu, but this will do
   for an example. */
static int pal_menu(void) {
    maple_device_t *cont1;
    cont_state_t *state;

    /* Re-init to a 50Hz mode to display the menu. */
    vid_set_mode(DM_640x480_PAL_IL, PM_RGB565);

    /* Draw the "menu" on the screen. */
    bfont_draw_str(vram_s + 640 * 200 + 64, 640, 1, "Press A to run at 60Hz");
    bfont_draw_str(vram_s + 640 * 240 + 64, 640, 1, "or B to run at 50Hz");

    /* Wait for the user to press either A or B to pick which mode to use.*/
    for(;;) {
        if((cont1 = maple_enum_type(0, MAPLE_FUNC_CONTROLLER))) {
            if((state = (cont_state_t *)maple_dev_status(cont1))) {
                if(state->buttons & CONT_A)
                    return USE_60HZ;
                else if(state->buttons & CONT_B)
                    return USE_50HZ;
            }
        }

        /* Sleep for a bit. */
        thd_sleep(20);
    }
}

void exec(char *filename) {
    int length = -1;
	void *subelf;

	/* Load in memory the binary to execute from the disk */
	length = fs_load(filename, &subelf);
	
	/* The file was not found on the disk! */
	if (length == -1) {
		assert_msg(false, "fs_load call failed");
	}

    /* Tell exec to replace us */   
    arch_exec(subelf, length);

    /* Shouldn't get here */
    assert_msg(false, "exec call failed");
}

int main(int argc, char *argv[]) {
    int region, cable,
		mode = USE_60HZ;

    /* KOS normally initializes the video hardware to run at 60Hz, so on NTSC
       consoles, or those with VGA connections, we don't have to do anything
       else here... */
    region = flashrom_get_region();
    cable = vid_check_cable();

    /* So, if we detect a European console that isn't using VGA, prompt the user
       whether they want 50Hz mode or 60Hz mode. */
    if(region == FLASHROM_REGION_EUROPE && cable != CT_VGA) {
        mode = pal_menu();        
    }
	
	/* Execute the right program */
	if(mode == USE_60HZ)
		exec(BINARY_NTSC);
	else
		exec(BINARY_PAL);

    return 0;
}
