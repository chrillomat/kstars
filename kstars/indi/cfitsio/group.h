/*
Copyright (Unpublished-all rights reserved under the copyright laws of the United States), U.S. Government as represented by the Administrator of the National Aeronautics and Space Administration. No copyright is claimed in the United States under Title 17, U.S. Code.

Permission to freely use, copy, modify, and distribute this software and its documentation without fee is hereby granted, provided that this copyright notice and disclaimer of warranty appears in all copies. (However, see the restriction on the use of the gzip compression code, below).

e-mail: pence@tetra.gsfc.nasa.gov

DISCLAIMER:

THE SOFTWARE IS PROVIDED 'AS IS' WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND FREEDOM FROM INFRINGEMENT, AND ANY WARRANTY THAT THE DOCUMENTATION WILL CONFORM TO THE SOFTWARE, OR ANY WARRANTY THAT THE SOFTWARE WILL BE ERROR FREE. IN NO EVENT SHALL NASA BE LIABLE FOR ANY DAMAGES, INCLUDING, BUT NOT LIMITED TO, DIRECT, INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES, ARISING OUT OF, RESULTING FROM, OR IN ANY WAY CONNECTED WITH THIS SOFTWARE, WHETHER OR NOT BASED UPON WARRANTY, CONTRACT, TORT , OR OTHERWISE, WHETHER OR NOT INJURY WAS SUSTAINED BY PERSONS OR PROPERTY OR OTHERWISE, AND WHETHER OR NOT LOSS WAS SUSTAINED FROM, OR AROSE OUT OF THE RESULTS OF, OR USE OF, THE SOFTWARE OR SERVICES PROVIDED HEREUNDER."

The file compress.c contains (slightly modified) source code that originally came from gzip-1.2.4, copyright (C) 1992-1993 by Jean-loup Gailly. This gzip code is distributed under the GNU General Public License and thus requires that any software that uses the CFITSIO library (which in turn uses the gzip code) must conform to the provisions in the GNU General Public License. A copy of the GNU license is included at the beginning of compress.c file.

Similarly, the file wcsutil.c contains 2 slightly modified routines from the Classic AIPS package that are also distributed under the GNU General Public License.

Alternate versions of the compress.c and wcsutil.c files (called compress_alternate.c and wcsutil_alternate.c) are provided for users who want to use the CFITSIO library but are unwilling or unable to publicly release their software under the terms of the GNU General Public License. These alternate versions contains non-functional stubs for the file compression and uncompression routines and the world coordinate transformation routines used by CFITSIO. Replace the file `compress.c' with `compress_alternate.c' and 'wcsutil.c' with 'wcsutil_alternate.c before compiling the CFITSIO library. This will produce a version of CFITSIO which does not support reading or writing compressed FITS files, or doing image coordinate transformations, but is otherwise identical to the standard version. 

*/

#define MAX_HDU_TRACKER 1000

typedef struct _HDUtracker HDUtracker;

struct _HDUtracker
{
  int nHDU;

  char *filename[MAX_HDU_TRACKER];
  int  position[MAX_HDU_TRACKER];

  char *newFilename[MAX_HDU_TRACKER];
  int  newPosition[MAX_HDU_TRACKER];
};

/* functions used internally in the grouping convention module */

int ffgtdc(int grouptype, int xtensioncol, int extnamecol, int extvercol,
	   int positioncol, int locationcol, int uricol, char *ttype[],
	   char *tform[], int *ncols, int  *status);

int ffgtgc(fitsfile *gfptr, int *xtensionCol, int *extnameCol, int *extverCol,
	   int *positionCol, int *locationCol, int *uriCol, int *grptype,
	   int *status);

int ffgmul(fitsfile *mfptr, int rmopt, int *status);

int ffgmf(fitsfile *gfptr, char *xtension, char *extname, int extver,	   
	  int position,	char *location,	long *member, int *status);

int ffgtrmr(fitsfile *gfptr, HDUtracker *HDU, int *status);

int ffgtcpr(fitsfile *infptr, fitsfile *outfptr, int cpopt, HDUtracker *HDU,
	    int *status);

int fftsad(fitsfile *mfptr, HDUtracker *HDU, int *newPosition, 
	   char *newFileName);

int fftsud(fitsfile *mfptr, HDUtracker *HDU, int newPosition, 
	   char *newFileName);

void prepare_keyvalue(char *keyvalue);

int fits_path2url(char *inpath, char *outpath, int  *status);

int fits_url2path(char *inpath, char *outpath, int  *status);

int fits_get_cwd(char *cwd, int *status);

int fits_get_url(fitsfile *fptr, char *realURL, char *startURL, 
		 char *realAccess, char *startAccess, int *iostate, 
		 int *status);

int fits_clean_url(char *inURL, char *outURL, int *status);

int fits_relurl2url(char *refURL, char *relURL, char *absURL, int *status);

int fits_url2relurl(char *refURL, char *absURL, char *relURL, int *status);

int fits_encode_url(char *inpath, char *outpath, int *status);

int fits_unencode_url(char *inpath, char *outpath, int *status);

int fits_is_url_absolute(char *url);

