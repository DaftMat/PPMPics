/**
 *	included : stdio,stdlib,string,assert
 */
/**	Mathis MARGOT - 21606171
 *	Project title : Pictures
 *	Package : main
 *	File : main.c
 *
 *	Decription : embossing, contrasting, ppm to pgm. (only on ppm).
 *	Compilation : gcc -fopenmp main.c utils.c utils.h -o exeName
 */
#include "utils.h"
#include <omp.h>
#include <sys/time.h>

int h[256] = { 0 };
int c[256] = { 0 };
int numthreads = 8;

void ppm_to_pgm(color_image_type *img, grey_image_type *ret){
	#pragma omp for
	for (int i = 0 ; i < (img->width)*(img->height) ; ++i){
		unsigned char g = (299*img->pixels[i].r + 587*img->pixels[i].g + 114*img->pixels[i].b)/1000;
		ret->pixels[i] = g;
	}
}

void pgm_embossage(grey_image_type *img, grey_image_type *ret){
	unsigned char e;
	int unit = img->width;
	
	//calculate each new pixels
	//Difficult to parallelize, a lot of data
	for (int i = 0 ; i < img->height ; ++i){
		int ind = i*unit;
		#pragma omp for
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
}

void pgm_contrastage(grey_image_type *img, grey_image_type *ret){
	unsigned char e;

	//Calculate Histogram
	//Non-parallelisable
	// -> 2 threads could find the same pixel and
	// increment the same h[i].
	#pragma omp single
	{
	for (int i = 0 ; i < img->width*img->height ; ++i){
		h[img->pixels[i]]++;
	}
		
	//Calculate Cumulated histogram
	//Non-parallelisable either, same reason.
	for (int i = 1 ; i < 256 ; ++i){
		c[0] = h[0];
		c[i] = c[i-1]+h[i];
	}
	}
	//Note : let a lonely thread do 
	//that and broadcast H to all others
	#pragma omp wait

	//Calculate new picture
	//Parallelisable : no critical data.
	#pragma omp for
	for (int i = 0 ; i < img->width*img->height ; ++i){
		ret->pixels[i] = (256*c[img->pixels[i]])/(img->width*img->height);
	}
}

int main(int argc, char **argv){
	if (argc != 3){
		fprintf(stderr,"Usage : %s picture_in_path num_threads\n",argv[0]);
		exit(9);
	}
	double start, stop, startGlobal, stopGlobal;
	startGlobal = omp_get_wtime();

	color_image_type *in = loadColorImage(argv[1]);
	sscanf(argv[2], "%d", &numthreads);
	grey_image_type *outGrey = createGreyImage(in->width, in->height);
	grey_image_type *outEmbo = createGreyImage(in->width, in->height);
	grey_image_type *outCont = createGreyImage(in->width, in->height);

	start = omp_get_wtime();
	#pragma omp parallel num_threads(numthreads)
	{
	ppm_to_pgm(in, outGrey);
	pgm_embossage(outGrey, outEmbo);
	pgm_contrastage(outGrey, outCont);
	}
	stop = omp_get_wtime();

	//Save files
	saveGreyImage("resultGrey.pgm",outGrey);
	saveGreyImage("resultEmbosse.pgm",outEmbo);
	saveGreyImage("resultContraste.pgm",outCont);
	stopGlobal = omp_get_wtime();

	printf("Program execution time > %lfs with\n\t> Parallel time : \t%lfs\n\t> Sequential time : \t%lfs\n", stopGlobal-startGlobal, stop-start, (stopGlobal-startGlobal)-(stop-start));

	return 0;
}