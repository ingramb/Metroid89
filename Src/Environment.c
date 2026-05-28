// C Source File
// Created 8/28/02; 4:52:05 PM

#include <tigcclib.h>         // Include All Header Files
#include "utility.h"
#include "entity.h"
#include "player.h"
#include "map.h"
#include "environment.h"
#include "miscgfx.h"

#define WATER_ANIM_DELAY 9

char environment_type;

char water_frame;
char water_anim_direction;
char water_anim_counter;
char water_direction;
short water_counter;
short water_speed;
short water_speed_max;
short water_top;
short water_min;
short water_max;
short water_cnt;
char water_disable;

/*---------------------------------------------------------------------------*/
/* speedy pixel access routines                                              */
/*---------------------------------------------------------------------------*/
#define PIXOFFSET(x,y)  ((y<<5)-(y<<1)+(x>>3))
#define PIXADDR(p,x,y)  (((unsigned char*)(p))+PIXOFFSET(x,y))
#define PIXMASK(x)      ((unsigned char)(0x80 >> ((x)&7)))

#define SETPIX(a,m)   (*(a) |= (m)) // uses precalculated values now
#define CLRPIX(a,m)   (*(a) &= ~(m))
#define XORPIX(a,m)   (*(a) ^= (m))
#define PIXUP(a)      ((a)-=30)
#define PIXDOWN(a)    ((a)+=30)
#define PIXLEFT(a,m)   asm("rol.b  #1,%0;bcc.s  0f;subq.l #1,%1;0:"\
                          : "=d" (m), "=g" (a) : "0" (m), "1" (a))
#define PIXRIGHT(a,m)  asm("ror.b  #1,%0;bcc.s  0f;addq.l #1,%1;0:"\
                          : "=d" (m), "=g" (a) : "0" (m), "1" (a))


/*---------------------------------------------------------------------------*/
/* useful macros                                                             */
/*---------------------------------------------------------------------------*/
#define ABS(a)          (((a)<0) ? -(a): (a))
#define SPECIAL_SGN(a)  (((a)<0) ? 0 : 1)

/*inline void draw_drop(short x1, short y1)
{
    short d,x,y,ax,ay,sx,sy,dx,dy;
    
    short x2 = x1 + 5;
    short y2 = y1 + 10;

    register unsigned char *pixaddr1;
    register unsigned char *pixaddr2;
    register unsigned char pixmask1;
    register unsigned char pixmask2;
    
    if(x2 > 159) {
    	y2 -= (x2 - 160) >> 1;
    	x2 = 159;
    }
    if(y2 > 99) {
    	x2 -= (y2 - 100) >> 1;
    	y2 = 100;
    }

    dx = x2 - x1;
    ax = ABS(dx)<<1;
    sx = SPECIAL_SGN(dx);

    dy = y2 - y1;
    ay = ABS(dy)<<1;
    sy = SPECIAL_SGN(dy);

    x = x1;
    y = y1;

    pixaddr1=PIXADDR(light_buffer, x, y);
    pixaddr2=PIXADDR(dark_buffer, x, y);
    pixmask1=PIXMASK(x);
    pixmask2=PIXMASK(x);
    
        d = ax - (ay>>1);
        do {
            CLRPIX(pixaddr1,pixmask1);
            CLRPIX(pixaddr2,pixmask2);
            if (y==y2) return;
            if (d>=0) {
                if (sx) {
                	PIXRIGHT(pixaddr1, pixmask1);
                	PIXRIGHT(pixaddr2, pixmask2);
                } else {
                	PIXLEFT(pixaddr1, pixmask1);
                	PIXLEFT(pixaddr2, pixmask2);
                }
                
                d -= ay;
            }
            if(sy) {
            	PIXDOWN(pixaddr1);
            	PIXDOWN(pixaddr2);
            	y++;
            } else {
            	PIXUP(pixaddr2);
            	PIXUP(pixaddr2);
            	y--;
            }
            d += ax;
        } while(1);
}*/

void draw_rain()
{
	//short max = random(10) + 10;
	//register short i;
	//register short x;
	//register short y;
	
	//for(i = max ; i >= 0 ; i--) {
	//	x = random(160);
	//	y = random(100);
		//draw_drop(x, y);
	//}
}


#define ASM_SWAP(val) ({\
    register unsigned long tmp = val;\
    asm volatile ("swap %0" : "=d" (tmp) : "0" (tmp));\
    tmp;})

/*===========================================================================*/
/* draws a horizontal line from (x1,y) to (x2,y) in given plane              */
/*===========================================================================*/
void draw_spotlight_strip(short x1, short x2, short y) {
    static const unsigned short table1[16] = {0xffff,0x7fff,0x3fff,0x1fff,
                                              0x0fff,0x07ff,0x03ff,0x01ff,
                                              0x00ff,0x007f,0x003f,0x001f,
                                              0x000f,0x0007,0x0003,0x0001};
    static const unsigned short table2[16] = {0x0000,0x8000,0xc000,0xe000,
                                              0xf000,0xf800,0xfc00,0xfe00,
                                              0xff00,0xff80,0xffc0,0xffe0,
                                              0xfff0,0xfff8,0xfffc,0xfffe};
    short           dx;
    short           sx;
    unsigned short* p1;
    unsigned short* p2;
    
    if(y < 0 || y >= display_hieght) return;
    
    if(x1 < 0) x1 = 0;
    else if(x1 >= display_width) return;
    
    if(x2 < 0) return;
    else if(x2 >= display_width) x2 = display_width - 1;

    p1  = (unsigned short*)(light_buffer+(y<<5)-(y<<1)+((x1>>3)&0x1e));
    p2  = (unsigned short*)(dark_buffer+(y<<5)-(y<<1)+((x1>>3)&0x1e));
    
    dx = x2-x1+1;
    if(dx < 1) return;
    sx = x1 & 0x000f;

    if (dx<16) {
        unsigned long val = (ASM_SWAP(table2[dx])) >> sx;
        *(unsigned long*)p1 = (*(unsigned long*)p1 & ~val) | (~*(unsigned long*)p2 & val);
        *(unsigned long*)p2 |= val;
        
        return;
    }

    if (sx) {
        *p1 = (*p1 & ~table1[sx]) | (~*p2 & table1[sx]); p1++;
        *p2++ |= table1[sx];
        dx -= (16-sx);
    }
    while (dx >= 16) {
        *p1++ = ~*p2;
        *p2++ |= 0xffff;
        dx-=16;
    }
    if (dx) {
    	*p1 = (*p1 & ~table2[dx]) | (~*p2 & table2[dx]);
    	*p2 |= table2[dx];
    }
}

void draw_spotlight(short xc, short yc, short r) 
{ 
	register short x; 
	register short y = r; 
	register short p = 3 - 2 * r;
	register long *addr1 = light_buffer;
	register long *addr2 = dark_buffer;
  
  for(x = 0 ; x < yc - r ; x++) {
  	*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;
  	*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;
  	*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;
  	*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;
  	*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;
  	*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;
  	
  	(char *)addr1 += 6;
  	(char *)addr2 += 6;
  }
  
  x = 0;
  
	while (x <= y) 
	{    
		draw_spotlight_strip(0, xc - y, yc + x); draw_spotlight_strip(xc + y, display_width - 1, yc + x);
		if(x != 0) {
			draw_spotlight_strip(0, xc - y, yc - x);
			draw_spotlight_strip(xc + y, display_width - 1, yc - x);
		}
		
		if (p < 0) {
			p += 4 * x++ + 6;

		} else {
		
			if(x != y) {
				draw_spotlight_strip(0, xc - x, yc + y); draw_spotlight_strip(xc + x, display_width - 1, yc + y);
				draw_spotlight_strip(0, xc - x, yc - y); draw_spotlight_strip(xc + x, display_width - 1, yc - y);
			}
			
			p += 4 * (x++ - y--) + 10;
		}
	}
	
	x = yc + r + 1;
	
	addr1 = light_buffer + 30 * x;
	addr2 = dark_buffer + 30 * x;
	
	for( ; x < display_hieght ; x++) {
		*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;
		*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;
		*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;
		*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;
		*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;
		*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;
		
  	(char *)addr1 += 6;
  	(char *)addr2 += 6;
  }
}

void draw_water()
{
	long addr;
	long sprite;
	short x, h;
	short y_pos;
	short cnt2 = 31 - water_cnt;
	
	for(x = 0 ; x < 5 ; x++) {
		y_pos = water_top - camera.y;
		if(y_pos < 0) y_pos = -((-y_pos) & 31);
		addr = (long)(light_buffer + 4 * x);
		if(y_pos > 0) addr += y_pos * 30;
		
		for(;y_pos < 100 ; y_pos += 32) {
			sprite = (long)(water_gfx + (water_frame << 7));
			if(y_pos != water_top - camera.y) sprite += (4 << 7);
			
			if(y_pos < 0) {
				sprite -= y_pos * 4;
				h = 32 + y_pos;
			} else if(y_pos + 32 >= 100) h = 100 - y_pos;
			else h = 32;
			
			for(;h;h--) {
				*(unsigned long *)addr ^= (*(unsigned long *)sprite) << water_cnt;
				*(unsigned long *)addr ^= (*(unsigned long *)sprite) >> cnt2;
				sprite += 4;
				addr += 30;
			}
		}
	}
}

void environment_setup(char type, short min, short max, short speed)
{
	environment_type = type;
	water_frame = 0;
	water_anim_direction = 1;
	water_anim_counter = 0;
	water_direction = 1;
	water_top = water_min = min;
	water_max = max;
	water_counter = 0;
	water_speed = water_speed_max = speed;
	water_cnt = 0;
	if(water_min == water_max && water_min == current_map.hieght * 12) water_disable = TRUE;
	else water_disable = FALSE;
}

void environment_process()
{
	short upper = water_min + 10;
	short lower = water_max - 10;
	short delta = water_speed_max >> 4;
	
	if(water_disable) return;
	
	if(delta < 1) delta = 1;
	
	water_anim_counter++;
	if(water_anim_counter == WATER_ANIM_DELAY) {
		water_frame += water_anim_direction;
		if(water_frame == 3) water_anim_direction = -1;
		else if(water_frame == 0) water_anim_direction = 1;
		water_anim_counter = 0;
		water_cnt = (water_cnt + 1) & 31;
	}
	
	if(water_min == water_max) return;
	
	water_counter += water_speed;
	while(water_counter >= 100) {
		water_counter -= 100;
		
		if((water_direction == 1 && water_top > lower) || (water_direction == -1 && water_top < upper))
			water_speed -= delta;
		else if(water_speed < water_speed_max)
			water_speed += delta;
		
		water_top += water_direction;
		if(water_top == water_max) {water_direction = -1; break;}
		else if(water_top == water_min) {water_direction = 1; break;}
	}
	
}

void environment_draw()
{
	if(environment_type == ENVIRONMENT_RAIN) draw_rain();
	else if(environment_type == ENVIRONMENT_DARK)
		draw_spotlight(player.e.x + player.e.size->x_off + player.e.size->width / 2 - camera.x,
		player.e.y + player.e.size->y_off + player.e.size->hieght / 2 - camera.y, 50);
		
	if(!water_disable) draw_water();
}
