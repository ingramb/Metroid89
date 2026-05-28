// Screen geometry shared by the SDL frontend and the graphics shim.
// The game renders into two 1-bpp planes with a fixed 30-byte row stride
// (240 px of addressable width); only the top-left 160x100 is shown on the 89.

#ifndef PORT_SCREEN_H
#define PORT_SCREEN_H

#define PLANE_STRIDE 30          // bytes per row in each plane
#define PLANE_ROWS   100         // rows in a plane buffer
#define PLANE_SIZE   3000        // PLANE_STRIDE * PLANE_ROWS

#define SCREEN_W 160             // visible width  (TI-89)
#define SCREEN_H 100             // visible height (TI-89)

int  screen_init(void);
void screen_present(const unsigned char *light, const unsigned char *dark);
void screen_quit(void);

#endif
