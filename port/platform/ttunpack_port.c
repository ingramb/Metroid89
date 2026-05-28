// C port of the ttunpack (pucrunch) decompressor from Src/unpack.c.
// UnpackBuffer() below is the original C routine, verbatim; only the bit-reader
// helpers (__Get8Bit/__GetValue/__GetBits) and the CORRECT_IN_MASK/NEXT_BIT_SET
// macros -- which were hand-written m68k assembly -- are reimplemented in C.
//
// Algorithm: Pasi 'Albert' Ojala's pucrunch.  http://www.cs.tut.fi/~albert/Dev/pucrunch/

typedef struct {
    unsigned char  osize_lo, osize_hi;   // original (decompressed) size
    unsigned char  magic1, magic2;       // 0x54 0x50
    unsigned char  csize_lo, csize_hi;   // compressed size
    unsigned char  esc1;                 // escape >> (8-escBits)
    unsigned char  notused3, notused4;
    unsigned char  esc2;                 // escBits
    unsigned char  gamma1, gamma2;
    unsigned char  extralz;              // extraLZPosBits
    unsigned char  notused1, notused2;
    unsigned char  rleentries;           // number of RLE byte-code entries
} __PACKHEADER;

#define __MAGIC_CHAR1          0x54
#define __MAGIC_CHAR2          0x50

#define __ERRPCK_OKAY             0
#define __ERRPCK_NOESCFOUND     248
#define __ERRPCK_ESCBITS        249
#define __ERRPCK_MAXGAMMA       250
#define __ERRPCK_EXTRALZP       251
#define __ERRPCK_NOMAGIC        252
#define __ERRPCK_OUTBUFOVERRUN  253
#define __ERRPCK_LZPOSUNDERRUN  254

#define COMBINE_LOWHIGH(lo,hi)  (((unsigned short)lo) | ((unsigned short)(hi << 8)));

// --- bit-reader state (MSB-first within each byte) ---
static unsigned char *__inputbuffer__;
static unsigned char  __inputmask__;   // one-hot mask, 0x80 = next bit

// m68k "ror.b #1,mask; if carry advance buffer": the bit rotated out of bit0
// (i.e. mask was 0x01) wraps to bit7 and steps to the next input byte.
#define CORRECT_IN_MASK() do {                                  \
        unsigned char _carry = __inputmask__ & 1;               \
        __inputmask__ = (unsigned char)((__inputmask__ >> 1) |  \
                                        (__inputmask__ << 7));  \
        if (_carry) __inputbuffer__++;                          \
    } while (0)

#define NEXT_BIT_SET (*__inputbuffer__ & __inputmask__)

// Read `bits` bits, MSB-first, as an unsigned value.
static unsigned short __GetBits(unsigned short bits) {
    unsigned short v = 0;
    while (bits--) {
        v <<= 1;
        if (NEXT_BIT_SET) v |= 1;
        CORRECT_IN_MASK();
    }
    return v;
}

// Read the next 8 bits (== __GetBits(8); the original asm was just an optimization).
static unsigned short __Get8Bit(void) {
    return __GetBits(8);
}

// pucrunch gamma code: count up to 7 leading 1-bits -> n, then value = (1<<n) | next n bits.
static unsigned short __GetValue(void) {
    unsigned short n = 0;
    while (n < 7) {
        unsigned short bit = NEXT_BIT_SET ? 1 : 0;
        CORRECT_IN_MASK();
        if (!bit) break;
        n++;
    }
    return (unsigned short)((1u << n) | __GetBits(n));
}

//=============================================================================
// the decompression routine (original C from Src/unpack.c, unchanged)
//=============================================================================
short UnpackBuffer(unsigned char *src, unsigned char *dest) {
    long           startesc;
    char*          bytecodevec;
    __PACKHEADER*  cth = (__PACKHEADER*)src;
    unsigned short escbits8;
    unsigned short escbits;
    unsigned short extralzposbits;
    unsigned char* outbuffer;

    if (cth->magic1 != __MAGIC_CHAR1 || cth->magic2 != __MAGIC_CHAR2) return __ERRPCK_NOMAGIC;

    startesc       = cth->esc1;
    escbits        = cth->esc2;
    extralzposbits = cth->extralz;
    escbits8       = 8 - escbits;

    if (escbits > 8)        return __ERRPCK_ESCBITS;
    if (extralzposbits > 4) return __ERRPCK_EXTRALZP;

    bytecodevec = (char*)&src[15];

    outbuffer = dest;
    __inputbuffer__ = src + sizeof(__PACKHEADER) + cth->rleentries;
    __inputmask__   = 0x80;

    while (1) {
        short sel = startesc;

        if (escbits) sel = __GetBits(escbits);

        if (sel == startesc) {
            unsigned short lzpos, lzlen = __GetValue(), i;
            unsigned short add = 0;

            if (lzlen != 1) {
                unsigned short lzposhi = __GetValue()-1, lzposlo;

                if (lzposhi == 254) {
                    if (lzlen > 3) {
                        add   = __Get8Bit();
                        lzpos = __Get8Bit() ^ 0xff;
                    }
                    else {
                        break;  // finish !!!
                    }
                }
                else {
                    if (extralzposbits) lzposhi = (lzposhi<<extralzposbits) | __GetBits(extralzposbits);

                    lzposlo = __Get8Bit() ^ 0xff;
                    lzpos   = COMBINE_LOWHIGH(lzposlo,lzposhi);
                }
            }
            else {
                if (NEXT_BIT_SET) {
                    unsigned short rlelen, bytecode, byte;

                    CORRECT_IN_MASK();

                    if (!NEXT_BIT_SET) {
                        unsigned short newesc;
                        CORRECT_IN_MASK();

                        newesc = __GetBits(escbits);

                        *outbuffer++ = (startesc<<escbits8) | __GetBits(escbits8);
                        startesc = newesc;
                        continue;
                    }
                    CORRECT_IN_MASK();
                    rlelen = __GetValue();
                    if (rlelen >= 128) {
                        rlelen = ((rlelen-128)<<1) | __GetBits(1);
                        rlelen |= ((__GetValue()-1)<<8);
                    }
                    bytecode = __GetValue();
                    if (bytecode < 32) byte = bytecodevec[bytecode];
                    else               byte = ((bytecode-32)<<3) | __GetBits(3);

                    for (i=0; i<=rlelen; i++) *outbuffer++ = byte;
                    continue;
                }
                CORRECT_IN_MASK();
                lzpos = __Get8Bit() ^ 0xff;
            }

            for (i=0; i<=lzlen; i++) {
                *outbuffer = *(outbuffer - lzpos - 1) + add;
                outbuffer++;
            }
        }
        else {
            *outbuffer++ = (sel<<escbits8) | __GetBits(escbits8);
        }
    }

    return __ERRPCK_OKAY;
}

// The game's `decompress` indirection now maps directly to UnpackBuffer.
unsigned short decompress(unsigned char *src, unsigned char *dest) {
    return (unsigned short)UnpackBuffer(src, dest);
}
