#ifndef __PNG_EXT_H__
#define __PNG_EXT_H__

#include <stdio.h>
#include <png.h> 
#include <stdlib.h>
/******************************图片数据*********************************/
typedef struct _pic_data pic_data;
struct _pic_data
{
 int width, height; /* 尺寸 */
 int bit_depth;  /* 位深 */
 int flag;   /* 一个标志，表示是否有alpha通道 */

 unsigned int *rgba; /* 图片数组 */
};
/**********************************************************************/
#define PNG_BYTES_TO_CHECK 4
#define HAVE_ALPHA 1
#define NO_ALPHA 0

#ifdef __cplusplus
extern "C" {
#endif


extern int read_png_file(const char *filepath, pic_data *out);
extern int write_png_file(const char *file_name , pic_data *graph);

#ifdef __cplusplus
}
#endif

#endif


