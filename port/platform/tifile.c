// Native replacement for the AMS symbol-table / heap "memory mapping".
//
// On the calculator the game located data variables (gfx, tiles, mtlevel, ...)
// with SymFind, locked their heap handle, and read the bytes in place. Here we
// load the matching .89z/.89y files from disk once and hand back pointers.
//
// TI single-variable file framing (verified across all bundled files):
//   0x00  "**TI89**" sig, folder, comment
//   0x3A  u16 LE  number of entries (always 1 here)
//   0x3C  entry: u32 LE dataoff, 8-byte name, 1-byte type, ...
//   dataoff      : 4 bytes (0) then u16 BE var-data size then the var data
//   So the heap image ([u16 size][var data]) begins at dataoff+4, and
//   file_pointer() (HeapDeref+2) returns dataoff+6.

#include "tigcc_compat.h"
#include <dirent.h>

#ifndef DATA_DIR
#define DATA_DIR "."
#endif

typedef struct {
    char           name[16];
    unsigned char *buf;       // whole file image (owned)
    long           buflen;
    unsigned char *image;     // heap image: [u16 BE size][var data...]
    SYM_ENTRY      sym;        // returned by SymFindPtr; .handle == (HANDLE)image
    int            used;
} Var;

#define MAX_VARS 64
static Var  g_vars[MAX_VARS];
static int  g_nvars = 0;
static int  g_loaded = 0;

static Var *var_find(const char *name) {
    int i;
    for (i = 0; i < g_nvars; i++)
        if (g_vars[i].used && strcmp(g_vars[i].name, name) == 0)
            return &g_vars[i];
    return 0;
}

static unsigned char *read_file(const char *path, long *out_len) {
    FILE *f = fopen(path, "rb");
    unsigned char *buf;
    long n;
    if (!f) return 0;
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    fseek(f, 0, SEEK_SET);
    buf = (unsigned char *)malloc(n);
    if (buf && fread(buf, 1, n, f) != (size_t)n) { free(buf); buf = 0; }
    fclose(f);
    if (out_len) *out_len = n;
    return buf;
}

// Parse one TI variable file and register its internal variable by name.
static void register_ti_file(const char *path) {
    long len = 0;
    unsigned char *buf = read_file(path, &len);
    unsigned int nent, dataoff;
    Var *v;
    if (!buf || len < 0x40) { free(buf); return; }

    nent = buf[0x3A] | (buf[0x3B] << 8);
    if (nent < 1) { free(buf); return; }

    // First entry only (all bundled files hold a single variable).
    dataoff = buf[0x3C] | (buf[0x3D] << 8) | (buf[0x3E] << 16) | (buf[0x3F] << 24);
    if (dataoff + 6 > (unsigned long)len) { free(buf); return; }

    {
        char nm[16];
        memcpy(nm, &buf[0x40], 8); nm[8] = 0;
        if (var_find(nm)) { free(buf); return; }   // already have it (e.g. native .raw)
    }
    if (g_nvars >= MAX_VARS) { free(buf); return; }
    v = &g_vars[g_nvars++];
    memset(v, 0, sizeof(*v));
    memcpy(v->name, &buf[0x40], 8);
    v->name[8] = 0;
    v->buf = buf;
    v->buflen = len;
    v->image = buf + dataoff + 4;       // [u16 BE size][var data]
    v->sym.handle = (HANDLE)v->image;
    strncpy(v->sym.name, v->name, sizeof(v->sym.name) - 1);
    v->used = 1;
}

// Register a raw native blob (e.g. mtlevel.raw produced by mapconv) as a
// variable. These are the var-data directly, so we prepend a 2-byte size
// slot to fit the [size][data] heap-image convention; file_pointer() returns
// the data start.
static void register_raw_file(const char *path, const char *varname) {
    long len = 0;
    unsigned char *data = read_file(path, &len);
    Var *v;
    if (!data) return;
    if (var_find(varname)) { free(data); return; }
    if (g_nvars >= MAX_VARS) { free(data); return; }
    v = &g_vars[g_nvars++];
    memset(v, 0, sizeof(*v));
    v->buf = malloc(len + 2);
    v->buflen = len + 2;
    memcpy(v->buf + 2, data, len);
    free(data);
    strncpy(v->name, varname, sizeof(v->name) - 1);
    v->image = v->buf;                  // [2-byte slot][data]; file_pointer -> +2
    v->sym.handle = (HANDLE)v->image;
    strncpy(v->sym.name, varname, sizeof(v->sym.name) - 1);
    v->used = 1;
}

static void ensure_loaded(void) {
    DIR *d;
    struct dirent *e;
    if (g_loaded) return;
    g_loaded = 1;
    d = opendir(DATA_DIR);
    if (!d) { fprintf(stderr, "tifile: cannot open data dir '%s'\n", DATA_DIR); return; }
    // Pass 1: native .raw blobs take precedence (e.g. mtlevel.raw over timdemo2.89y).
    while ((e = readdir(d))) {
        const char *dot = strrchr(e->d_name, '.');
        if (dot && strcmp(dot, ".raw") == 0) {
            char path[1024], var[16];
            int n = (int)(dot - e->d_name);
            if (n >= (int)sizeof(var)) n = sizeof(var) - 1;
            memcpy(var, e->d_name, n); var[n] = 0;
            snprintf(path, sizeof(path), "%s/%s", DATA_DIR, e->d_name);
            register_raw_file(path, var);
        }
    }
    rewinddir(d);
    // Pass 2: the bundled TI variable files.
    while ((e = readdir(d))) {
        const char *dot = strrchr(e->d_name, '.');
        if (!dot) continue;
        if (strcmp(dot, ".89z") == 0 || strcmp(dot, ".89y") == 0) {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", DATA_DIR, e->d_name);
            register_ti_file(path);
        }
    }
    closedir(d);
}

// --- AMS symbol table ---
HSym SymFind(const char *name) {
    HSym h = {0, 0};
    ensure_loaded();
    if (var_find(name)) h.folder = 1;
    return h;
}

SYM_ENTRY *SymFindPtr(const char *name, short flags) {
    Var *v;
    (void)flags;
    ensure_loaded();
    v = var_find(name);
    return v ? &v->sym : 0;
}

// Create (or find) a writable variable -- used by the save game.
HSym SymAdd(const char *name) {
    HSym h = {0, 0};
    Var *v;
    ensure_loaded();
    v = var_find(name);
    if (!v && g_nvars < MAX_VARS) {
        v = &g_vars[g_nvars++];
        memset(v, 0, sizeof(*v));
        strncpy(v->name, name, sizeof(v->name) - 1);
        strncpy(v->sym.name, name, sizeof(v->sym.name) - 1);
        v->used = 1;
    }
    if (v) h.folder = (unsigned short)(v - g_vars + 1);
    return h;
}

SYM_ENTRY *DerefSym(HSym h) {
    if (h.folder == 0 || h.folder > (unsigned short)g_nvars) return 0;
    return &g_vars[h.folder - 1].sym;
}

void ti_unlock_file(const char *name) {
    SYM_ENTRY *e = SymFindPtr(name, 0);
    if (e) HeapUnlock(e->handle);
}

// --- AMS heap. Handles are just pointers (file images or malloc blocks). ---
void  *HeapDeref(HANDLE h)        { return (void *)h; }
void  *HLock(HANDLE h)            { return (void *)h; }
short  HeapLock(HANDLE h)         { (void)h; return 1; }
short  HeapUnlock(HANDLE h)       { (void)h; return 1; }
short  HeapGetLock(HANDLE h)      { (void)h; return 1; }
HANDLE HeapPtrToHandle(void *p)   { return (HANDLE)p; }
HANDLE HeapAlloc(unsigned long s) { return (HANDLE)malloc(s ? s : 1); }
HANDLE HeapAllocHigh(unsigned long s) { return (HANDLE)malloc(s ? s : 1); }
void   HeapFree(HANDLE h)         { free((void *)h); }
HANDLE HeapRealloc(HANDLE h, unsigned long s) { return (HANDLE)realloc((void *)h, s ? s : 1); }
unsigned long HeapMax(void)       { return 256UL * 1024; }
unsigned long HeapAvail(void)     { return 256UL * 1024; }
void   HeapCompress(void)         {}

// --- ExtGraph TTArchive container ---
// Layout (big-endian): "tta\0" magic, u16 count, count * 16-byte records of
// [u16 offset][u16 size][12 bytes], offsets relative to the post-table data.
static unsigned short be16(const unsigned char *p) {
    return (unsigned short)((p[0] << 8) | p[1]);
}

short ttarchive_valid(void *archive) {
    const unsigned char *a = (const unsigned char *)archive;
    return a && a[0] == 't' && a[1] == 't' && a[2] == 'a' && a[3] == 0;
}

void *ttarchive_data(void *archive, unsigned short index) {
    unsigned char *a = (unsigned char *)archive;
    unsigned short count = be16(a + 4);
    unsigned char *table = a + 6;
    unsigned char *data  = table + (unsigned long)count * 16;
    unsigned short offset;
    if (index >= count) return 0;
    offset = be16(table + (unsigned long)index * 16);
    return data + offset;
}
