#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <pthread.h>
#include <wand/magick_wand.h>
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
  static int i = 0;
  if(i > 0) return ;
  i++;
  std::ofstream fout; // create an ofstream object named fout
  fout.open(filename,std::ios::trunc);
  fout.write((char *)buff,size);
  fout.close();
}
void CompressJpgUsingImageMagick(unsigned char *orig_buff, unsigned char *&buff, size_t &img_size)
{
  MagickWand *im = NULL;
  im = NewMagickWand();
  MagickReadImageBlob(im, (const unsigned char *)orig_buff, img_size);
  MagickResetIterator(im);
  MagickSetImageCompressionQuality(im, 75);
  double factors420[3] = {2.0, 1.0, 1.0};
  MagickSetSamplingFactors(im, 3, factors420);
  MagickSetInterlaceScheme(im, LineInterlace);
  MagickSetImageFormat(im, "jpg");
  MagickStripImage(im);
  if(buff != NULL) free(buff);
  img_size = 0;
  buff = (unsigned char *)MagickWriteImageBlob(im, &img_size);
  DestroyMagickWand(im);
}
void* WholeJpegCompress(void*)
{
  unsigned char *orig_buff = NULL;
  unsigned char *buff = NULL;
  size_t ori_img_size = 0;
  size_t img_size = 0;
  for(int i = 0; i < 1; i++)
  {
    ReadJpgFile("1.jpg",orig_buff,ori_img_size);
    for(int j = 0; j < 100; j++)
    {
      img_size = ori_img_size;
      if(buff != NULL)
      {
        free(buff);
        buff = NULL;
      }
      CompressJpgUsingImageMagick(orig_buff,buff,img_size);
    }
    WriteJpgFile("2.jpg",buff,img_size);
    free(buff);
    buff = NULL;
    delete orig_buff;
    orig_buff = NULL;
  }
  pthread_exit(0);
  return NULL;
}
void* EmptyRun(void*)
{
  int i = 0;
  while(i < 1000000000)
  {
    int j = 0;
    while(j < 100) j++;
    i++;
  }
  pthread_exit(0);
}
int main()
{
  pthread_t deal_thr[4];
  for(int i = 0; i < 4; i ++)
  {
    int ret=pthread_create(&deal_thr[i],NULL,WholeJpegCompress,NULL);
    if(ret!=0)
    {
      printf("Create pthread error!\n");
      return -1;
    }
  }
  for(int i = 0; i < 4; i ++)
  {
    pthread_join(deal_thr[i],NULL);
  }
  return 0;
}
