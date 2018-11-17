#include "utils.h"

  /**********************************************************************/

color_image_type * loadColorImage(char *filename){
  int i, width,height,max_value;
  char format[8];
  color_image_type * image;
  FILE * f = fopen(filename,"r");
  if (!f){
    fprintf(stderr,"Cannot open file %s...\n",filename);
    exit(-1);
  }
  fscanf(f,"%s\n",format);
  assert( (format[0]=='P' && format[1]=='3'));  // check P3 format
  while(fgetc(f)=='#') // commentaire
    {
      while(fgetc(f) != '\n'); // aller jusqu'a la fin de la ligne
    }
  fseek( f, -1, SEEK_CUR);
  fscanf(f,"%d %d\n", &width, &height);
  fscanf(f,"%d\n", &max_value);
  image = malloc(sizeof(color_image_type));
  assert(image != NULL);
  image->width = width;
  image->height = height;
  image->pixels = malloc(width*height*sizeof(color_pixel_type));
  assert(image->pixels != NULL);
  
  for(i=0 ; i<width*height ; i++){
      int r,g,b;
      fscanf(f,"%d %d %d", &r, &g, &b);
      image->pixels[i].r = (unsigned char) r;
      image->pixels[i].g = (unsigned char) g;
      image->pixels[i].b = (unsigned char) b;
    }
  fclose(f);
  return image;
}

/**********************************************************************/

grey_image_type * createGreyImage(int width, int height){
  grey_image_type * image = malloc(sizeof(grey_image_type));
  assert(image != NULL);
  image->width = width;
  image->height = height;
  image->pixels = malloc(width*height*sizeof(unsigned char));
  assert(image->pixels != NULL);
  return(image);
}

/**********************************************************************/

void saveGreyImage(char * filename, grey_image_type *image){
  int i;
  FILE * f = fopen(filename,"w");
  if (!f){
    fprintf(stderr,"Cannot open file %s...\n",filename);
    exit(-1);
  }
  fprintf(f,"P2\n%d %d\n255\n",image->width,image->height);
  for(i=0 ; i<image->width*image->height ; i++){
    fprintf(f,"%d\n",image->pixels[i]);
  }
  fclose(f);
}

