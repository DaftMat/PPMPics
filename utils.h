#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct color_pixel_struct {
    unsigned char r,g,b; 
} color_pixel_type;

typedef struct color_image_struct
{
  int width, height;
  color_pixel_type * pixels;
} color_image_type;

typedef struct grey_image_struct
{
  int width, height;
  unsigned char * pixels;
} grey_image_type;


/**********************************************************************/

color_image_type * loadColorImage(char *filename);

/**********************************************************************/

grey_image_type * createGreyImage(int width, int height);

/**********************************************************************/

void saveGreyImage(char * filename, grey_image_type *image);

