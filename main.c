/**
 *	included : stdio,stdlib,string,assert
 */
/**	Mathis MARGOT - 21606171
 *	Project title : Pictures
 *	Package : main
 *	File : main.c
 *
 *	Decription : embossing, contrasting, ppm to pgm. (only on ppm).
 *	Compilation : gcc main.c utils.c utils.h -o exeName
 */
#include "utils.h"

int h[256] = { 0 };
int c[256] = { 0 };

grey_image_type *ppm_to_pgm(color_image_type *img){
	grey_image_type *ret = createGreyImage(img->width,img->height);
	//Easily parallelizable
	for (int i = 0 ; i < (img->width)*(img->height) ; ++i){
		unsigned char g = (299*img->pixels[i].r + 587*img->pixels[i].g + 114*img->pixels[i].b)/1000;
		ret->pixels[i] = g;
	}
	return ret;
}

grey_image_type *pgm_embossage(grey_image_type *img){
	grey_image_type *ret = createGreyImage(img->width,img->height);
	unsigned char e;
	int unit = img->width;
	
	//calculate each new pixels
	//Difficult to parallelize, a lot of data
	for (int i = 0 ; i < img->height ; ++i){
		int ind = i*unit;
		for (int j = 0 ; j < img->width ; ++j){
			if (i == 0 || i == img->height-1 || j == 0 || j == img->width-1){
				e = img->pixels[ind+j];
			}else{
				e = 128 +
				img->pixels[ind-unit+j-1]*(-2) +//top left corner
				img->pixels[ind-unit+j]*(-1) +	//top
				img->pixels[ind+j-1]*(-1) +		//left
				img->pixels[ind+j+1] +			//right
				img->pixels[ind+unit+j] +		//bottom
				img->pixels[ind+unit+j+1]*2;	//bottom right corner
			}
			ret->pixels[ind+j] = e;
		}
	}
	return ret;
}

grey_image_type *pgm_contrastage(grey_image_type *img){
	grey_image_type *ret = createGreyImage(img->width,img->height);
	unsigned char e;

	//Calculate Histogram
	//Non-parallelisable
	// -> 2 threads could find the same pixel and
	// increment the same h[i].
	//PRAGMA OMP SINGLE {{
	for (int i = 0 ; i < img->width*img->height ; ++i){
		h[img->pixels[i]]++;
	}
		
	//Calculate Cumulated histogram
	//Non-parallelisable either, same reason.
	for (int i = 1 ; i < 256 ; ++i){
		c[0] = h[0];
		c[i] = c[i-1]+h[i];
	}
	//}}
	//Note : let a lonely thread do 
	//that and broadcast H to all others


	//Calculate new picture
	//Parallelisable : no critical data.
	for (int i = 0 ; i < img->width*img->height ; ++i){
		ret->pixels[i] = (256*c[img->pixels[i]])/(img->width*img->height);
	}

	return ret;
}

int main(int argc, char **argv){
	if (argc != 4){
		fprintf(stderr,"Usage : %s -g|-e|-c picture_in_path picture_out_path\n",argv[0]);
		exit(9);
	}

	color_image_type *in = loadColorImage(argv[2]);
	grey_image_type *temp = ppm_to_pgm(in);

	if (!strncmp("-g",argv[1],2)){
		saveGreyImage(argv[3],temp);
	}else if (!strncmp("-e",argv[1],2)){
		saveGreyImage(argv[3],pgm_embossage(temp));
	}else if (!strncmp("-c",argv[1],2)){
		saveGreyImage(argv[3],pgm_contrastage(temp));
	}else{
		perror("Argument error, expected -g, -e or -c");
		exit(8);
	}

	return 0;
}