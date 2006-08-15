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

/*  This file, scalnull.c, contains the FITSIO routines used to define     */
/*  the starting heap address, the value scaling and the null values.      */

/*  The FITSIO software was written by William Pence at the High Energy    */
/*  Astrophysic Science Archive Research Center (HEASARC) at the NASA      */
/*  Goddard Space Flight Center.                                           */

#include <string.h>
#include "fitsio2.h"
/*--------------------------------------------------------------------------*/
int ffpthp(fitsfile *fptr,      /* I - FITS file pointer */
           long theap,          /* I - starting addrss for the heap */
           int *status)         /* IO - error status     */
/*
  Define the starting address for the heap for a binary table.
  The default address is NAXIS1 * NAXIS2.  It is in units of
  bytes relative to the beginning of the regular binary table data.
  This routine also writes the appropriate THEAP keyword to the
  FITS header.
*/
{
    if (*status > 0 || theap < 1)
        return(*status);

    /* reset position to the correct HDU if necessary */
    if (fptr->HDUposition != (fptr->Fptr)->curhdu)
        ffmahd(fptr, (fptr->HDUposition) + 1, NULL, status);

    (fptr->Fptr)->heapstart = theap;

    ffukyj(fptr, "THEAP", theap, "byte offset to heap area", status);

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpscl(fitsfile *fptr,      /* I - FITS file pointer               */
           double scale,        /* I - scaling factor: value of BSCALE */
           double zero,         /* I - zero point: value of BZERO      */
           int *status)         /* IO - error status                   */
/*
  Define the linear scaling factor for the primary array or image extension
  pixel values. This routine overrides the scaling values given by the
  BSCALE and BZERO keywords if present.  Note that this routine does not
  write or modify the BSCALE and BZERO keywords, but instead only modifies
  the values temporarily in the internal buffer.  Thus, a subsequent call to
  the ffrdef routine will reset the scaling back to the BSCALE and BZERO
  keyword values (or 1. and 0. respectively if the keywords are not present).
*/
{
    tcolumn *colptr;
    int hdutype;

    if (*status > 0)
        return(*status);

    if (scale == 0)
        return(*status = ZERO_SCALE);  /* zero scale value is illegal */

    if (ffghdt(fptr, &hdutype, status) > 0)  /* get HDU type */
        return(*status);

    if (hdutype != IMAGE_HDU)
        return(*status = NOT_IMAGE);         /* not proper HDU type */

    if (fits_is_compressed_image(fptr, status)) /* compressed images */
    {
        (fptr->Fptr)->cn_bscale = scale;
        (fptr->Fptr)->cn_bzero  = zero;

        return(*status);
    }

    /* set pointer to the first 'column' (contains group parameters if any) */
    colptr = (fptr->Fptr)->tableptr; 

    colptr++;   /* increment to the 2nd 'column' pointer  (the image itself) */

    colptr->tscale = scale;
    colptr->tzero = zero;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffpnul(fitsfile *fptr,      /* I - FITS file pointer                */
           LONGLONG nulvalue,   /* I - null pixel value: value of BLANK */
           int *status)         /* IO - error status                    */
/*
  Define the value used to represent undefined pixels in the primary array or
  image extension. This only applies to integer image pixel (i.e. BITPIX > 0).
  This routine overrides the null pixel value given by the BLANK keyword
  if present.  Note that this routine does not write or modify the BLANK
  keyword, but instead only modifies the value temporarily in the internal
  buffer. Thus, a subsequent call to the ffrdef routine will reset the null
  value back to the BLANK  keyword value (or not defined if the keyword is not
  present).
*/
{
    tcolumn *colptr;
    int hdutype;

    if (*status > 0)
        return(*status);

    if (ffghdt(fptr, &hdutype, status) > 0)  /* get HDU type */
        return(*status);

    if (hdutype != IMAGE_HDU)
        return(*status = NOT_IMAGE);         /* not proper HDU type */

    if (fits_is_compressed_image(fptr, status)) /* ignore compressed images */
        return(*status);

    /* set pointer to the first 'column' (contains group parameters if any) */
    colptr = (fptr->Fptr)->tableptr; 

    colptr++;   /* increment to the 2nd 'column' pointer  (the image itself) */

    colptr->tnull = nulvalue;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int fftscl(fitsfile *fptr,      /* I - FITS file pointer */
           int colnum,          /* I - column number to apply scaling to */
           double scale,        /* I - scaling factor: value of TSCALn   */
           double zero,         /* I - zero point: value of TZEROn       */
           int *status)         /* IO - error status     */
/*
  Define the linear scaling factor for the TABLE or BINTABLE extension
  column values. This routine overrides the scaling values given by the
  TSCALn and TZEROn keywords if present.  Note that this routine does not
  write or modify the TSCALn and TZEROn keywords, but instead only modifies
  the values temporarily in the internal buffer.  Thus, a subsequent call to
  the ffrdef routine will reset the scaling back to the TSCALn and TZEROn
  keyword values (or 1. and 0. respectively if the keywords are not present).
*/
{
    tcolumn *colptr;
    int hdutype;

    if (*status > 0)
        return(*status);

    if (scale == 0)
        return(*status = ZERO_SCALE);  /* zero scale value is illegal */

    if (ffghdt(fptr, &hdutype, status) > 0)  /* get HDU type */
        return(*status);

    if (hdutype == IMAGE_HDU)
        return(*status = NOT_TABLE);         /* not proper HDU type */

    colptr = (fptr->Fptr)->tableptr;   /* set pointer to the first column */
    colptr += (colnum - 1);     /* increment to the correct column */

    colptr->tscale = scale;
    colptr->tzero = zero;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int fftnul(fitsfile *fptr,      /* I - FITS file pointer                  */
           int colnum,          /* I - column number to apply nulvalue to */
           LONGLONG nulvalue,   /* I - null pixel value: value of TNULLn  */
           int *status)         /* IO - error status                      */
/*
  Define the value used to represent undefined pixels in the BINTABLE column.
  This only applies to integer datatype columns (TFORM = B, I, or J).
  This routine overrides the null pixel value given by the TNULLn keyword
  if present.  Note that this routine does not write or modify the TNULLn
  keyword, but instead only modifies the value temporarily in the internal
  buffer. Thus, a subsequent call to the ffrdef routine will reset the null
  value back to the TNULLn  keyword value (or not defined if the keyword is not
  present).
*/
{
    tcolumn *colptr;
    int hdutype;

    if (*status > 0)
        return(*status);

    if (ffghdt(fptr, &hdutype, status) > 0)  /* get HDU type */
        return(*status);

    if (hdutype != BINARY_TBL)
        return(*status = NOT_BTABLE);        /* not proper HDU type */
 
    colptr = (fptr->Fptr)->tableptr;   /* set pointer to the first column */
    colptr += (colnum - 1);    /* increment to the correct column */

    colptr->tnull = nulvalue;

    return(*status);
}
/*--------------------------------------------------------------------------*/
int ffsnul(fitsfile *fptr,      /* I - FITS file pointer                  */
           int colnum,          /* I - column number to apply nulvalue to */
           char *nulstring,     /* I - null pixel value: value of TNULLn  */
           int *status)         /* IO - error status                      */
/*
  Define the string used to represent undefined pixels in the ASCII TABLE
  column. This routine overrides the null  value given by the TNULLn keyword
  if present.  Note that this routine does not write or modify the TNULLn
  keyword, but instead only modifies the value temporarily in the internal
  buffer. Thus, a subsequent call to the ffrdef routine will reset the null
  value back to the TNULLn keyword value (or not defined if the keyword is not
  present).
*/
{
    tcolumn *colptr;
    int hdutype;

    if (*status > 0)
        return(*status);

    if (ffghdt(fptr, &hdutype, status) > 0)  /* get HDU type */
        return(*status);

    if (hdutype != ASCII_TBL)
        return(*status = NOT_ATABLE);        /* not proper HDU type */
 
    colptr = (fptr->Fptr)->tableptr;   /* set pointer to the first column */
    colptr += (colnum - 1);    /* increment to the correct column */

    colptr->strnull[0] = '\0';
    strncat(colptr->strnull, nulstring, 19);  /* limit string to 19 chars */

    return(*status);
}
