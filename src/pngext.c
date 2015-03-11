#include "pngext.h"

int read_png_file(const char *filepath, pic_data *out)
/* 用于解码png图片 */
{
 FILE *pic_fp;
 pic_fp = fopen(filepath, "rb");
 if(pic_fp == NULL) /* 文件打开失败 */
  return -1;
 
 /* 初始化各种结构 */
 png_structp png_ptr;
 png_infop   info_ptr;
 char        buf[PNG_BYTES_TO_CHECK];
 int         temp;
 
 png_ptr  = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
 info_ptr = png_create_info_struct(png_ptr);
 
 setjmp(png_jmpbuf(png_ptr)); // 这句很重要
 
 temp = fread(buf,1,PNG_BYTES_TO_CHECK,pic_fp);
 temp = png_sig_cmp((png_bytep)buf, (png_size_t)0, PNG_BYTES_TO_CHECK);
 
 /*检测是否为png文件*/
 if (temp!=0) return 1;
 
 rewind(pic_fp);
 /*开始读文件*/
 png_init_io(png_ptr, pic_fp);
 // 读文件了
 png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);

 int color_type,channels;
 
 /*获取宽度，高度，位深，颜色类型*/
 channels       = png_get_channels(png_ptr, info_ptr); /*获取通道数*/
 out->bit_depth = png_get_bit_depth(png_ptr, info_ptr); /* 获取位深 */
 color_type     = png_get_color_type(png_ptr, info_ptr); /*颜色类型*/
 
 int i,j;
 int size, pos = 0;
 /* row_pointers里边就是rgba数据 */
 png_bytep* row_pointers;
 row_pointers = png_get_rows(png_ptr, info_ptr);
 out->width = png_get_image_width(png_ptr, info_ptr);
 out->height = png_get_image_height(png_ptr, info_ptr);
 
 size = out->width * out->height; /* 计算图片的总像素点数量 */ 

 if(channels == 4 || color_type == PNG_COLOR_TYPE_RGB_ALPHA)
 {/*如果是RGB+alpha通道，或者RGB+其它字节*/ 
  size *= (4*sizeof(unsigned char)); /* 每个像素点占4个字节内存 */
  out->flag = HAVE_ALPHA;    /* 标记 */
  out->rgba = (unsigned int*) malloc(size);
  if(out->rgba == NULL)
  {/* 如果分配内存失败 */
   fclose(pic_fp);
   puts("错误(png):无法分配足够的内存供存储数据!");
   return 1;
  }

  temp = (4 * out->width);/* 每行有4 * out->width个字节 */
  for(i = 0; i < out->height; i++)
  {
   for(j = 0; j < temp; j += 4)
   {/* 一个字节一个字节的赋值 */
	char* p = (char*)&out->rgba[pos];
    p[0] = row_pointers[i][j]; // red
    p[1] = row_pointers[i][j+1]; // green
    p[2] = row_pointers[i][j+2];   // blue
    p[3] = row_pointers[i][j+3]; // alpha
    ++pos;
   }
  }
 }
 else 
 {
	 return 1;
 }
 
 /* 撤销数据占用的内存 */
 png_destroy_read_struct(&png_ptr, &info_ptr, 0); 
 return 0;
}



int write_png_file(const char *file_name , pic_data *graph)
/* 功能：将LCUI_Graph结构中的数据写入至png文件 */
{
 int j, i, temp, pos;
 png_byte color_type; 

 png_structp png_ptr;
 png_infop info_ptr; 
 png_bytep * row_pointers;
 /* create file */
 FILE *fp = fopen(file_name, "wb");
 if (!fp)
 {
  printf("[write_png_file] File %s could not be opened for writing", file_name);
  return -1;
 }


 /* initialize stuff */
 png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

 if (!png_ptr)
 {
  printf("[write_png_file] png_create_write_struct failed");
  return -1;
 }
 info_ptr = png_create_info_struct(png_ptr);
 if (!info_ptr)
 {
  printf("[write_png_file] png_create_info_struct failed");
  return -1;
 }
 if (setjmp(png_jmpbuf(png_ptr)))
 {
  printf("[write_png_file] Error during init_io");
  return -1;
 }
 png_init_io(png_ptr, fp);


 /* write header */
 if (setjmp(png_jmpbuf(png_ptr)))
 {
  printf("[write_png_file] Error during writing header");
  return -1;
 }
 /* 判断要写入至文件的图片数据是否有透明度，来选择色彩类型 */
 if(graph->flag == HAVE_ALPHA) color_type = PNG_COLOR_TYPE_RGB_ALPHA;
 else color_type = PNG_COLOR_TYPE_RGB;
 
 png_set_IHDR(png_ptr, info_ptr, graph->width, graph->height,
  graph->bit_depth, color_type, PNG_INTERLACE_NONE,
  PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

 png_write_info(png_ptr, info_ptr);


 /* write bytes */
 if (setjmp(png_jmpbuf(png_ptr)))
 {
  printf("[write_png_file] Error during writing bytes");
  return -1;
 }
 if(graph->flag == HAVE_ALPHA) temp = (4 * graph->width);
 else temp = (3 * graph->width);
 
 pos = 0;
 row_pointers = (png_bytep*)malloc(graph->height*sizeof(png_bytep));
 for(i = 0; i < graph->height; i++)
 {
  row_pointers[i] = (png_bytep)malloc(sizeof(unsigned char)*temp);
  for(j = 0; j < temp; j += 4)
  {
   char* p = (char*)&graph->rgba[pos];
   row_pointers[i][j]   = p[0]; // red
   row_pointers[i][j+1] = p[1]; // green
   row_pointers[i][j+2] = p[2];   // blue
   if(graph->flag == HAVE_ALPHA) 
    row_pointers[i][j+3] = p[3]; // alpha
   ++pos;
  }
 }
 png_write_image(png_ptr, row_pointers);

 /* end write */
 if (setjmp(png_jmpbuf(png_ptr)))
 {
  printf("[write_png_file] Error during end of write");
  return -1;
 }
 png_write_end(png_ptr, NULL);

    /* cleanup heap allocation */
 for (j=0; j<graph->height; j++)
  free(row_pointers[j]);
 free(row_pointers);

    fclose(fp);
    return 0;
}



