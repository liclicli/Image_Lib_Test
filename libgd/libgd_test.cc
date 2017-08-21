/* Bring in gd library functions */
#include "gd.h"

/* Bring in standard I/O so we can output the PNG to a file */
#include <stdio.h>

int main() {
  /* Declare the image */
  gdImagePtr im;
  /* Declare output files */
  FILE *jpegout;
  /* Declare color indexes */
  if(gdSupportsFileType("1.jpg",0)) im = gdImageCreateFromFile("1.jpg");


  /* Do the same for a JPEG-format file. */
  jpegout = fopen("2.jpg", "wb");


  /* Output the same image in JPEG format, using the default
    JPEG quality setting. */
  gdImageInterlace(im, 1);
  gdImageJpeg(im, jpegout, 75);

  /* Close the files. */
  fclose(jpegout);

  /* Destroy the image in memory. */
  gdImageDestroy(im);
}
