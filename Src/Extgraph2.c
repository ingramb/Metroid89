#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#include <tigcclib.h>         // Include All Header Files
#include "utility.h"

//enum Attrs {A_REVERSE,A_NORMAL,A_XOR,A_SHADED,A_REPLACE,A_OR,A_AND,A_THICK1,
//            A_SHADE_V,A_SHADE_H,A_SHADE_NS,A_SHADE_PS};

/*---------------------------------------------------------------------------*/
/* speedy pixel access routines                                              */
/*---------------------------------------------------------------------------*/
#define PIXOFFSET(x,y)  ((y) * plane_width + (x>>3))
#define PIXADDR(p,x,y)  (((unsigned char*)(p))+PIXOFFSET(x,y))
#define PIXMASK(x)      ((unsigned char)(0x80 >> ((x)&7)))

#define SETPIX(a,m)   (*(a) |= (m)) // uses precalculated values now
#define CLRPIX(a,m)   (*(a) &= ~(m))
#define XORPIX(a,m)   (*(a) ^= (m))
#define PIXUP(a)      ((a)-=plane_width)
#define PIXDOWN(a)    ((a)+=plane_width)
#define PIXLEFT(a,m)   asm("rol.b  #1,%0;bcc.s  0f;subq.l #1,%1;0:"\
                          : "=d" (m), "=g" (a) : "0" (m), "1" (a))
#define PIXRIGHT(a,m)  asm("ror.b  #1,%0;bcc.s  0f;addq.l #1,%1;0:"\
                          : "=d" (m), "=g" (a) : "0" (m), "1" (a))


/*---------------------------------------------------------------------------*/
/* useful macros                                                             */
/*---------------------------------------------------------------------------*/
#define ABS(a)          (((a)<0) ? -(a): (a))
#define SPECIAL_SGN(a)  (((a)<0) ? 0 : 1)


/*===========================================================================*/
/* draws a line from (x1,y1) to (x2,y2) in given plane                       */
/*                                                                           */
/* valid modes: A_REPLACE,A_NORMAL,A_REVERSE,A_XOR                           */
/*===========================================================================*/
void FastDrawLine2(unsigned char* plane, char plane_width, short x1,short y1, short x2,short y2, short mode)
{
    short d,x,y,ax,ay,sx,sy,dx,dy;

    unsigned char *pixaddr;
    unsigned char pixmask;

    dx = x2 - x1;
    ax = ABS(dx)<<1;
    sx = SPECIAL_SGN(dx);

    dy = y2 - y1;
    ay = ABS(dy)<<1;
    sy = SPECIAL_SGN(dy);

    x = x1;
    y = y1;

    pixaddr=PIXADDR(plane, x, y);
    pixmask=PIXMASK(x);

    //-------------------------------------------------------------------------
    // for speedup reasons the different modes are handled by own blocks.
    // The only difference is the usage of SETPIX/CLRPIX or XORPIX
    //-------------------------------------------------------------------------

    // sets each pixel of the line ...

    if (mode == A_REPLACE || mode == A_NORMAL) {
        if (ax>ay) {           /* x dominant */
            d = ay - (ax>>1);
            do {
                SETPIX(pixaddr,pixmask);
                if (x==x2) return;
                if (d>=0) {
                    if(sy) PIXDOWN(pixaddr);
                    else   PIXUP(pixaddr);
                    d -= ax;
                }
                if (sx) {
                    PIXRIGHT(pixaddr, pixmask);
                    x++;
                }
                else {
                    PIXLEFT(pixaddr, pixmask);
                    x--;
                }
                d += ay;
            } while(1);
        }
        else {                 /* y dominant */
            d = ax - (ay>>1);
            do {
                SETPIX(pixaddr,pixmask);
                if (y==y2) return;
                if (d>=0) {
                    if (sx) PIXRIGHT(pixaddr, pixmask);
                    else    PIXLEFT(pixaddr, pixmask);
                    d -= ay;
                }
                if (sy) {
                    PIXDOWN(pixaddr);
                    y++;
                }
                else {
                    PIXUP(pixaddr);
                    y--;
                }
                d += ax;
            } while(1);
        }
    }

    // clears each pixel of the line ...
    else if (mode == A_REVERSE) {
        if (ax>ay) {           /* x dominant */
            d = ay - (ax>>1);
            do {
                CLRPIX(pixaddr,pixmask);
                if (x==x2) return;
                if (d>=0) {
                    if (sy) PIXDOWN(pixaddr);
                    else    PIXUP(pixaddr);
                    d -= ax;
                }
                if (sx) {PIXRIGHT(pixaddr, pixmask);x++;}
                else    {PIXLEFT(pixaddr, pixmask);x--;}
                d += ay;
            } while(1);
        }
        else {                 /* y dominant */
            d = ax - (ay>>1);
            do {
                CLRPIX(pixaddr,pixmask);
                if (y==y2) return;
                if (d>=0) {
                    if (sx) PIXRIGHT(pixaddr, pixmask);
                    else    PIXLEFT(pixaddr, pixmask);
                    d -= ay;
                }
                if(sy) {PIXDOWN(pixaddr);y++;}
                else   {PIXUP(pixaddr);y--;}
                d += ax;
            } while(1);
        }
    }

    /* XORs each pixel of the line ...

    else if (mode == A_XOR) {
        if (ax>ay) {           // x dominant
            d = ay - (ax>>1);
            do {
                XORPIX(pixaddr,pixmask);
                if (x==x2) return;
                if (d>=0) {
                    if (sy) PIXDOWN(pixaddr);
                    else    PIXUP(pixaddr);
                    d -= ax;
                }
                if (sx) {PIXRIGHT(pixaddr, pixmask);x++;}
                else    {PIXLEFT(pixaddr, pixmask);x--;}
                d += ay;
            } while(1);
        }
        else {                 // y dominant
            d = ax - (ay>>1);
            do {
                XORPIX(pixaddr,pixmask);
                if (y==y2) return;
                if (d>=0) {
                    if (sx) PIXRIGHT(pixaddr, pixmask);
                    else    PIXLEFT(pixaddr, pixmask);
                    d -= ay;
                }
                if (sy) {PIXDOWN(pixaddr);y++;}
                else    {PIXUP(pixaddr);y--;}
                d += ax;
            } while(1);
        }
    }*/

    // NOTE: if the mode is not within A_REPLACE/A_REVERSE/A_XOR nothing is drawn !!
}

void Sprite8_OR2(short x, short y, short h, unsigned char* sprite, void* dest, char dest_width)
{
    register long           addr = (long)dest + y*dest_width + ((x>>3)&0xfffe);
    register unsigned short cnt  = 24-(x&15);
    for(;h;h--,addr+=dest_width) *(long*)addr|=(long)(*sprite++)<<cnt;
}

void Sprite8_XOR2(short x, short y, short h, unsigned char* sprite, void* dest, char dest_width)
{
    register long           addr = (long)dest+y*dest_width+((x>>3)&0xfffe);
    register unsigned short cnt  = 24-(x&15);
    for(;h;h--,addr+=dest_width) *(long*)addr^=(long)(*sprite++)<<cnt;
}

void Sprite8_BLIT2(short x,short y,short h,unsigned char* sprite,
                  unsigned char maskval,void* dest, char dest_width)
{
    register long           addr     = (long)dest+y*dest_width+((x>>3)&0xfffe);
    register unsigned short cnt      = 24-(x&15);
    register long           maskval2 = ~((long)(~(maskval)&0xff)<<cnt);
    for (;h;h--,addr+=dest_width) {
        *(long*)addr&=maskval2;
        *(long*)addr|=(long)(*sprite++)<<cnt;
    }
}

void Sprite8_MASK2(short x,short y,short h,unsigned char* sprite,
                  unsigned char* mask,void* dest, char dest_width)
{
    register long           addr = (long)dest+y*dest_width+((x>>3)&0xfffe);
    register unsigned short cnt  = 24-(x&15);

    for (;h;h--,addr+=dest_width) {
        *(long*)addr&=~((long)(~(*mask++)&0xff)<<cnt);
        *(long*)addr|=(long)(*sprite++)<<cnt;
    }
}


void Sprite8_REVERSE2(short x, short y, short h, unsigned char* sprite, void* dest, char dest_width)
{
    register long           addr = (long)dest+y*dest_width+((x>>3)&0xfffe);
    register unsigned short cnt  = 24-(x&15);
    for(;h;h--,addr+=dest_width) *(long*)addr&=~((long)(*sprite++)<<cnt);
}

void GraySpriteClipX8_OR(short x,short y,short h,
                       unsigned char* sprite1,
                       unsigned char* sprite2,
                       short bytewidth,
                       void* dest1,
                       void* dest2)
{
    register long           offset;
    register unsigned char* addr1;
    register unsigned char* addr2;
    register unsigned short shift1;
    register unsigned short shift2;
    register unsigned short lineoffset;
    register          short loop;
    register 					short left_buffer = 0;
    register					short right_buffer = 0;
    
    if(x < -bytewidth * 8 + 1 || x > 159 || y < -h + 1 || y > 99) return;
    
    if(y < 0){
    	h += y;
    	sprite1 -= (y * bytewidth);
    	sprite2 -= (y * bytewidth);
    	y = 0;
    }
    if(y + h > 99)
    	h = 100 - y;
    
    if(x < 0){
    	left_buffer = (-x) / 8;
    	bytewidth -= left_buffer;
    	x += left_buffer * 8;
    }
    if(x + bytewidth * 8> 159){
    	right_buffer = (x + bytewidth * 8 - 160) / 8;
    	bytewidth -= right_buffer;
    }
    
    x += x_buffer;
    y += y_buffer;
    
    offset = (y<<5)-(y<<1)+(x>>3);
    addr1  = ((unsigned char*)dest1)+offset;
    addr2  = ((unsigned char*)dest2)+offset;
    shift1 = x & 7;
    lineoffset = 30-bytewidth;

    if (shift1) {
        shift2 = 8 - shift1;
        for (;h;h--,addr1+=lineoffset,addr2+=lineoffset) {
            
            if(left_buffer){
            	sprite1 += left_buffer;
            	sprite2 += left_buffer;
            }
            
            *addr1++ |= *sprite1 >> shift1;
            *addr2++ |= *sprite2 >> shift1;
            for (loop=1;loop<bytewidth;loop++) {
               *addr1   |= ((*sprite1++) << shift2);
               *addr1++ |= (*sprite1 >> shift1);
               *addr2   |= ((*sprite2++) << shift2);
               *addr2++ |= (*sprite2 >> shift1);
            }
            *addr1 |= (*sprite1++ << shift2);
            *addr2 |= (*sprite2++ << shift2);
            
            if(right_buffer){
            	sprite1 += right_buffer;
            	sprite2 += right_buffer;
            }
        }
    }
    else {
        for (;h;h--,addr1+=lineoffset,addr2+=lineoffset) {
            
            if(left_buffer){
            	sprite1 += left_buffer;
            	sprite2 += left_buffer;
            }
            
            for (loop=0;loop<bytewidth;loop++) {
                *addr1++ |= *sprite1++;
                *addr2++ |= *sprite2++;
            }
            
            if(right_buffer){
            	sprite1 += right_buffer;
            	sprite2 += right_buffer;
            }
        }
    }
}

void GraySpriteClipX8_MASK(short x,short y,short h,
                       unsigned char* sprite1,
                       unsigned char* sprite2,
                       unsigned char* mask,
                       short bytewidth,
                       void* dest1,
                       void* dest2)
{
    register long           offset;
    register unsigned char* addr1;
    register unsigned char* addr2;
    register unsigned short shift1;
    register unsigned short shift2;
    register unsigned short lineoffset;
    register          short loop;
    register unsigned char  startmask;
    register unsigned char  endmask;
    register 					short left_buffer = 0;
    register					short right_buffer = 0;
    
    if(x < -bytewidth * 8 + 1 || x > 159 || y < -h + 1 || y > 99) return;
    
    if(y < 0){
    	h += y;
    	sprite1 -= (y * bytewidth);
    	sprite2 -= (y * bytewidth);
    	mask -= (y * bytewidth);
    	y = 0;
    }
    if(y + h > 99)
    	h = 100 - y;
    
    if(x < 0){
    	left_buffer = (-x) / 8;
    	bytewidth -= left_buffer;
    	x += left_buffer * 8;
    }
    if(x + bytewidth * 8> 159){
    	right_buffer = (x + bytewidth * 8 - 160) / 8;
    	bytewidth -= right_buffer;
    }
    
    x += x_buffer;
    y += y_buffer;
    
    offset = (y<<5)-(y<<1)+(x>>3);
    addr1  = ((unsigned char*)dest1)+offset;
    addr2  = ((unsigned char*)dest2)+offset;
    shift1 = x & 7;
    lineoffset = 30-bytewidth;

    if (shift1) {
        shift2 = 8 - shift1;
        startmask = 0xff << shift2;
        endmask   = 0xff >> shift1;
        for (;h;h--,addr1+=lineoffset,addr2+=lineoffset) {
            
            if(left_buffer){
            	sprite1 += left_buffer;
            	sprite2 += left_buffer;
            	mask += left_buffer;
            }
            
            *addr1   &= ((*mask) >> shift1) | startmask;
            *addr1++ |= *sprite1 >> shift1;
            *addr2   &= ((*mask) >> shift1) | startmask;
            *addr2++ |= *sprite2 >> shift1;
            for (loop=1;loop<bytewidth;loop++) {
               register unsigned char val = (*mask++) << shift2;
               *addr1   &= val | ((*mask) >> shift1);
               *addr1   |= ((*sprite1++) << shift2);
               *addr1++ |= (*sprite1 >> shift1);
               *addr2   &= val | ((*mask) >> shift1);
               *addr2   |= ((*sprite2++) << shift2);
               *addr2++ |= (*sprite2 >> shift1);
            }
            *addr1 &= ((*mask) << shift2) | endmask;
            *addr1 |= (*sprite1++ << shift2);
            *addr2 &= ((*mask++) << shift2) | endmask;
            *addr2 |= (*sprite2++ << shift2);
            
            if(right_buffer){
            	sprite1 += right_buffer;
            	sprite2 += right_buffer;
            	mask += right_buffer;
            }
        }
    }
    else {
        for (;h;h--,addr1+=lineoffset,addr2+=lineoffset) {
            
            if(left_buffer){
            	sprite1 += left_buffer;
            	sprite2 += left_buffer;
            	mask += left_buffer;
            }
            
            for (loop=0;loop<bytewidth;loop++) {
                *addr1   &= *mask;
                *addr1++ |= *sprite1++;
                *addr2   &= *mask++;
                *addr2++ |= *sprite2++;
            }
            
            if(right_buffer){
            	sprite1 += right_buffer;
            	sprite2 += right_buffer;
            	mask += right_buffer;
            }
        }
    }
}

