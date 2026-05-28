// C Header File
// Created 11/19/01; 6:44:15 PM

#define EXT_PIXOFFSET2(w,x,y)  ((y) * (w) + (x>>3))
#define EXT_PIXADDR2(p,w,x,y)  (((unsigned char*)(p))+EXT_PIXOFFSET2(w,x,y))
#define EXT_PIXMASK2(x)      ((unsigned char)(0x80 >> ((x)&7)))

/*#define EXT_SETPIX_AM(a,m)   (*(a) |= (m))
#define EXT_CLRPIX_AM(a,m)   (*(a) &= ~(m))
#define EXT_XORPIX_AM(a,m)   (*(a) ^= (m))
#define EXT_GETPIX_AM(a,m)   (*(a) & (m))

#define EXT_PIXUP_AM(a)      ((a)-=30)
#define EXT_PIXDOWN_AM(a)    ((a)+=30)
#define EXT_PIXLEFT_AM(a,m)   asm("rol.b  #1,%0;bcc.s  0f;subq.l #1,%1;0:"\
                                  : "=d" (m), "=g" (a) : "0" (m), "1" (a))
#define EXT_PIXRIGHT_AM(a,m)  asm("ror.b  #1,%0;bcc.s  0f;addq.l #1,%1;0:"\
                                  : "=d" (m), "=g" (a) : "0" (m), "1" (a))*/

#define EXT_SETPIX2(p,w,x,y)   EXT_SETPIX_AM(EXT_PIXADDR2(p,w,x,y),EXT_PIXMASK2(x))
#define EXT_CLRPIX2(p,w,x,y)   EXT_CLRPIX_AM(EXT_PIXADDR2(p,w,x,y),EXT_PIXMASK2(x))
#define EXT_XORPIX2(p,w,x,y)   EXT_XORPIX_AM(EXT_PIXADDR2(p,w,x,y),EXT_PIXMASK2(x))
#define EXT_GETPIX2(p,w,x,y)   EXT_GETPIX_AM(EXT_PIXADDR2(p,w,x,y),EXT_PIXMASK2(x))

void FastDrawLine2(unsigned char* plane, char plane_width, short x1,short y1, short x2,short y2, short mode);
void Sprite8_OR2(short x, short y, short h, unsigned char* sprite, void* dest, char dest_width);
void Sprite8_XOR2(short x, short y, short h, unsigned char* sprite, void* dest, char dest_width);
void Sprite8_MASK2(short x,short y,short h,unsigned char* sprite,
									 unsigned char* mask,void* dest, char dest_widht);
void Sprite8_BLIT2(short x,short y,short h,unsigned char* sprite,
                  unsigned char maskval,void* dest, char dest_width);
void Sprite8_REVERSE2(short x, short y, short h, unsigned char* sprite, void* dest, char dest_width);
void GraySpriteClipX8_MASK(short x,short y,short h,
                       unsigned char* sprite1,
                       unsigned char* sprite2,
                       unsigned char* mask,
                       short bytewidth,
                       void* dest1,
                       void* dest2);
void GraySpriteClipX8_OR(short x,short y,short h,
                       unsigned char* sprite1,
                       unsigned char* sprite2,
                       short bytewidth,
                       void* dest1,
                       void* dest2);
