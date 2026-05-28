void ClipSprite8_OR_R( short x asm("%d0"), short y asm("%d1"), short h asm("%d2"), char *sprt asm("%a1"), void *dest asm("%a0"));
void ClipSprite8_XOR_R( short x asm("%d0"), short y asm("%d1"), short h asm("%d2"), char *sprt asm("%a1"), void *dest asm("%a0"));
void ClipSprite8_AND_R( short x asm("%d0"), short y asm("%d1"), short h asm("%d2"), char *sprt asm("%a1"), void *dest asm("%a0"));
void ClipSprite8_MASK_R( short x asm("%d0"), short y asm("%d1"), short h asm("%d2"), char *sprt asm("%a1"), char *mask asm("%a2"), void *dest asm("%a0"));
void ClipSprite8_BLIT_R( short x asm("%d0"), short y asm("%d1"), short h asm("%d2"), char *sprt asm("%a1"), char mask asm("%d3"), void *dest asm("%a0"));

void ClipSprite16_OR_R( short x asm("%d0"), short y asm("%d1"), short h asm("%d2"), short *sprt asm("%a1"), void *dest asm("%a0"));
void ClipSprite16_XOR_R( short x asm("%d0"), short y asm("%d1"), short h asm("%d2"), short *sprt asm("%a1"), void *dest asm("%a0"));
void ClipSprite16_AND_R( short x asm("%d0"), short y asm("%d1"), short h asm("%d2"), short *sprt asm("%a1"), void *dest asm("%a0"));
void ClipSprite16_MASK_R( short x asm("%d0"), short y asm("%d1"), short h asm("%d2"), short *sprt asm("%a1"), short *mask asm("%a2"), void *dest asm("%a0"));
void ClipSprite16_BLIT_R( short x asm("%d0"), short y asm("%d1"), short h asm("%d2"), short *sprt asm("%a1"), short mask asm("%d3"), void *dest asm("%a0"));

void ClipSprite32_OR_R( short x asm("%d0"), short y asm("%d1"), short h asm("%d2"), long *sprt asm("%a1"), void *dest asm("%a0"));
void ClipSprite32_XOR_R( short x asm("%d0"), short y asm("%d1"), short h asm("%d2"), long *sprt asm("%a1"), void *dest asm("%a0"));
void ClipSprite32_AND_R( short x asm("%d0"), short y asm("%d1"), short h asm("%d2"), long *sprt asm("%a1"), void *dest asm("%a0"));
void ClipSprite32_MASK_R( short x asm("%d0"), short y asm("%d1"), short h asm("%d2"), long *sprt asm("%a1"), long *mask asm("%a2"), void *dest asm("%a0"));
void ClipSprite32_BLIT_R( short x asm("%d0"), short y asm("%d1"), short h asm("%d2"), long *sprt asm("%a1"), long mask asm("%d3"), void *dest asm("%a0"));
