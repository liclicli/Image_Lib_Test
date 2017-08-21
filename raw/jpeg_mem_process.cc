#include <stdio.h>
#include <string.h>
extern "C" {
#include <jpeglib.h>
}
#define MaxBufferExtent  8192
typedef struct{
    UINT8* ImgBuffer;
    long BufferSize;
    long pos;
}BUFF_JPG;
typedef struct {
  struct jpeg_source_mgr manager;   /* public fields */
  BUFF_JPG jpg;     /* jpeg image buffer */
  JOCTET * buffer;      /* start of buffer */
  boolean start_of_blob;    /* have we gotten any data yet? */
} SourceManager;
typedef struct {
  struct jpeg_destination_mgr manager;   /* public fields */
  BUFF_JPG jpg;     /* jpeg image buffer */
  JOCTET * buffer;      /* start of buffer */
  size_t * size;
} DestinationManager;
void InitializeSource(j_decompress_ptr cinfo)
{
  SourceManager *source;
  source=(SourceManager *) cinfo->src;
  source->start_of_blob=TRUE;
}
static boolean FillInputBuffer(j_decompress_ptr cinfo)
{
  SourceManager *source;
  source=(SourceManager *) cinfo->src;
  size_t nbytes;
  if(source->jpg.ImgBuffer == NULL || source->jpg.pos >= source->jpg.BufferSize){
    nbytes = -1;
  }
  else {
    nbytes = (source->jpg.pos + MaxBufferExtent > source->jpg.BufferSize ?   \
    source->jpg.BufferSize - source->jpg.pos : MaxBufferExtent);
    memcpy(source->buffer, source->jpg.ImgBuffer + source->jpg.pos, nbytes);
    source->jpg.pos += nbytes;
    source->manager.bytes_in_buffer=nbytes;
  }
  if (source->manager.bytes_in_buffer == 0)
  {
    source->buffer[0]=(JOCTET) 0xff;
    source->buffer[1]=(JOCTET) JPEG_EOI;
    source->manager.bytes_in_buffer=2;
  }
  source->manager.next_input_byte=source->buffer;
  source->start_of_blob=FALSE;
  return(TRUE);
}
void SkipInputData(j_decompress_ptr cinfo,long number_bytes)
{
  SourceManager *source;
  if (number_bytes <= 0) return;
  source=(SourceManager *) cinfo->src;
  while (number_bytes > (long) source->manager.bytes_in_buffer)
  {
    number_bytes-=(long) source->manager.bytes_in_buffer;
    (void) FillInputBuffer(cinfo);
  }
  source->manager.next_input_byte+=number_bytes;
  source->manager.bytes_in_buffer-=number_bytes;
}
void TerminateSource(j_decompress_ptr cinfo)
{
  return ;
}
void JpegSrcMgr(j_decompress_ptr cinfo,unsigned char *data,size_t size)
{
  SourceManager *source;
  cinfo->src=(struct jpeg_source_mgr *) (*cinfo->mem->alloc_small)
    ((j_common_ptr) cinfo,JPOOL_PERMANENT,sizeof(SourceManager));
  source=(SourceManager *) cinfo->src;
  source->buffer=(JOCTET *) (*cinfo->mem->alloc_small)
    ((j_common_ptr) cinfo,JPOOL_PERMANENT,MaxBufferExtent *sizeof(JOCTET));
  source=(SourceManager *) cinfo->src;
  source->manager.init_source=InitializeSource;
  source->manager.fill_input_buffer=FillInputBuffer;
  source->manager.skip_input_data=SkipInputData;
  source->manager.resync_to_restart=jpeg_resync_to_restart;
  source->manager.term_source=TerminateSource;
  source->jpg.ImgBuffer = data;
  source->jpg.BufferSize = size;
  source->jpg.pos = 0;
  source->manager.bytes_in_buffer=0;
  source->manager.next_input_byte=NULL;
}

boolean EmptyOutputBuffer(j_compress_ptr cinfo)
{
  DestinationManager *destination;
  destination=(DestinationManager *) cinfo->dest;
  memcpy(destination->jpg.ImgBuffer+*(destination->size),destination->buffer,MaxBufferExtent);
  destination->manager.free_in_buffer = MaxBufferExtent;
  destination->manager.next_output_byte = destination->buffer;
  //destination->jpg.pos+=MaxBufferExtent;
  //destination->jpg.BufferSize+=MaxBufferExtent;
  *(destination->size)+= MaxBufferExtent;
  return(TRUE);
}

void InitializeDestination(j_compress_ptr cinfo)
{
  DestinationManager *destination;
  destination=(DestinationManager *) cinfo->dest;
  destination->buffer=(JOCTET *) (*cinfo->mem->alloc_small)
    ((j_common_ptr) cinfo,JPOOL_IMAGE,MaxBufferExtent*sizeof(JOCTET));
  destination->manager.next_output_byte=destination->buffer;
  destination->manager.free_in_buffer=MaxBufferExtent;
  destination->jpg.pos=0;
  destination->jpg.BufferSize=0;
  *(destination->size) = 0;
}
void TerminateDestination(j_compress_ptr cinfo)
{
  DestinationManager *destination;
  destination=(DestinationManager *) cinfo->dest;
  if ((MaxBufferExtent-(int) destination->manager.free_in_buffer) > 0)
    {
      memcpy(destination->jpg.ImgBuffer+*(destination->size),destination->buffer,(ssize_t)
          (MaxBufferExtent-destination->manager.free_in_buffer));
      *(destination->size)+= (ssize_t)(MaxBufferExtent-destination->manager.free_in_buffer);
      destination->manager.free_in_buffer = MaxBufferExtent;
    }
}
void JpegDestMgr(j_compress_ptr cinfo,unsigned char *data,size_t &size)
{
  DestinationManager *destination;
  cinfo->dest=(struct jpeg_destination_mgr *) (*cinfo->mem->alloc_small)
    ((j_common_ptr) cinfo,JPOOL_PERMANENT,sizeof(DestinationManager));
  destination=(DestinationManager *) cinfo->dest;
  destination->manager.init_destination=InitializeDestination;
  destination->manager.empty_output_buffer=EmptyOutputBuffer;
  destination->manager.term_destination=TerminateDestination;
  destination->jpg.ImgBuffer = data;
  destination->size = &size;
}
