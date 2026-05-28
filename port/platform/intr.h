// Native port replacement for TIGCC's <intr.h>.
// The original used auto-interrupts (AUTO_INT_1/3/5) for game timing/FPS.
// Natively, timer_input.c drives those counters from an SDL timer; the
// SetIntVec/GetIntVec machinery is reduced to bookkeeping no-ops.

#ifndef PORT_INTR_H
#define PORT_INTR_H

typedef void (*INT_HANDLER)(void);

#define AUTO_INT_COUNT 7
#define FIRST_AUTO_INT 1
#define LAST_AUTO_INT  7
#define AUTO_INT(n)    (n)
#define AUTO_INT_1     1
#define AUTO_INT_2     2
#define AUTO_INT_3     3
#define AUTO_INT_5     5

#define DUMMY_HANDLER  ((INT_HANDLER)0)

// DEFINE_INT_HANDLER(name) { ... } defines a plain function on native.
#define DEFINE_INT_HANDLER(name) void name(void)

INT_HANDLER GetIntVec(short n);
void        SetIntVec(short n, INT_HANDLER h);
void        ExecuteHandler(INT_HANDLER h);

#endif
