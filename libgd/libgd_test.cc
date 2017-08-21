/* Bring in gd library functions */
#include "gd.h"

/* Bring in standard I/O so we can output the PNG to a file */
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <pthread.h>
#define MAX_THREAD_NUM 4
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
void* run_test(void *) {
  size_t insize = 0;
  int outsize = 0;
  unsigned char *in_buf = NULL;
  unsigned char *out_buf = NULL;
  ReadJpgFile("1.jpg",in_buf,insize);
  /* Declare the image */
  static
  for (int i = 0; i < 100; i++){
    gdImagePtr im;
    im = gdImageCreateFromJpegPtr(insize,in_buf);
    gdImageInterlace(im, 1);
    out_buf = (unsigned char *)gdImageJpegPtr(im, &outsize, 75);
    gdImageDestroy(im);
    if(i == 99) WriteJpgFile("2.jpg",out_buf,outsize); 
    gdFree(out_buf);
  }
  delete in_buf;
  pthread_exit(0);
  return NULL;
}
int main(){
  pthread_t deal_thr[MAX_THREAD_NUM];
  for(int i = 0; i < MAX_THREAD_NUM; i ++)
  {
    int ret=pthread_create(&deal_thr[i],NULL,run_test,NULL);
    if(ret!=0)
    {
      printf("Create pthread error!\n");
      return -1;
    }
  }
  for(int i = 0; i < MAX_THREAD_NUM; i ++)
  {
    pthread_join(deal_thr[i],NULL);
  }
  return 0;
}
