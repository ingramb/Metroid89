// C Header File
// Created 11/19/01; 8:16:02 PM

void draw_char(void *buffer, char buffer_width, int x, int y, char c, short mode);
void draw_string(short x, short y, char *text);
int text_length(char *text);
char text_hieght(char *text);
char init_small_font();
void cleanup_small_font();
void draw_small_char(void *buffer, char buffer_width, int x, int y, char c, short mode);

extern unsigned char font[];
extern char font_width[];
extern unsigned char *small_font;
extern char small_font_width[];
