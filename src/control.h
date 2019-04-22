#ifndef	CONTROL_H
#define	CONTROL_H

// Generic control stuff (keyboard+joystick).


#define	CONTROL_ESC		73

#define	CONTROL_DEFAULT1_UP	 5
#define	CONTROL_DEFAULT1_DOWN	 6
#define	CONTROL_DEFAULT1_LEFT	 7
#define	CONTROL_DEFAULT1_RIGHT	 8
#define	CONTROL_DEFAULT1_SPECIAL 10
#define CONTROL_DEFAULT1_ATTACK  11
#define CONTROL_DEFAULT1_JUMP    3
#define CONTROL_DEFAULT1_START   4
#define CONTROL_DEFAULT1_SCREENSHOT 73

#define	CONTROL_DEFAULT2_UP	 (5+18)
#define	CONTROL_DEFAULT2_DOWN	 (6+18)
#define	CONTROL_DEFAULT2_LEFT	 (7+18)
#define	CONTROL_DEFAULT2_RIGHT	 (8+18)
#define	CONTROL_DEFAULT2_SPECIAL (10+18)
#define CONTROL_DEFAULT2_ATTACK  (11+18)
#define CONTROL_DEFAULT2_JUMP    (3+18)
#define CONTROL_DEFAULT2_START   (4+18)
#define CONTROL_DEFAULT2_SCREENSHOT 73


typedef struct{
	int		settings[32];
	unsigned gamelib_long	keyflags, newkeyflags;
	int		kb_break;
}s_playercontrols;

void control_exit();
void control_init(int joy_enable);
int control_usejoy(int enable);
int control_getjoyenabled();

void control_setkey(s_playercontrols * pcontrols, unsigned int flag, int key);
int control_scankey();

char * control_getkeyname(unsigned int keycode);
void control_update(s_playercontrols ** playercontrols, int numplayers);


#endif

