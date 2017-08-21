/* Bring in gd library functions */
#include "gd.h"

/* Bring in standard I/O so we can output the PNG to a file */
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
void ReadJpgFile(const char *filename, unsigned char *&buff, size_t &size)
{
  std::ifstream infile;
  infile.open(filename);
  infile.seekg(0, std::ios::end);
  size = infile.tellg();
  infile.seekg(0, std::ios::beg);
  buff = new unsigned char[size];
  infile.read((char *)buff, size);
  infile.close();
}
void WriteJpgFile(const char *filename, unsigned char *buff, size_t size)
{
  std::ofstream fout; // create an ofstream object named fout
  fout.open(filename,std::ios::trunc);
  fout.write((char *)buff,size);
  fout.close();
}
int main() {
  size_t insize = 0;
  int outsize = 0;
  unsigned char *in_buf = NULL;
  unsigned char *out_buf = NULL;
  ReadJpgFile("1.jpg",in_buf,insize);
  /* Declare the image */
  gdImagePtr im;
  im = gdImageCreateFromJpegPtr(insize,in_buf);
  gdImageInterlace(im, 1);
  out_buf = gdImageJpegPtr(im, &outsize, 75);


  WriteJpgFile("2.jpg",out_buf,outsize);
  /* Destroy the image in memory. */
  gdImageDestroy(im);
  gdFree(out_buf);
  delete in_buf;
}
