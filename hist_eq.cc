/*
 * CPSC4310 Assignment 2 histogram equalization implementation
 * usage:
 *   hist_eq input.pgm output.pgm
 *
 * Joshua Enns
 */

#include <pam.h>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <vector>

using namespace std;

tuple **read_image(char *filename, pam &inpam);
void write_image(char *filename, const pam &inpam, tuple **array);
void eq_image(tuple **array, const pam &inpam);

int main(int argc, char *argv[])
{
  /* structures for input image */
  pam inpam;

  /* a dynamic two-dimensional array to store the pixels*/
  tuple **array;

  /* initializes the library */
  pm_init(argv[0], 0);

  /* read the image */
  array = read_image(argv[1], inpam);
  
  /* euqalilze the image */
  eq_image(array, inpam);

  /* write the output */
  write_image(argv[2], inpam, array);

  /* clean up */
  pnm_freepamarray(array, &inpam);

  return 0;
}

/* reads the image into the netpbm structure */
tuple **read_image(char *filename, pam &inpam)
{
  FILE *f;
  tuple **A;

  if ((f = pm_openr(filename)) == NULL) {
    cerr << "Cannot open file \"" << filename << "\" for reading." << endl;
    exit(1);
  }

  if ((A = pnm_readpam(f, &inpam, PAM_STRUCT_SIZE(tuple_type))) == NULL) {
    cerr << "Cannot read image \"" << filename << "\"." << endl;
    exit(1);
  }
  
  pm_close(f);
  return A;
}

/* writes the image to the given file */
void write_image(char *filename, const pam &inpam, tuple **array)
{
  FILE *f;
  pam outpam = inpam;
  
  if ((f = pm_openw(filename)) == NULL) {
    cerr << "Cannot open file \"" << filename << "\" for writing." << endl;
    exit(1);
  }

  outpam.file = f;
  
  pnm_writepam(&outpam, array);

  pm_close(f);
}

/* equalize the image */
void eq_image(tuple **array, const pam &inpam)
{
  int row, col, i, cumulativeDist, min;
  int histogram [255];
  int totalPixels = inpam.height * inpam.width;
  //initialize array
  for (i = 0; i < 255; i++){
    histogram[i] = 0;
  }
  //build histogram in [0-255] array
  for (row = 0; row < inpam.height; row++) {
    for (col = 0; col < inpam.width; col++) {
      histogram[array[row][col][0]]++;
    }
  }
  //calculate cumulative distribution
  cumulativeDist = 0;
  min = 0;
  for (i = 0; i < 255; i++){
    //set min to histogram[0] for the first loop
    if (min == 0){
      min = histogram[i];
    }
    cumulativeDist += histogram[i];
    histogram[i] = cumulativeDist;
  }
  int current;
  //loop through image and calculate each pixel
  for (row = 0; row < inpam.height; row++) {
    for (col = 0; col < inpam.width; col++) {
      current = histogram[array[row][col][0]];
      //formula: h(v) = ((cdf(v) - cdfmin)/(M * N - cdfmin) * (L-1))
      array[row][col][0] = (int)((current-min)/double(totalPixels-min)*255);
    }    
  }
}
