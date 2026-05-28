// Big-endian load/store helpers for the word-based blitters.
//
// The framebuffer planes and the packed sprite/tile data are big-endian byte
// streams (byte 0 holds the leftmost 8 pixels, MSB first) -- that's how the
// m68k calculator laid them out. The original code accessed them with 16/32-bit
// `*(short*)`/`*(long*)` reads and writes, which only behaves correctly on a
// big-endian CPU. On a little-endian host we route those word accesses through
// these helpers so the byte order (and therefore pixel order) is preserved
// exactly, matching the calculator and the byte-wise SDL plane->texture blit.

#ifndef PORT_BITOPS_H
#define PORT_BITOPS_H

static inline unsigned long LD32(const void *p) {
    const unsigned char *b = (const unsigned char *)p;
    return ((unsigned long)b[0] << 24) | ((unsigned long)b[1] << 16) |
           ((unsigned long)b[2] << 8)  |  (unsigned long)b[3];
}
static inline void ST32(void *p, unsigned long v) {
    unsigned char *b = (unsigned char *)p;
    b[0] = (unsigned char)(v >> 24); b[1] = (unsigned char)(v >> 16);
    b[2] = (unsigned char)(v >> 8);  b[3] = (unsigned char)v;
}
static inline unsigned short LD16(const void *p) {
    const unsigned char *b = (const unsigned char *)p;
    return (unsigned short)((b[0] << 8) | b[1]);
}
static inline void ST16(void *p, unsigned short v) {
    unsigned char *b = (unsigned char *)p;
    b[0] = (unsigned char)(v >> 8); b[1] = (unsigned char)v;
}

#endif
