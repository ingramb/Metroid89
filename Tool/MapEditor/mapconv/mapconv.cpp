// Headless converter: reads a MapEditor .mmp project and writes the game's
// native-endian map blobs (mtlevel.raw, mtlevel1.raw, ...) by reusing the
// editor's own export_data() with flip_short() neutralized (MAPCONV_NATIVE).
//
//   mapconv <project.mmp> <output_dir>

#include <allegro.h>
#include <stdio.h>
#include "tiles.h"
#include "enemy.h"
#include "doors.h"
#include "screen.h"
#include "map.h"

// Allegro globals the editor modules reference (unused headlessly).
BITMAP *screen = 0;
FONT   *font   = 0;
char    key[256] = {0};
int     mouse_b = 0;

int main(int argc, char **argv)
{
    if (argc < 3) { fprintf(stderr, "usage: mapconv <project.mmp> <out_dir>\n"); return 2; }

    FILE *f = fopen(argv[1], "rb");
    if (!f) { fprintf(stderr, "mapconv: cannot open %s\n", argv[1]); return 1; }

    map m;
    m.load_data(f);
    fclose(f);

    m.export_data(argv[2]);
    return 0;
}
