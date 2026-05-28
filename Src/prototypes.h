void ClipSprite8_OR_R( short x, short y, short h, char *sprt, void *dest);
void ClipSprite8_XOR_R( short x, short y, short h, char *sprt, void *dest);
void ClipSprite8_AND_R( short x, short y, short h, char *sprt, void *dest);
void ClipSprite8_MASK_R( short x, short y, short h, char *sprt, char *mask, void *dest);
void ClipSprite8_BLIT_R( short x, short y, short h, char *sprt, char mask, void *dest);

void ClipSprite16_OR_R( short x, short y, short h, short *sprt, void *dest);
void ClipSprite16_XOR_R( short x, short y, short h, short *sprt, void *dest);
void ClipSprite16_AND_R( short x, short y, short h, short *sprt, void *dest);
void ClipSprite16_MASK_R( short x, short y, short h, short *sprt, short *mask, void *dest);
void ClipSprite16_BLIT_R( short x, short y, short h, short *sprt, short mask, void *dest);

void ClipSprite32_OR_R( short x, short y, short h, long *sprt, void *dest);
void ClipSprite32_XOR_R( short x, short y, short h, long *sprt, void *dest);
void ClipSprite32_AND_R( short x, short y, short h, long *sprt, void *dest);
void ClipSprite32_MASK_R( short x, short y, short h, long *sprt, long *mask, void *dest);
void ClipSprite32_BLIT_R( short x, short y, short h, long *sprt, long mask, void *dest);
