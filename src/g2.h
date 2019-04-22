#ifndef G2_H
#define G2_H

char g2_readbyte (char *address);
void g2_writebyte(char *address, char data);
int  g2_readword (int  *address);
void g2_writeword(int  *address, int  data);

void g2_copybytes(char *dst, const char *src, int n);
void g2_copywords(int  *dst, const int  *src, int n);

#endif
