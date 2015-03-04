/*
 * CPSC4310 Assignment 2 unsharp filter implementation
 * usage:  
 *   unsharp input.pgm output.pgm
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
void unsharp_image(tuple **array, const pam &inpam);

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
  
  /* unsharp the image */
  unsharp_image(array, inpam);

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

/* unsharp the image */
void unsharp_image(tuple **array, const pam &inpam)
{
  //initialize local variables
  int row, col, intensity;
  intensity = 0;
  //create temp image to write new intensities
  pam temppam = inpam;
  temppam.height =  inpam.height;
  temppam.width = inpam.width;
  tuple **newarray;
  newarray = pnm_allocpamarray(&temppam);

  //modify image using 3x3 averaging mask 
  for (row = 1; row < inpam.height-1; row++) {
    for (col = 1; col < inpam.width-1; col++) {
      intensity = array[row-1][col-1][0];
      intensity += array[row-1][col][0];
      intensity += array[row-1][col+1][0];
      intensity += array[row][col-1][0];
      intensity += array[row][col+1][0];
      intensity += array[row+1][col-1][0];
      intensity += array[row+1][col][0];
      intensity += array[row+1][col+1][0];
      intensity += (array[row][col][0]);
      newarray[row][col][0] = intensity/9;
    }
  }
  int temp = 0;
  //apply unsharpen
  for (row = 0; row < inpam.height; row++) {
    for (col = 0; col < inpam.width; col++) { 
      //copy subracted value to temporary variable    
      temp = array[row][col][0] - newarray[row][col][0];
      //add temporary variable's value to original image
      array[row][col][0] = array[row][col][0] + temp;
      
      //check that the pixel is within boundaries ([0-255])
      if (array[row][col][0] > 255){
        array[row][col][0] = 255;
      }
      if (array[row][col][0] < 0){
        array[row][col][0] = 0;
      }
    }
  }
  //clean up
  pnm_freepamarray(newarray, &temppam);
}
