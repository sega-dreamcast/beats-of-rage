// Generic control stuff (keyboard+joystick)

#include "dcport.h"
#include "dcpad.h"

#include "control.h"

#define	PAD_START 1
#define	PAD_END   (18*4)

static const char *padnames[PAD_END+1+1] = {
	"...",
#define CONTROLNAMES(x) \
	x" C",          \
	x" B",          \
	x" A",          \
	x" Start",      \
	x" Up",         \
	x" Down",       \
	x" Left",       \
	x" Right",      \
	x" Z",          \
	x" Y",          \
	x" X",          \
	x" D",          \
	x" Up 2",       \
	x" Down 2",     \
	x" Left 2",     \
	x" Right 2",    \
	x" L",          \
	x" R",
	CONTROLNAMES("P1")
	CONTROLNAMES("P2")
	CONTROLNAMES("P3")
	CONTROLNAMES("P4")
	"undefined"
};

static unsigned getpadbits(int port) {
  unsigned bits0 = dcpad_buttonstate0[(port) & 3];
  unsigned bits1 = dcpad_buttonstate1[(port) & 3];
  bits0 ^= 0xFFFF;
  bits0 &= 0x8080FFFF;
  if(bits0 & 0x80000000) bits0 |= 0x10000;  // analog L
  if(bits0 & 0x00800000) bits0 |= 0x20000;  // analog R
  bits0 &= 0x0003FFFF;
  if(((bits1>>8) & 0xFF) <= 0x40) bits0 |= 0x10; // analog up
  if(((bits1>>8) & 0xFF) >= 0xC0) bits0 |= 0x20; // analog down
  if(((bits1   ) & 0xFF) <= 0x40) bits0 |= 0x40; // analog left
  if(((bits1   ) & 0xFF) >= 0xC0) bits0 |= 0x80; // analog right
  return bits0;
}

static int flag_to_index(unsigned flag){
	int index = 0;
	unsigned bit = 1;

	while(!((bit<<index)&flag) && index<31) ++index;
	return index;
}

void control_exit(){ }

void control_init(int joy_enable){ }


int control_usejoy(int enable){ return 0; }
int control_getjoyenabled(){ return 0; }


void control_setkey(s_playercontrols * pcontrols, unsigned int flag, int key){
	if(!pcontrols) return;
	pcontrols->settings[flag_to_index(flag)] = key;
	pcontrols->keyflags = pcontrols->newkeyflags = 0;
}



// Scan input for newly-pressed keys.
// Return value:
// 0  = no key was pressed
// >0 = key code for pressed key
// <0 = error
int control_scankey(){
  static unsigned ready = 0;
  unsigned k=0;
  unsigned port0=getpadbits(0);
  unsigned port1=getpadbits(1);
  unsigned port2=getpadbits(2);
  unsigned port3=getpadbits(3);

  k = 0;
       if(port0) k = 1 + 0*18 + flag_to_index(port0);
  else if(port1) k = 1 + 1*18 + flag_to_index(port1);
  else if(port2) k = 1 + 2*18 + flag_to_index(port2);
  else if(port3) k = 1 + 3*18 + flag_to_index(port3);

  if(ready && k) {
    ready = 0;
    return k;
  }
  ready = (!k);
  return 0;
}


char * control_getkeyname(unsigned keycode){

	if(keycode >= PAD_START && keycode <= PAD_END){
		return (char*)padnames[keycode];
	}
	return "...";
}


void control_update(s_playercontrols ** playercontrols, int numplayers){

	unsigned gamelib_long k;
	unsigned gamelib_long i;
	int player;
	int t;
	s_playercontrols * pcontrols;
  unsigned port[4];
  port[0]=getpadbits(0);
  port[1]=getpadbits(1);
  port[2]=getpadbits(2);
  port[3]=getpadbits(3);
	for(player=0; player<numplayers; player++){

		pcontrols = playercontrols[player];

		k = 0;

		for(i=0; i<32; i++){
			t = pcontrols->settings[i];
			if(t >= PAD_START && t <= PAD_END){
				int portnum = (t-1) / 18;
				int shiftby = (t-1) % 18;
				if(portnum >= 0 && portnum <= 3){
					if((port[portnum] >> shiftby) & 1) k |= (1<<i);
				}
			}
		}
		pcontrols->kb_break = 0; //kb_break;

		pcontrols->newkeyflags = k & (~pcontrols->keyflags);
		pcontrols->keyflags = k;
	}
}

