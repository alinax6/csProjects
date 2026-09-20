#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <a2plain.h>
#include <a2methods.h>
#include <pnm.h>
#include <math.h>
#include <except.h>

// calculates the squared difference of pixel values between 2 images
double pixelDiff(int col, int row, Pnm_ppm image1, Pnm_ppm image2);

int main(int argc, char *argv[]){

        // ensure that exactly 2 file arguments are provided
        assert(argc == 3);

        FILE *fp1, *fp2; 
        Pnm_ppm image1, image2;
        unsigned int col, row;
        double sum = 0.0;
        double error;

        A2Methods_T methods = uarray2_methods_plain;
        assert(methods);
        
        // open the first image file in binary read mode and read the ppm image
        fp1 = fopen(argv[1],"rb");
        image1 = Pnm_ppmread(fp1, methods);
        

        fp2 = fopen(argv[2],"rb");
        image2 = Pnm_ppmread(fp2, methods);
          

        printf("image1 width: %u,   height: %u\n", image1->width, 
                image1->height);
        printf("image2 width: %u,   height: %u\n", image2->width, 
                image2->height);
        // ensures the images differ in width and height by at most 1 pixel
        // assert(abs((int)(image1->width) - (int)(image2->width)) <= 1); 
        // assert(abs((int)(image1->height) - (int)(image2->height)) <= 1); 
        

        unsigned int pixel_count = 0;

        for (row = 0; row < image1->height && row < image2->height; row++) {
                for (col = 0; col < image1->width && col < image2->width; col++) {
                sum += pixelDiff(col, row, image1, image2);
                pixel_count++;
                }
        }

        double max_pixel_value = 255.0; 
        error = sqrt(sum / (3.0 * pixel_count * max_pixel_value * 
                     max_pixel_value)); 

        printf("%.4f\n", error);

        Pnm_ppmfree(&image1);
        Pnm_ppmfree(&image2);

        fclose(fp1);
        fclose(fp2);

        return 0;
}

double pixelDiff(int col, int row, Pnm_ppm image1, Pnm_ppm image2){
        Pnm_rgb pixel1, pixel2;
        int rdiff, gdiff, bdiff, sum;
        pixel1 = image1->methods->at(image1->pixels, col, row);
        pixel2 = image2->methods->at(image2->pixels, col, row);
        rdiff = pixel1->red - pixel2->red;
        gdiff = pixel1->green - pixel2->green;
        bdiff = pixel1->blue - pixel2->blue;
        sum = rdiff * rdiff + gdiff * gdiff + bdiff * bdiff;
        return sum;
}