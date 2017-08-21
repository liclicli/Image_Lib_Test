#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <fstream>
#include <iostream>
extern "C" {
#include <jpeglib.h>
}
#define MAX_JPEG_FILE_LEN 84200
#define MAX_THREAD_NUM 4
extern void JpegSrcMgr(j_decompress_ptr cinfo,unsigned char *data,size_t size);
extern void JpegDestMgr(j_compress_ptr cinfo,unsigned char *data,size_t &size);

void WriteJpgFile(const char *filename, unsigned char *buff, size_t size)
{
  std::ofstream fout; // create an ofstream object named fout
  fout.open(filename,std::ios::trunc);
  fout.write((char *)buff,size);
  fout.close();
}

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

int read_and_write_JPEG_mem(unsigned char *&infile_buffer, size_t infile_buffer_size,
                            unsigned char * outfile_buffer,size_t &size,int quality)
{
  int image_width = 0;
  int image_height = 0;
  int num_components = 0;
  unsigned char * image_buffer;
  J_COLOR_SPACE jpeg_color_space = JCS_YCbCr;
  struct jpeg_decompress_struct cinfo_r;
  struct jpeg_error_mgr jerr_r;
  struct jpeg_compress_struct cinfo_w;
  struct jpeg_error_mgr jerr_w;
  memset(&cinfo_w, 0, sizeof(jpeg_compress_struct));
  int row_stride;
  cinfo_r.err = jpeg_std_error(&jerr_r);
  jpeg_create_decompress(&cinfo_r);
  JpegSrcMgr(&cinfo_r, infile_buffer, infile_buffer_size);
  jpeg_read_header(&cinfo_r, TRUE);
  cinfo_r.out_color_space = JCS_RGB;
  //cinfo_r.out_color_space = cinfo_r.jpeg_color_space;
  jpeg_start_decompress(&cinfo_r);
  image_width = cinfo_r.output_width;
  image_height = cinfo_r.output_height;
  num_components = cinfo_r.num_components;
  jpeg_color_space = cinfo_r.out_color_space;
  row_stride = cinfo_r.output_width * cinfo_r.output_components;
  image_buffer = new unsigned char [cinfo_r.output_width*cinfo_r.output_components*cinfo_r.output_height];
  JSAMPROW row_pointer[1];
  cinfo_w.err = jpeg_std_error(&jerr_w);
  jpeg_create_compress(&cinfo_w);
  JpegDestMgr(&cinfo_w,outfile_buffer,size);
  cinfo_w.image_width = image_width;
  cinfo_w.image_height = image_height;
  cinfo_w.input_components = num_components;
  cinfo_w.in_color_space = jpeg_color_space;
  jpeg_set_defaults(&cinfo_w);
  cinfo_w.optimize_coding = TRUE;
  //cinfo_w.dct_method=JDCT_ISLOW;
  jpeg_set_quality(&cinfo_w, quality, TRUE);
  if (cinfo_w.jpeg_color_space == JCS_YCbCr)
  {
        cinfo_w.comp_info[0].h_samp_factor = 2;
        cinfo_w.comp_info[0].v_samp_factor = 2;
  }
  jpeg_simple_progression(&cinfo_w);
  jpeg_start_compress(&cinfo_w, TRUE);
  while (cinfo_r.output_scanline < cinfo_r.output_height)
  {
    row_pointer[0] = &image_buffer[cinfo_r.output_scanline*row_stride];
    jpeg_read_scanlines(&cinfo_r,row_pointer,1);
    jpeg_write_scanlines(&cinfo_w, row_pointer, 1);
  }
  jpeg_finish_decompress(&cinfo_r);
  jpeg_destroy_decompress(&cinfo_r);
  jpeg_finish_compress(&cinfo_w);
  jpeg_destroy_compress(&cinfo_w);
  delete image_buffer;
  return 1;
}
void *WholeRawJpegCompress(void *)
{
  size_t insize = 0;
  size_t outsize = 0;
  unsigned char *infile_buffer;
  unsigned char *outfile_buffer;
  outfile_buffer = NULL;
  ReadJpgFile("1.jpg",infile_buffer,insize);
  for(int i = 0; i < 100; i++)
  {
    if(outfile_buffer != NULL) delete outfile_buffer;
    outfile_buffer = new unsigned char[MAX_JPEG_FILE_LEN];
    outsize = 0;
    //if(read_JPEG_mem(infile_buffer,insize)==1) write_JPEG_mem(outfile_buffer,outsize,75);
    read_and_write_JPEG_mem(infile_buffer,insize,outfile_buffer,outsize,75);
  }
  WriteJpgFile("2.jpg",outfile_buffer,outsize);
  //printf("outsize: %d\n",outsize);
  delete outfile_buffer;
  delete infile_buffer;
}
int main()
{
  pthread_t deal_thr[MAX_THREAD_NUM];
  for(int i = 0; i < MAX_THREAD_NUM; i ++)
  {
    int ret=pthread_create(&deal_thr[i],NULL,WholeRawJpegCompress,NULL);
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
