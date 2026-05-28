// Native port replacement for TIGCC's <dll.h>.
// The DLL system existed only to swap code in/out of flash to save RAM on the
// calculator. Statically linked natively, so the loader is reduced to no-ops
// and the export/import indirection macros are unused (headers were converted
// to direct prototypes).

#ifndef PORT_DLL_H
#define PORT_DLL_H

#define DLL_OK 0

#define LoadDLL(name, id, major, minor) (DLL_OK)
#define UnloadDLL()                     ((void)0)

#endif
