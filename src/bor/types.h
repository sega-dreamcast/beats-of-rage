#ifndef GAMELIB_TYPES_H
#define GAMELIB_TYPES_H


#define		ANYNUMBER	2


//#pragma pack(push,4)



typedef struct{
	int	width;
	int	height;
	char *	data;
}s_screen;


typedef struct{
	int	width;
	int	height;
	int	planar;
	int	banked;		// Still unused
	char *	data;
}s_vram;


typedef struct{
	int	width;
	int	height;
	char	data[ANYNUMBER];
}s_bitmap;


typedef struct{
  int centerx;
  int centery;
  int width;
  int height;
  void *is_flip_of;
  int data[ANYNUMBER];
}s_sprite;

//#pragma pack(pop)

#endif



