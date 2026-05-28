// C Source File
// Created 5/4/02; 2:25:17 PM

/******************************************************************************
*
* project name:    ExtGraph
* file name:       unpack.c (originally part of the TIGCC Tools Suite)
* initial date:    24/02/2002 (integrated into ExtGraph)
* authors:         albert@cs.tut.fi
*                  b.denis@libertysurf.fr
*                  thomas.nussbaumer@gmx.net
*
*
* description:     unpack routine to exepack compression
*
* -----------------------------------------------------------------------------
*
* based on code from Pasi 'Albert' Ojala, albert@cs.tut.fi
* heavily reduced to fit to the needs by thomas.nussbaumer@gmx.net
* assembler optimizations by b.denis@libertysurf.fr
*
* for details on the used algorithm see:
*
* http://www.cs.tut.fi/~albert/Dev/pucrunch/index.html
*
******************************************************************************/
//#define ALL_CHECKS  --- just for testing purposes ...

#define USE_TI89
#define NO_EXIT_SUPPORT
#include <tigcclib.h>

typedef struct {
    unsigned char  osize_lo;   // original size lowbyte
    unsigned char  osize_hi;   // original size highbyte
    unsigned char  magic1;     // must be equal to UNPACK_MAGIC1
    unsigned char  magic2;     // must be equal to UNPACK_MAGIC2
    unsigned char  csize_lo;   // compressed size lowbyte
    unsigned char  csize_hi;   // compressed size lowbyte
    unsigned char  esc1;       // escape >> (8-escBits)
    unsigned char  notused3;
    unsigned char  notused4;
    unsigned char  esc2;       // escBits
    unsigned char  gamma1;     // maxGamma + 1
    unsigned char  gamma2;     // (1<<maxGamma)
    unsigned char  extralz;    // extraLZPosBits
    unsigned char  notused1;
    unsigned char  notused2;
    unsigned char  rleentries; // rleUsed
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

//-----------------------------------------------------------------------------
// All globals and functions are prefixed with "__" by the intention to
// prevent possible name conflicts in the global namespace
//-----------------------------------------------------------------------------
static unsigned char* __inputbuffer__  = 0; // input buffer
static unsigned short __inputmask__    = 0; // input buffer byte mask


//---------------------------------------------------------------------------
// __inputmask__ is initialized with 0x80 and gets shifted one bit to the
// right each time. CORRECT_IN_MASK will handle the shift right and if the
// mask becomes zero it will set it again to 0x80 + increments the
// inputbuffer pointer
//---------------------------------------------------------------------------
#define CORRECT_IN_MASK()  asm("ror.b  #1,%0;bcc.s 0f;addq.l #1,%1;0:"\
                               : "=d" (__inputmask__), "=g" (__inputbuffer__) \
                               : "0"  (__inputmask__), "1" (__inputbuffer__));

//---------------------------------------------------------------------------
// returns >0 if next bit is set, otherwise 0
//---------------------------------------------------------------------------
#define NEXT_BIT_SET   (*__inputbuffer__ & __inputmask__)


//=============================================================================
// get next 8 bits
//
// trashes a0,a1,d0,d1 (returns value in d0)
//=============================================================================
unsigned short __Get8Bit(void);
asm(".data\n"
"    .even\n"
"__Get8Bit:\n"
"    lea     __inputbuffer__,%a0\n"
"    movea.l (%a0),%a1\n"
"    clr.w   %d0\n"
"    move.b  (%a1)+,%d0\n"
"    move.l  %a1,(%a0)\n"
"    move.w  __inputmask__,%d1\n"
"    cmpi.b  #0x80,%d1\n"
"    bne.s   __inl_tries\n"
"    rts\n"
"__inl_tries:\n"
"    lsl.w   #8,%d0\n"
"    move.b  (%a1),%d0\n"
"    cmpi.b  #0x08,%d1\n"
"    bhi.s   __inl_try20\n"
"    bne.s   __inl_try02\n"
"    andi.w  #0x0fff,%d0\n"
"    lsr.w   #4,%d0\n"
"    rts\n"
"__inl_try40:\n"
"    andi.w  #0x7fff,%d0\n"
"    lsr.w   #7,%d0\n"
"    rts\n"
"__inl_try20:\n"
"    cmpi.b  #0x20,%d1\n"
"    bhi.s   __inl_try40\n"
"    bcs.s   __inl_try10\n"
"    andi.w  #0x3fff,%d0\n"
"    lsr.w   #6,%d0\n"
"    rts\n"
"__inl_try10:\n"
"    andi.w  #0x1fff,%d0\n"
"    lsr.w   #5,%d0\n"
"    rts\n"
"__inl_try04:\n"
"    andi.w  #0x07ff,%d0\n"
"    lsr.w   #3,%d0\n"
"    rts\n"
"__inl_try02:\n"
"    cmpi.b  #0x02,%d1\n"
"    bhi.s   __inl_try04\n"
"    bne.s   __inl_try01\n"
"    andi.w  #0x03ff,%d0\n"
"    lsr.w   #2,%d0\n"
"    rts\n"
"__inl_try01:\n"
"    andi.w  #0x01ff,%d0\n"
"    lsr.w   #1,%d0\n"
"    rts");

//=============================================================================
// get next value from the input buffer
//
// trashes a0,d0,d1,d2 (returns value in d0)
//=============================================================================
unsigned short __GetValue(void);
asm(".data\n"
"    .even\n"
"__GetValue:\n"
"    move.l  %d3,-(%sp)\n"
"    clr.l   %d1\n"
"    move.w  #7,%d0\n"
"    move.l  __inputbuffer__,%a0\n"
"    move.w  __inputmask__,%d2\n"
"__inl_loop1:\n"
"    cmp.w   %d1,%d0\n"
"    bls.s   __inl_exitloop\n"
"    move.w  %d2,%d3\n"
"    and.b   (%a0),%d3\n"
"    ror.b   #1,%d2\n"
"    bcc.s   __inl_samebyte1\n"
"    addq.l  #1,%a0\n"
"__inl_samebyte1:\n"
"    tst.b   %d3\n"
"    beq.s   __inl_exitloop\n"
"    addq.w  #1,%d1\n"
"    bra.s   __inl_loop1\n"
"__inl_exitloop:\n"
"    move.l  %a0,__inputbuffer__\n"
"    move.w  %d2,__inputmask__\n"
"    moveq   #1,%d3\n"
"    lsl.w   %d1,%d3\n"
"    move.w  %d1,-(%sp)\n"
"    bsr.s   __GetBits\n"
"    addq    #2,%sp\n"
"    or.w    %d3,%d0\n"
"    move.l  (%sp)+,%d3\n"
"    rts");

//=============================================================================
// gets a number of bits from the input buffer
//
// trashes a0,a1,d0,d1,d2 (returns value in d0)
//=============================================================================
unsigned short __GetBits(unsigned short bits);
asm(".data\n"
"    .even\n"
"__GetBits:\n"
"    clr.w   %d0\n"
"    move.w  4(%sp),%d1\n"
"    beq.s   __inl_exit\n"
"    movea.l __inputbuffer__,%a0\n"
"    movea.l %d3,%a1\n"
"    move.w  __inputmask__,%d2\n"
"    subq.w  #1,%d1\n"
"__inl_loop0:\n"
"    lsl.w   #1, %d0\n"
"    move.w  %d2,%d3\n"
"    and.b   (%a0),%d3\n"
"    beq.s   __inl_correct0\n"
"    addq.b  #1,%d0\n"
"__inl_correct0:\n"
"    ror.b   #1,%d2\n"
"    bcc.s   __inl_samebyte0\n"
"    addq.l  #1,%a0\n"
"__inl_samebyte0:\n"
"    dbra    %d1,__inl_loop0\n"
"    move.w  %d2,__inputmask__\n"
"    move.l  %a1,%d3\n"
"    move.l  %a0,__inputbuffer__\n"
"__inl_exit:\n"
"    rts");

//=============================================================================
// the decompression routine
//
// using it is very simple: feed in a filled source array and a buffer which is
// large enough to hold the decompression result
//
// returns 0 if okay
//=============================================================================
short UnpackBuffer(unsigned char *src, unsigned char *dest)  {
    long           startesc;
    char*          bytecodevec;
    __PACKHEADER*  cth = (__PACKHEADER*)src;
    unsigned short escbits8;
    unsigned short escbits;
    unsigned short extralzposbits;
    unsigned char* outbuffer;
#ifdef ALL_CHECKS
    unsigned char* pend_in;
    unsigned char* pend_out;
#endif

    //---------------------------------------------------------------------
    // check if the magic markers exists. if they are not present we cannot
    // decompress this type of file
    //---------------------------------------------------------------------
    if (cth->magic1 != __MAGIC_CHAR1 || cth->magic2 != __MAGIC_CHAR2) return __ERRPCK_NOMAGIC;

    //if (cth->gamma2 != 128 || cth->gamma1 != 7) return __ERRPCK_MAXGAMMA;

    startesc       = cth->esc1;
    escbits        = cth->esc2;
    extralzposbits = cth->extralz;
    escbits8       = 8 - escbits;

    if (escbits > 8)        return __ERRPCK_ESCBITS;
    if (extralzposbits > 4) return __ERRPCK_EXTRALZP;


    bytecodevec = &src[15];

    outbuffer = dest;
    //--------------------------
    // initialize buffer globals
    //--------------------------
    __inputbuffer__ = src + sizeof(__PACKHEADER) + cth->rleentries;   // points at start of data
    __inputmask__   = 0x80;


#ifdef ALL_CHECKS
    pend_in  = __inputbuffer__ + COMBINE_LOWHIGH(cth->csize_lo,cth->csize_hi);
    pend_out = dest            + COMBINE_LOWHIGH(cth->osize_lo,cth->osize_hi);
#endif

    while (1) {
        short sel = startesc;

#ifdef ALL_CHECKS
        if (outbuffer > pend_out)       return __ERRPCK_OUTBUFOVERRUN;
        if (__inputbuffer__  > pend_in) return __ERRPCK_NOESCFOUND;
#endif

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
#ifdef ALL_CHECKS
                        if (outbuffer > pend_out) return __ERRPCK_OUTBUFOVERRUN;
#endif
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

#ifdef ALL_CHECKS
            if (outbuffer + lzlen + 1 > pend_out) return __ERRPCK_OUTBUFOVERRUN;
#endif
            for (i=0; i<=lzlen; i++) {
                *outbuffer = *(outbuffer - lzpos - 1) + add;
                outbuffer++;
            }
        }
        else {
            *outbuffer++ = (sel<<escbits8) | __GetBits(escbits8);
#ifdef ALL_CHECKS
            if (outbuffer > pend_out) return __ERRPCK_OUTBUFOVERRUN;
#endif
        }
    }

    return __ERRPCK_OKAY;
}