// C Source File
// Created 2/2/02; 12:36:45 PM

//#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#include "globals.h"
#include "dllutility.h"
#include "dllclipsprites.h"
#include "bitops.h"   // big-endian 32-bit framebuffer access (LD32/ST32)

/*void GraySpriteClip16_MASK(
	short x, short y, short h, unsigned short* sprite1) {
    register long            addr1   = (long)glbs->light_buffer;
    register long            addr2   = (long)glbs->dark_buffer;
    register unsigned short* sprite2 = sprite1 + h;
    register unsigned short* mask    = sprite2 + h;
    register long            offset;
    register unsigned short  cnt;

    if(x <= -16 || x >=  glbs->display_width || y < -h || y >= glbs->display_hieght) return;

    if (y<0) {
        sprite1 -= y;
        sprite2 -= y;
        mask    -= y;
        h       += y;
        y        = 0;
    }

    else if (y+h>glbs->display_hieght) h = glbs->display_hieght-y;

    if (x < 0) {
        cnt    = -x+16;
        offset = (y<<5)-(y<<1);
    } else {
    		cnt    = 16-(x&15);
        offset = (y<<5)-(y<<1)+((x>>3)&0x1e);
    }
    addr1  += offset;
    addr2  += offset;

    for (;h;h--,addr1+=30,addr2+=30) {
			register long val = ~((long)(~*mask++)<<cnt);
			*(long*)addr1&=val;
			*(long*)addr1|=(long)(*sprite1++)<<cnt;
			*(long*)addr2&=val;
			*(long*)addr2|=(long)(*sprite2++)<<cnt;
		}
}*/

void GraySpriteClip8_OR(short x, short y, short h, unsigned char* sprite1) {
    // The original wrote the framebuffer with `*(long*)` (32-bit big-endian on
    // m68k). On a little-endian / 64-bit host that both reads the wrong byte
    // order and clobbers 4 extra bytes, so route the word access through
    // LD32/ST32 with a uint32 accumulator (matches the calculator exactly).
    register unsigned char*  addr1   = (unsigned char*)glbs->light_buffer;
    register unsigned char*  addr2   = (unsigned char*)glbs->dark_buffer;
    register unsigned char*  sprite2 = sprite1 + h;
    register unsigned char*  mask    = sprite2 + h;
    register long            offset;
    register unsigned short  cnt;

    if(x <= -8 || x >=  glbs->display_width || y < -h || y >= glbs->display_hieght) return;

    if (y<0) {
        sprite1 -= y;
        sprite2 -= y;
        mask    -= y;
        h       += y;
        y        = 0;
    }

    else if (y+h>glbs->display_hieght) h = glbs->display_hieght-y;

    if (x < 0) {
        cnt    = -x+24;
        offset = (y<<5)-(y<<1);
    } else {
    		cnt    = 24-(x&15);
        offset = (y<<5)-(y<<1)+((x>>3)&0x1e);
    }
    addr1  += offset;
    addr2  += offset;

    for (;h;h--,addr1+=30,addr2+=30) {
			unsigned long val = ~((unsigned long)(~(*mask++)&0xff)<<cnt);
			ST32(addr1, (LD32(addr1) & val) | ((unsigned long)(*sprite1++)<<cnt));
			ST32(addr2, (LD32(addr2) & val) | ((unsigned long)(*sprite2++)<<cnt));
		}
}

/*void GraySpriteClip8_MASK(short x, short y, short h, unsigned char* sprite1) {
    register long            addr1   = (long)glbs->light_buffer;
    register long            addr2   = (long)glbs->dark_buffer;
    register unsigned char*  sprite2 = sprite1 + h;
    register long            offset;
    register unsigned short  cnt;

    if(x <= -8 || x >=  glbs->display_width || y < -h || y >= glbs->display_hieght) return;

    if (y<0) {
        sprite1 -= y;
        sprite2 -= y;
        h       += y;
        y        = 0;
    }

    else if (y+h>glbs->display_hieght) h = glbs->display_hieght-y;

    if (x < 0) {
        cnt    = -x+24;
        offset = (y<<5)-(y<<1);
    } else {
    		cnt    = 24-(x&15);
        offset = (y<<5)-(y<<1)+((x>>3)&0x1e);
    }
    addr1  += offset;
    addr2  += offset;

    for (;h;h--,addr1+=30,addr2+=30) {
			*(long*)addr1|=(long)(*sprite1++)<<cnt;
			*(long*)addr2|=(long)(*sprite2++)<<cnt;
		}
}*/

void GraySpriteClipX8_MASK(
	short x,short y,short h, unsigned char* sprite1, short bytewidth, char flash)
{
    unsigned char *sprite2 = sprite1 + bytewidth * h;
    unsigned char *mask = sprite2 + bytewidth * h;

    long           offset;
    unsigned char* addr1;
    unsigned char* addr2;
    unsigned short shift1;
    unsigned short shift2;
    unsigned short lineoffset;
             short loop;
    unsigned char  startmask;
    unsigned char  endmask;
     				 short buffer;

    // Native port: the width-1/2/4 fast paths were hand-written m68k asm
    // (the missing ClipSprite{8,16,32}_MASK_R.s). Use the generic byte-wise
    // blit below for all widths -- it's endian-safe and handles any bytewidth.

    if(x <= -bytewidth * 8 || x >=  glbs->display_width || y < -h || y >= glbs->display_hieght) return;

    if(y < 0){
    	h += y;
    	sprite1 -= (y * bytewidth);
    	sprite2 -= (y * bytewidth);
    	mask -= (y * bytewidth);
    	y = 0;
    }
    if(y + h >= glbs->display_hieght) h = glbs->display_hieght - y;

	  offset = (y<<5)-(y<<1)+(x>>3);
	  addr1  = ((unsigned char*)glbs->light_buffer)+offset;
	  addr2  = ((unsigned char*)glbs->dark_buffer)+offset;

	  if(flash == 1) {
	  	sprite2 = sprite1;
	  	addr2 = addr1;
	  } else if(flash == 2)
	  	sprite2 = sprite1;

	  shift1 = x & 7;
	  lineoffset = 30-bytewidth;

	  shift2 = 8 - shift1;
	  startmask = 0xff << shift2;
	  endmask   = 0xff >> shift1;

	  if(x < 0) {
    	buffer = (-x) >> 3;
    	if((buffer << 3) != (-x)) buffer++;

	    for (;h;h--,addr1+=lineoffset,addr2+=lineoffset) {
	        sprite1 += buffer - 1;
	        sprite2 += buffer - 1;
	        mask += buffer - 1;
	        addr1 += buffer;
	        addr2 += buffer;

	        for (loop=buffer;loop<bytewidth;loop++) {
	           unsigned char val = (*mask++) << shift2;
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
	    }

    } else if(x + (bytewidth << 3) >= glbs->display_width) {
    	buffer = (x + (bytewidth << 3) - glbs->display_width) >> 3;

	    for (;h;h--,addr1+=lineoffset,addr2+=lineoffset) {
	        *addr1   &= ((*mask) >> shift1) | startmask;
	        *addr1++ |= *sprite1 >> shift1;
	        *addr2   &= ((*mask) >> shift1) | startmask;
	        *addr2++ |= *sprite2 >> shift1;
	        for (loop=1;loop<bytewidth-buffer;loop++) {
	           unsigned char val = (*mask++) << shift2;
	           *addr1   &= val | ((*mask) >> shift1);
	           *addr1   |= ((*sprite1++) << shift2);
	           *addr1++ |= (*sprite1 >> shift1);
	           *addr2   &= val | ((*mask) >> shift1);
	           *addr2   |= ((*sprite2++) << shift2);
	           *addr2++ |= (*sprite2 >> shift1);
	        }

	        sprite1 += buffer + 1;
	        sprite2 += buffer + 1;
	        mask += buffer + 1;
	        addr1 += buffer;
	        addr2 += buffer;
	    }

	  } else {

	    for (;h;h--,addr1+=lineoffset,addr2+=lineoffset) {
	        *addr1   &= ((*mask) >> shift1) | startmask;
	        *addr1++ |= *sprite1 >> shift1;
	        *addr2   &= ((*mask) >> shift1) | startmask;
	        *addr2++ |= *sprite2 >> shift1;
	        for (loop=1;loop<bytewidth;loop++) {
	           unsigned char val = (*mask++) << shift2;
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
	    }
		}
}

void GraySpriteClipX8_WMASK(
	short x,short y,short h, unsigned char* sprite1, short bytewidth, char flash)
{
    unsigned char *sprite2 = sprite1 + bytewidth * h;
    unsigned char *mask1, *mask2;

    long           offset;
    unsigned char* addr1;
    unsigned char* addr2;
    unsigned short shift1;
    unsigned short shift2;
    unsigned short lineoffset;
             short loop;
    unsigned char  startmask;
    unsigned char  endmask;
     				 short buffer;

    if(x < -bytewidth * 8 + 1 || x >=  glbs->display_width || y < -h || y >= glbs->display_hieght) return;

    if(y < 0){
    	h += y;
    	sprite1 -= (y * bytewidth);
    	sprite2 -= (y * bytewidth);
    	y = 0;
    }
    if(y + h >= glbs->display_hieght) h = glbs->display_hieght - y;

	  offset = (y<<5)-(y<<1)+(x>>3);
	  addr1  = ((unsigned char*)glbs->light_buffer)+offset;

	  if(flash) {
	  	sprite2 = sprite1;
	  	addr2 = addr1;
	  } else
	  	addr2  = ((unsigned char*)glbs->dark_buffer)+offset;

	  shift1 = x & 7;
	  lineoffset = 30-bytewidth;

	  shift2 = 8 - shift1;
	  startmask = 0xff << shift2;
	  endmask   = 0xff >> shift1;

	  mask1 = sprite1;
	  mask2 = sprite2;

	  if(x < 0) {
    	buffer = (-x) >> 3;
    	if((buffer << 3) != (-x)) buffer++;

	    for (;h;h--,addr1+=lineoffset,addr2+=lineoffset) {
	        sprite1 += buffer - 1;
	        sprite2 += buffer - 1;
	        mask1 += buffer - 1;
	        mask2 += buffer - 1;
	        addr1 += buffer;
	        addr2 += buffer;

	        for (loop=buffer;loop<bytewidth;loop++) {
	           unsigned char val = (*mask1++ | *mask2++) << shift2;
	           *addr1   &= ~(val | ((*mask1 | *mask2) >> shift1));
	           *addr1   |= ((*sprite1++) << shift2);
	           *addr1++ |= (*sprite1 >> shift1);
	           *addr2   &= ~(val | ((*mask1 | *mask2) >> shift1));
	           *addr2   |= ((*sprite2++) << shift2);
	           *addr2++ |= (*sprite2 >> shift1);
	        }
	        *addr1 &= ~((*mask1 | *mask2) << shift2) | endmask;
	        *addr1 |= (*sprite1++ << shift2);
	        *addr2 &= ~((*mask1++ | *mask2++) << shift2) | endmask;
	        *addr2 |= (*sprite2++ << shift2);
	    }

    } else if(x + (bytewidth << 3) >= glbs->display_width) {
    	buffer = (x + (bytewidth << 3) - glbs->display_width) >> 3;

	    for (;h;h--,addr1+=lineoffset,addr2+=lineoffset) {
	        *addr1   &= ~((*mask1 | *mask2) >> shift1) | startmask;
	        *addr1++ |= *sprite1 >> shift1;
	        *addr2   &= ~((*mask1 | *mask2) >> shift1) | startmask;
	        *addr2++ |= *sprite2 >> shift1;
	        for (loop=1;loop<bytewidth-buffer;loop++) {
	           unsigned char val = (*mask1++ | *mask2++) << shift2;
	           *addr1   &= ~(val | ((*mask1 | *mask2) >> shift1));
	           *addr1   |= ((*sprite1++) << shift2);
	           *addr1++ |= (*sprite1 >> shift1);
	           *addr2   &= ~(val | ((*mask1 | *mask2) >> shift1));
	           *addr2   |= ((*sprite2++) << shift2);
	           *addr2++ |= (*sprite2 >> shift1);
	        }

	        sprite1 += buffer + 1;
	        sprite2 += buffer + 1;
	        mask1 += buffer + 1;
	        mask2 += buffer + 1;
	        addr1 += buffer;
	        addr2 += buffer;
	    }

	  } else {

	    for (;h;h--,addr1+=lineoffset,addr2+=lineoffset) {
	        *addr1   &= ~((*mask1 | *mask2) >> shift1) | startmask;
	        *addr1++ |= *sprite1 >> shift1;
	        *addr2   &= ~((*mask1 | *mask2) >> shift1) | startmask;
	        *addr2++ |= *sprite2 >> shift1;
	        for (loop=1;loop<bytewidth;loop++) {
	           unsigned char val = (*mask1++ | *mask2++) << shift2;
	           *addr1   &= ~(val | ((*mask1 | *mask2) >> shift1));
	           *addr1   |= ((*sprite1++) << shift2);
	           *addr1++ |= (*sprite1 >> shift1);
	           *addr2   &= ~(val | ((*mask1 | *mask2) >> shift1));
	           *addr2   |= ((*sprite2++) << shift2);
	           *addr2++ |= (*sprite2 >> shift1);
	        }
	        *addr1 &= ~((*mask1 | *mask2) << shift2) | endmask;
	        *addr1 |= (*sprite1++ << shift2);
	        *addr2 &= ~((*mask1++ | *mask2++) << shift2) | endmask;
	        *addr2 |= (*sprite2++ << shift2);
	    }
		}
}

void GraySpriteClipVFlipX8_MASK(
	short x,short y,short h, unsigned char* sprite1, short bytewidth, char flash)
{
    unsigned char *sprite2 = sprite1 + bytewidth * h;
    unsigned char *mask = sprite2 + bytewidth * h;

    long           offset;
    unsigned char* addr1;
    unsigned char* addr2;
    unsigned short shift1;
    unsigned short shift2;
    unsigned short lineoffset;
             short loop;
    unsigned char  startmask;
    unsigned char  endmask;
    short					b1 = bytewidth << 1;
     				 short buffer;

    if(x < -bytewidth * 8 + 1 || x >= glbs->display_width || y < -h || y >= glbs->display_hieght) return;

    sprite1 += bytewidth * (h - 1);
    sprite2 += bytewidth * (h - 1);
    mask += bytewidth * (h - 1);

    if(y < 0){
    	h += y;
    	sprite1 += (y * bytewidth);
    	sprite2 += (y * bytewidth);
    	mask += (y * bytewidth);
    	y = 0;
    } else if(y + h >= glbs->display_hieght)
    	h = glbs->display_hieght - y;

	  offset = (y<<5)-(y<<1)+(x>>3);
	  addr1  = ((unsigned char*)glbs->light_buffer)+offset;

	  if(flash) {
	  	addr2 = addr1;
	  	sprite2 = sprite1;
	  } else
	  	addr2  = ((unsigned char*)glbs->dark_buffer)+offset;

	  shift1 = x & 7;
	  lineoffset = 30-bytewidth;

	  shift2 = 8 - shift1;
	  startmask = 0xff << shift2;
	  endmask   = 0xff >> shift1;

	  if(x < 0) {
    	buffer = (-x) >> 3;
    	if((buffer << 3) != (-x)) buffer++;

	    for (;h;h--,addr1+=lineoffset,addr2+=lineoffset) {
	        sprite1 += buffer - 1;
	        sprite2 += buffer - 1;
	        mask += buffer - 1;
	        addr1 += buffer;
	        addr2 += buffer;

	        for (loop=buffer;loop<bytewidth;loop++) {
	           unsigned char val = (*mask++) << shift2;
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

	        sprite1 -= b1;
	        sprite2 -= b1;
	        mask -= b1;
	    }

    } else if(x + (bytewidth << 3) >= glbs->display_width) {
    	buffer = (x + (bytewidth << 3) - glbs->display_width) >> 3;

	    for (;h;h--,addr1+=lineoffset,addr2+=lineoffset) {
	        *addr1   &= ((*mask) >> shift1) | startmask;
	        *addr1++ |= *sprite1 >> shift1;
	        *addr2   &= ((*mask) >> shift1) | startmask;
	        *addr2++ |= *sprite2 >> shift1;
	        for (loop=1;loop<bytewidth-buffer;loop++) {
	           unsigned char val = (*mask++) << shift2;
	           *addr1   &= val | ((*mask) >> shift1);
	           *addr1   |= ((*sprite1++) << shift2);
	           *addr1++ |= (*sprite1 >> shift1);
	           *addr2   &= val | ((*mask) >> shift1);
	           *addr2   |= ((*sprite2++) << shift2);
	           *addr2++ |= (*sprite2 >> shift1);
	        }

	        sprite1 += buffer + 1;
	        sprite2 += buffer + 1;
	        mask += buffer + 1;
	        addr1 += buffer;
	        addr2 += buffer;

	        sprite1 -= b1;
	        sprite2 -= b1;
	        mask -= b1;
	    }

	  } else

	    for (;h;h--,addr1+=lineoffset,addr2+=lineoffset) {
	        *addr1   &= ((*mask) >> shift1) | startmask;
	        *addr1++ |= *sprite1 >> shift1;
	        *addr2   &= ((*mask) >> shift1) | startmask;
	        *addr2++ |= *sprite2 >> shift1;
	        for (loop=1;loop<bytewidth;loop++) {
	           unsigned char val = (*mask++) << shift2;
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

	        sprite1 -= b1;
	        sprite2 -= b1;
	        mask -= b1;
	    }
}
