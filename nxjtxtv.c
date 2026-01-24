// Simple UTF-8 Japanese text file viewer with Nano-X for ELKS
// 2024 T.Yamada
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <nano-X.h>

#define HAVEBLIT 0
#define FONT_NAME "misaki"
#define FONT_DIR "/lib/font/"
#define FILE_COUNT 12

typedef struct {
  unsigned int font_count;
  unsigned int firstchar;
  unsigned int size;
} font_header_t;

static GR_WINDOW_ID w1;
static GR_GC_ID gc1;
static GR_SCREEN_INFO si;
static GR_BITMAP bitmaptxt[16];
static GR_EVENT gr_eve;

static FILE *fptxt;
static long ftop;

static font_header_t font_header[FILE_COUNT];
static char __far *fontbmp[FILE_COUNT];
static int __far *fontoffset[FILE_COUNT];

static int text_col;
static int text_row;
static int dmode = 0;
static int detect_eof = 0;

/* print usage */
void printUsage(void)
{
  printf("Usage : nxjtxtv textfile\n");
  printf("-d : double hight, width mode\n");
}

/* print Unicode */
void printUni(unsigned long uni_c)
{
  char __far *getbmp;
  int __far *getoffset;
  int sbmp;

  for (int i = 0; i < FILE_COUNT; i++) {
    if ((uni_c >=  font_header[i].firstchar) && (uni_c <= (font_header[i].firstchar + font_header[i].size - 1))) {
      getoffset = fontoffset[i] + (uni_c - font_header[i].firstchar);
      getbmp = fontbmp[i] + *getoffset;
      if (dmode) {
        for (int j = 0; j < 16; j+=2) {
          sbmp = *getbmp;
          bitmaptxt[j] = 0;
          for (int k = 0; k < 8; k++) {
            bitmaptxt[j] <<= 2;
            if (sbmp & 0x80) {
              bitmaptxt[j] |= 3;
            }
            sbmp <<= 1;
          }
          bitmaptxt[j+1] = bitmaptxt[j];
          getbmp++;
        }
      }
      else {
        for (int j = 0; j < 8; j++) {
          bitmaptxt[j] = (*getbmp << 8);
          getbmp++;
        }
      }
      break;
    }
  }
}

/* print key message */
void printKeyM(void)
{
  char key_m[] = "n: next page, q : quit";
  int mi = 0;

  GrSetGCForeground(gc1, LTGRAY);
  GrSetGCBackground(gc1, BLACK);

  while (key_m[mi] != '\0') {
    printUni((unsigned long) key_m[mi]);
    if (dmode) {
      GrBitmap(w1, gc1, 16+mi*16, 16+(text_row+2)*16, 16, 16, bitmaptxt);
    }
    else {
      GrBitmap(w1, gc1, 8+mi*8, 8+(text_row+2)*8, 8, 8, bitmaptxt);
    }
    mi++;
  }

  GrSetGCForeground(gc1, BLACK);
  GrSetGCBackground(gc1, LTGRAY);
}

/* wait key for next page or quit*/
void waitKeyEvent(void)
{
  printKeyM();
  while (1) {
    GrGetNextEventTimeout(&gr_eve, GR_TIMEOUT_BLOCK);
    if (gr_eve.type == GR_EVENT_TYPE_KEY_DOWN) {
      if (gr_eve.keystroke.ch == 'q') {
        fclose(fptxt);
        GrClose();
        exit(0);
      }
      else if (gr_eve.keystroke.ch == 'n') {
        if (!detect_eof) {
          /* Next Page */
          ftop = ftell(fptxt);
          break;
        }
        else {
          fclose(fptxt);
          GrClose();
          exit(0);
        }
      }
    }
    else if (gr_eve.type == GR_EVENT_TYPE_CLOSE_REQ) {
      fclose(fptxt);
      GrClose();
      exit(0);
    }
    else if (gr_eve.type == GR_EVENT_TYPE_EXPOSURE) {
      /* Redraw */
      fseek(fptxt, ftop, SEEK_SET);
      break;
    }
  }
}

/* clear col */
void clearCol(int x, int y)
{
  while (x != text_col) {
    printUni(0x20);

    if (dmode) {
      GrBitmap(w1, gc1, 16+x*16, 16+y*16, 16, 16, bitmaptxt);
    }
    else {
      GrBitmap(w1, gc1, 8+x*8, 8+y*8, 8, 8, bitmaptxt);
    }
    x++;
  }
}

/* clear row */
void clearRow(int x, int y)
{
  while (y != text_row) {
    clearCol(x, y);
    x = 0;
    y++;
  }
}

int main(int argc, char **argv)
{
  char font_file[25];

  FILE *fpfont;
  FILE *fpfont_off;

  char __far *putbmp;
  int __far *putoffset;

  int utf8_c;
  unsigned long uni_c;

  int x = 0;
  int y = 0;

  /* Read Font files */
  for (int i = 0; i < FILE_COUNT; i++) {
    sprintf(font_file, "%s%x%s", FONT_NAME, i, ".bin");

    if (!(fpfont = fopen(font_file, "rb"))) {
      sprintf(font_file, "%s%s%x%s", FONT_DIR, FONT_NAME, i, ".bin");
      if (!(fpfont = fopen(font_file, "rb"))) {
        printf("Cannot open fonts %s\n", font_file);
        exit(1);
      }
    }

    sprintf(font_file, "%s%x%s", FONT_NAME, i, "o.bin");

    if (!(fpfont_off = fopen(font_file, "rb"))) {
      sprintf(font_file, "%s%s%x%s", FONT_DIR, FONT_NAME, i, "o.bin");
      if (!(fpfont_off = fopen(font_file, "rb"))) {
        printf("Cannot open fonts offsets %s\n", font_file);
        exit(1);
      }
    }

    font_header[i].font_count = getc(fpfont_off);
    font_header[i].font_count += (getc(fpfont_off) << 8);
    font_header[i].firstchar = getc(fpfont_off);
    font_header[i].firstchar += (getc(fpfont_off) << 8);
    font_header[i].size = getc(fpfont_off);
    font_header[i].size += (getc(fpfont_off) << 8);

    if (!(fontbmp[i] = fmemalloc(font_header[i].font_count*sizeof(char)*8))) {
      printf("Cannot allocate memory\n");
      exit(1);
    }
    if (!(fontoffset[i] = fmemalloc(font_header[i].size*sizeof(int)))) {
      printf("Cannot allocate memory\n");
      exit(1);
    }

    putbmp = fontbmp[i];

    for (int j = 0; j < font_header[i].font_count*8; j++) {
      *putbmp = getc(fpfont);
      putbmp++;
    }

    putoffset = fontoffset[i];
    for (int j = 0; j < font_header[i].size; j++) {
      *putoffset = getc(fpfont_off);
      *putoffset += (getc(fpfont_off) << 8);
      putoffset++;
    }

    fclose(fpfont);
    fclose(fpfont_off);

  }

  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
        if (argv[i][1] == 'd') {
          dmode = 1;
          if (argc == 2) {
            fptxt = stdin;
          }
        }
        else {
          printUsage();
          exit(1);
        }
      }
      else if (!(fptxt = fopen(argv[i], "r"))) {
        printf("Cannot open %s\n", argv[i]);
        exit(1);
      }
    }
  }
  else {
    fptxt = stdin;
  }

  ftop = ftell(fptxt);

  /* start nano-X */
  if (GrOpen() < 0) {
    exit(1);
  }

  GrGetScreenInfo(&si);

  w1 = GrNewWindowEx(GR_WM_PROPS_APPWINDOW, "nxjtxtv", GR_ROOT_WINDOW_ID, 8, 8, si.cols - 16, si.rows - 64, LTGRAY);

  if (dmode) {
    text_col = (si.cols - 16) / 16 - 2;
    text_row = (si.rows - 64) / 16 - 4;
  }
  else {
    text_col = (si.cols - 16) / 8 - 2;
    text_row = (si.rows - 64) / 8 - 4;
  }

  GrSelectEvents(w1, GR_EVENT_MASK_EXPOSURE | GR_EVENT_MASK_KEY_DOWN | GR_EVENT_MASK_CLOSE_REQ);

  //GdHideCursor();

  GrMapWindow(w1);

  gc1 = GrNewGC();

  GrSetGCForeground(gc1, BLACK);
  GrSetGCBackground(gc1, LTGRAY);


  while (1) {

    if ((utf8_c = getc(fptxt)) == EOF) {
      detect_eof = 1;
      clearRow(x, y);
      waitKeyEvent();
      y = 0;
      continue;
    }

    /* Convert UTF-8 to Unicode */
    if ((utf8_c & 0xF0) == 0xF0) {
      uni_c = ((unsigned long) (utf8_c & 0x07)) << 18;
      uni_c += ((unsigned long) (getc(fptxt) & 0x3F)) << 12;
      uni_c += ((unsigned long) (getc(fptxt) & 0x3F)) << 6;
      uni_c += (unsigned long) (getc(fptxt) & 0x3F);
    }
    else if ((utf8_c & 0xE0) == 0xE0) {
      uni_c = ((unsigned long) (utf8_c & 0x0F)) << 12;
      uni_c += ((unsigned long) (getc(fptxt) & 0x3F)) << 6;
      uni_c += (unsigned long) (getc(fptxt) & 0x3F);
    }
    else if ((utf8_c & 0xC0) == 0xC0) {
      uni_c = ((unsigned long) (utf8_c & 0x1F)) << 6;
      uni_c += (unsigned long) (getc(fptxt) & 0x3F);
    }
    else {
      uni_c = (unsigned long) utf8_c;
    }

    if (uni_c == '\n') { // Line Feed
      clearCol(x, y);
      x = 0;
      y++;
      if (y == text_row) {
        waitKeyEvent();
        y = 0;
      }
    }
    else if (uni_c == '\r') { // Carriage Return
      //x = 0;
    }
    else {
      if (uni_c == '\t') { // Replace tab with space
        uni_c = 0x20;
      }

      /* print Text */
      printUni(uni_c);

      if (dmode) {
        GrBitmap(w1, gc1, 16+x*16, 16+y*16, 16, 16, bitmaptxt);
      }
      else {
        GrBitmap(w1, gc1, 8+x*8, 8+y*8, 8, 8, bitmaptxt);
      }

      if (x < text_col-1) {
        x++;
      }
      else {
        x = 0;
        y++;
        if (y == text_row) {
          waitKeyEvent();
          y = 0;
        }
      }
    }
  }

  return 0;
}
