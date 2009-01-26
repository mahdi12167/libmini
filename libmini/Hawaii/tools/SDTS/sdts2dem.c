/*********************************************************************

   SDTS to DEM optional (native) format converter

   THIS PROGRAM IS IN THE PUBLIC DOMAIN AND MAY BE FREELY REDISTRIBUTED.

          program:  stds2dem.c
  original author:  Sol Katz, US Bureau of Land Management
    original date:  Mar 23, 1998
        revisions:  see below

   This program dumps SDTS DEM modules to a data file which is
   supposed to reconstruct the original dem source file


Revision History:

.006  corrected writing to quad to new file
.007  corrected calculation for xmin, ymin to work on east of meridian.
      set profiles x,y to be multiples of height and width.
.008  printed maxvalue and minvalue in header correctly.
.009  print arc/info extention in header to indicate datum,
      properly handle elevation units,
.010  use fread and fwrite to speed up program
.011  change calc of each row Y to be 1 cell height less, NAX fix

.012  initialize col=1 in cells_out
      10/13/98 initialize row=1, col=1 in dem_out

      It has been suggested that the calculation for the xmin and
      ymin is wrong, that it should be rounded down, and this will
      fix the occasional missing row or column between maps. Need
      to look at line with .5 in it and see if it needs to be changes.
      ssk.

      Ed Russel of computer train mapping gave me 3 changes that
      he said fixed the code. They are in this release. It included
      removing an earlier fix that set profiles x,y to be multiples of
      height and width.

.012a [02/28/99] Ported to gcc+EMX and gcc+RSXNT for OS/2 and Win32
      environments, respectively (Allen Cogbill).

.013  [02/01/98] Removed the dependence upon a direct-access file,
      thus speeding up the calculations. Resulting code uses more
      memory, of course. The use of a direct-access file seems tied
      the fact that the original code was run under DOS using a
      M$ compiler. (Allen Cogbill)

.013a [04/25/00] Fixed some printf format problems (long ints as ints).
      Changed fgets to scanf.  Changed some prompt info.  Made "cells"
      global (to avoid stack overflow). (Will Evans)

.014  [14-aug-2001, Gregg Townsend] Lots of changes:
      Read new series 24K DEM files with different RSDF & SPDM files.
      Read "decimal meter" files with 32-bit floating-point elevations.
      Run correctly on big-endian (e.g. Sun) architecture.
      Increase MAX_CELLS to handle 10 meter grids at southerly latitudes.
      Simplify output banner.
      Clean up source code somewhat.

.015  [22-aug-2001, Gregg Townsend]
      Properly handle maximum elevations exceeding 32767 (decimeters).
      Implement some simple output speedups.

.016  [5-sep-2001, Gregg Townsend]
      Improve calculations that align data samples with coordinate grid;
      believe the SADR value if it is credible (as in y2001 SDTS files).
      Output minimum/maximum elevation correctly for floating-point files.
      Punctuate the text in the output file header a little better.
      Change output progress report to a simple row of dots, to avoid
      messing up log files.  Also show progress when reading input.

.017  [3-oct-2001, Gregg Townsend]
      Fix column and grid minimum/maximum calculations:  Don't limit
      minimum to a max of 32767 units, and don't carry forward min/max
      from one column into the next.  Don't output "missing count" if
      zero.  Clean up the source code a little more.

.018  [29-apr-2002, Gregg Townsend]
      Properly scale 10x10m input grid read in BI16 format.
      Deduce input precision based on DDSH/FMT field, instead of guessing.
      Don't ask "usually L0" question if nnnnCEL0.DDF exists.
      Indicate coordinate units as part of stderr commentary.
      Fix "Missing value" undercount.

.018P [18-jan-2005, Stefan Roettger]
      Added PGM output.

.019P [19-apr-2006, Stefan Roettger]
      Interprete sea level values as missing data.

*********************************************************************/

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stc123.h"

#define MAX_ROWS 2048
#define BASE_FILENAME 249
#define MAX_CELLS 2000000

#define PDOTS 50        /* number of progress dots (...) to output */

/* Elevations are biased by 1000 and stored as unsigned shorts. */
/* This accommodates Death Valley to Denali in either feet or decimeters. */
/* A stored value of zero, indicating a missing value, outputs as -9999. */
#define ELEVATION_BIAS 1000     /* elevation bias for storage */
#define MISSING_STORED 0        /* value stored to indicate missing data */
#define MISSING_VALUE -9999     /* traditional missing value on output */

const char *version = "SDTS2DEM v.0.019P";
const char *last_updated = "19 April 2006";
const char *authors = "Sol Katz / AHC / WSE / GMT / SR";

FILE *fpdem;            /* DEM output file */
FILE *fppgm;            /* PGM output file */
int newlines;           /* if true [non-zero], new lines are added */

int idem[MAX_ROWS];
unsigned short cells[MAX_CELLS];

long nrows, ncols;
long izone;
char short_buff[200];
char buffer[1025];
double sfax, sfay, xorg, yorg, xhrs, yhrs;

/* Internal Spatial Reference transformation parameters */
double height, width;
int vscale = 1;         /* vertical scale factor for output */
double upperlx, upperly;
long scale;

char descr[5000];
char frmts[500];
long fillvalue;
long voidvalue;
double minvalue, maxvalue;
char title[144];
char DAID[100];
char date[11];
char base_name[BASE_FILENAME];
char file_name[13];
char out_file[BASE_FILENAME + 4];
char out_file2[BASE_FILENAME + 4];
int order;                      /* byte order returned by g123order */
int gnd_units = 2;
int elev_units = 2;
int datum = 1;

double NEX, NEY, NWX, NWY, SWX, SWY, SEX, SEY;
char rsnm[5];
char cellid[3];

/* beg123file() return values */
long int_level;
char ice;
char ccs[4];
FILE *fpin;

/* rd123sfld() return values */
char tag[10];
char leadid;
char string[5000];
long str_len;
int status;

/*********************************************/

int e2d(char *s);       /* converts "e" exponentials to "D" exponentials */
void chomp(char *s);    /* removes a trailing newline at end of a string */
void get_iref(void);
void get_xref(void);
void dem_rc(void);
void dem_head(void);
void dem_mbr(void);
void cell_range(void);
int fill_cells(unsigned short[], int);  /* fills array cells[] with values */
void dem_out(unsigned short[], int *);
void pgm_out(unsigned short[], int *);
void get_nw_corner(void);
void get_elev_units(void);
char *units(int);                       /* translates unit codes to strings */

/*********************************************/

int main(int argc, char *argv[]) {
    int ncells, k;
    char format[12];

    fprintf(stderr, "\n%s (%s) by %s\n", version, last_updated, authors);

    /* Check to see if the newline option has been set */
    newlines = 0;
    if (argc > 1) {
        if (!strcmp(argv[1], "-n")) {
            newlines = 1;
            for (k = 1; k < argc; k++) {
                argv[k] = argv[k + 1];
            }
            argc--;
        }
    }

    if (argc < 3) {
        fprintf(stderr, "\n");
        fprintf(stderr,
            "Usage: sdts2dem [-n] [DDFbase] [DEMbase] [cell_id]\n");
        fprintf(stderr,
            "            -n:  "
            "put newlines at end of 1024-char line [default is NO newline].\n");
        fprintf(stderr,
            "       DDFbase:  "
            "first 4 chars (ABCD) in DDF file name (ABCDxxxx.ddf)\n");
        fprintf(stderr,
            "       DEMbase:  "
            "file name for output without .DEM extension\n");
        fprintf(stderr,
            "       cell_id:  "
            "characters in position 7 and 8 of CELL file name (usually 'L0')\n");
        fprintf(stderr, "\n");
    }

    if (argc < 2) {             /* prompt for input SDTS file name */
        fprintf(stdout,
                "Enter first 4 charcters of the base SDTS file name: ");
        scanf("%4s", base_name);
    } else {
        base_name[0] = '\0';
        strncpy(base_name, argv[1], 4);
    }

    if (argc < 3) {             /* prompt for output dem file name */
        fprintf(stdout,
                "Enter base output file name (exclude any extension): ");
        sprintf(format, "%%%ds", BASE_FILENAME);
        scanf(format, out_file);
    } else {
        out_file[0] = '\0';
        strncpy(out_file, argv[2], BASE_FILENAME);
    }
    strcpy(out_file2, out_file);
    strcat(out_file, ".dem");
    strcat(out_file2, ".pgm");

    if (argc < 4) {             /* prompt for 2 digit layer number */
        sprintf(file_name, "%.4sCEL0.DDF", base_name);
        fpin = fopen(file_name, "rb");
        if (fpin != NULL) {
            fclose(fpin);               /* found nnnnCEL0.DDF */
            strcpy(cellid, "L0");       /* so don't ask */
        } else {
            fprintf(stdout,
                "Enter the 2 chars in position 7-8 of the CELL file name"
                " (usually L0): ");
            scanf("%2s", cellid);
        }
    } else {
        cellid[0] = '\0';
        strncpy(cellid, argv[3], 2);
    }

    /* Determine byte order of current machine */
    g123order(&order);

    /* Get translation parameters for DEM dataset from the ISRF module */
    strcpy(file_name, base_name);       /* Note that base_name <= 4 chars */
    strcat(file_name, "CE");
    strcat(file_name, cellid);          /* cellid <= 2 chars in length */
    strcat(file_name, ".DDF");
    if (!beg123file(file_name, 'R', &int_level, &ice, ccs, &fpin)) {
        fprintf(stderr, "\nCAN'T OPEN 'DEM' CELL FILE %s", file_name);
        fprintf(stderr, "\nAre you sure this is a SDTS/DEM directory?\n");
        exit(-1);
    }
    end123file(&fpin);

    /* Open output .dem file */
    fpdem = fopen(out_file, "w");
    if (fpdem == NULL) {
        fprintf(stderr, "\nERROR OPENING .DEM FILE %s\n", out_file);
        end123file(&fpin);
        exit(-2);
    }

    /* Open output .pgm file */
    fppgm = fopen(out_file2, "wb");
    if (fppgm == NULL) {
        fprintf(stderr, "\nERROR OPENING .PGM FILE %s\n", out_file);
        end123file(&fpin);
        exit(-2);
    }

    get_elev_units();
    dem_head();
    cell_range();
    get_iref();
    get_xref();
    get_nw_corner();

    dem_mbr();
    dem_rc();

    strcat(title, "  ");
    strcat(title, DAID);
    fprintf(stderr, "\nSummary of SDTS/DEM Data Files\n");
    fprintf(stderr, "Title = %s \nDate = %s \n", title, date);
    fprintf(stderr, "Range:  max= %.1f, min= %.1f, void= %ld, fill= %ld\n",
            maxvalue, minvalue, voidvalue, fillvalue);
    fprintf(stderr, "Cell Width:              %f %s\n",width, units(gnd_units));
    fprintf(stderr, "Cell Height:             %f %s\n",height,units(gnd_units));
    fprintf(stderr, "Vertical Resolution:     %f %s\n", 1.0 / vscale,
            units(elev_units));
    fprintf(stderr, "Projection:              %s\n", rsnm);
    fprintf(stderr, "Zone:                    %ld\n", izone);
    fprintf(stderr, "Total rows = %ld, Total columns= %ld \n", nrows, ncols);
    fprintf(stderr, "\n");

    if (nrows > MAX_ROWS) {
        fprintf(stderr, "%ld rows exceed the limit (%d) of the program\n",
                nrows, MAX_ROWS);
        exit(1);
    }
    ncells = fill_cells(cells, MAX_CELLS);
    if (ncells <= 0) {
        exit(ncells);
    }

    strcat(title, " (");
    strcat(title, version);
    strcat(title, ")");
    dem_out(cells, &ncells);
    fprintf(stderr,"\n");
    pgm_out(cells, &ncells);

    /* Close files and end */
    fclose(fpdem);
    fclose(fppgm);
    return 0;
}

/***********************/
void dem_rc(void) {

    strcpy(file_name, base_name);
    strcat(file_name, "LDEF.DDF");

    if (!beg123file(file_name, 'R', &int_level, &ice, ccs, &fpin)) {
        fprintf(stderr, "\nCAN'T OPEN 'dem' LDEF FILE %s", file_name);
        exit(1);
    }

    /* Read data descriptive record (DDR) */
    if (!rd123ddrec(fpin,               /* file pointer */
                    string,             /* DDR record returned */
                    &status)) {         /* status returned */
        fprintf(stderr, "\n*** ERROR READING DDR *** %d", status);
        fprintf(fpdem, "\n*** ERROR READING DDR *** %d", status);
        goto done;
    }
    status = -1;

    /* Loop to process each subfield */
    do {

        /* Read data record subfield */
        if (!rd123sfld(fpin,            /* file pointer */
                       tag,             /* field tag returned */
                       &leadid,         /* leader identifier returned */
                       string,          /* subfield contents returned */
                       &str_len,        /* length of subfield */
                       &status)) {      /* status returned */
            fprintf(stderr, "\nERROR READING DATA RECORD SUBFIELD");
            fprintf(fpdem, "\nERROR READING DATA RECORD SUBFIELD");
            goto done;
        }

        /* Retrieve description of current subfield */
        if (!chk123sfld(fpin,           /* file pointer */
                        tag,            /* tag output */
                        descr,          /* subfield descriptions output */
                        frmts)) {       /* subfield format control */
            fprintf(stderr, "\nERROR CHECKING DATA RECORD SUBFIELD");
            fprintf(fpdem, "\nERROR CHECKING DATA RECORD SUBFIELD");
            goto done;
        }

        /* Process based on field and subfield tags */
        if (!strcmp(tag, "LDEF") && !strcmp(descr, "NROW")) {
            nrows = atol(string);
        } else if (!strcmp(tag, "LDEF") && !strcmp(descr, "NCOL")) {
            ncols = atol(string);
        }

    } while (status != 4);       /* Break out of loop at end of file */

done:
    end123file(&fpin);
    return;

}

/********************************/
void get_xref(void) {

    /* Set some default values */
    strcpy(rsnm, "??1");
    strcpy(file_name, base_name);
    strcat(file_name, "XREF.DDF");

    if (!beg123file(file_name, 'R', &int_level, &ice, ccs, &fpin)) {
        fprintf(stderr, "\nERROR OPENING FILE %s", file_name);
        exit(0);
    }

    if (!rd123ddrec(fpin,               /* file pointer */
                    string,             /* DDR record returned */
                    &status)) {         /* status returned */
        fprintf(stderr, "\n*** ERROR READING DDR ***");
        goto done;
    }
    status = -1;

    /* Loop to process each subfield in Identification module */
    do {

        /* Read data record subfield */
        if (!rd123sfld(fpin,            /* file pointer */
                       tag,             /* field tag returned */
                       &leadid,         /* leader identifier returned */
                       string,          /* subfield contents returned */
                       &str_len,        /* string length */
                       &status)) {      /* status returned */
            fprintf(stderr,
                    "\nERROR READING DATA RECORD SUBFIELD (IDEN MODULE)");
            goto done;
        }

        /* Retrieve description of current subfield */
        if (!chk123sfld(fpin,           /* file pointer */
                        tag,            /* tag output */
                        descr,          /* subfield descriptions output */
                        frmts)) {       /* subfield format control */
            fprintf(stderr, "\nERROR CHECKING DATA RECORD SUBFIELD");
            goto done;
        }

        /* Check subfield name and extract contents for each subfield */
        if (!strcmp(tag, "XREF") && !strcmp(descr, "RSNM")) {
            strcpy(rsnm, string);
        } else if (!strcmp(tag, "XREF") && !strcmp(descr, "ZONE")) {
            izone = atoi(string);
        } else if (!strcmp(tag, "XREF") && !strcmp(descr, "HDAT")) {
            /* This is for the arcinfo datum extension */
            if (!strcmp(string, "NAS")) {
                datum = 1;
            } else if (!strcmp(string, "WGS72")) {
                datum = 2;
            } else if (!strcmp(string, "WGS84")) {
                datum = 3;
            } else if (!strcmp(string, "NAX")) {
                datum = 4;
            }
        }

    } while (status != 4);      /* Break out of loop at end of file */

done:
    end123file(&fpin);
    return;
}

/***************************************/
void dem_head(void) {

    /* Open Identifcation module */
    strcpy(file_name, base_name);
    strcat(file_name, "IDEN.DDF");
    if (!beg123file(file_name, 'R', &int_level, &ice, ccs, &fpin)) {
        fprintf(stderr, "\nERROR OPENING FILE %s", file_name);
        exit(0);
    }

    /* Read Identification module data descriptive record (DDR)*/
    if (!rd123ddrec(fpin,               /* file pointer */
                    string,             /* DDR record returned */
                    &status)) {         /* status returned */
        fprintf(stderr, "\n*** ERROR READING DDR ***");
        goto done;
    }
    status = -1;

    /* Loop to process each subfield in Identification module */
    do {

        /* Read data record subfield */
        if (!rd123sfld(fpin,            /* file pointer */
                       tag,             /* field tag returned */
                       &leadid,         /* leader identifier returned */
                       string,          /* subfield contents returned */
                       &str_len,        /* string length */
                       &status)) {      /* status returned */
            fprintf(stderr,
                    "\nERROR READING DATA RECORD SUBFIELD (IDEN MODULE)");
            goto done;
        }

        /* Retrieve description of current subfield */
        if (!chk123sfld(fpin,           /* file pointer */
                        tag,            /* tag output */
                        descr,          /* subfield descriptions output */
                        frmts)) {       /* subfield format control */
            fprintf(stderr, "\nERROR CHECKING DATA RECORD SUBFIELD");
            goto done;
        }

        /* Display subfield name and contents for each subfield */
        if (!strcmp(tag, "IDEN") && !strcmp(descr, "TITL"))
            strcpy(title, string);      /* title */
        else if (!strcmp(tag, "IDEN") && !strcmp(descr, "SCAL")) {
            scale = atol(string);       /* scale */
        } else if (!strcmp(tag, "IDEN") && !strcmp(descr, "DAID"))
            strcpy(DAID, string);
        else if (!strcmp(tag, "IDEN") && !strcmp(descr, "DCDT"))
            strcpy(date, string);

    } while (status != 4);      /* Break out of loop at end of file */

done:
    /* Close input Identification module */
    status = end123file(&fpin);
}

/*********************************************/
void dem_mbr(void) {
    int i, nxy;
    int seq = 0;
    double fl;
    double x[5], y[5];

    strcpy(file_name, base_name);
    strcat(file_name, "SPDM.DDF");

    if (!beg123file(file_name, 'R', &int_level, &ice, ccs, &fpin)) {
        fprintf(stderr, "\nCAN'T OPEN 'dem' SPDM FILE %s", file_name);
        exit(0);
    }

    /* Read data descriptive record (DDR) */
    if (!rd123ddrec(fpin,               /* file pointer */
                    string,             /* DDR record returned */
                    &status)) {         /* status returned */
        fprintf(stderr, "\n*** ERROR READING DDR *** %d", status);
        fprintf(fpdem, "\n*** ERROR READING DDR *** %d", status);
        goto done;
    }

    status = -1;

    nxy = 1;                    /* number of coordinate pairs */

    /* Loop to process each subfield */
    do {

        /* Read data record subfield */
        if (!rd123sfld(fpin,            /* file pointer */
                       tag,             /* field tag returned */
                       &leadid,         /* leader identifier returned */
                       string,          /* subfield contents returned */
                       &str_len,        /* length of subfield */
                       &status)) {      /* status returned */
            fprintf(stderr, "\nERROR READING DATA RECORD SUBFIELD");
            fprintf(fpdem, "\nERROR READING DATA RECORD SUBFIELD");
            goto done;
        }

        /* Retrieve description of current subfield */
        if (!chk123sfld(fpin,           /* file pointer */
                        tag,            /* tag output */
                        descr,          /* subfield descriptions output */
                        frmts)) {       /* subfield format control */
            fprintf(stderr, "\nERROR CHECKING DATA RECORD SUBFIELD");
            fprintf(fpdem, "\nERROR CHECKING DATA RECORD SUBFIELD");
            goto done;
        }

        /* Process based on field and subfield tags */
        if (!strcmp(tag, "DMSA")) {
            fl = atof(string);
            if (!strcmp(descr, "X") || !strcmp(descr, "!X")) {
                x[nxy] = fl;            /* X spatial address */
            } else if (!strcmp(descr, "Y") || !strcmp(descr, "!Y")) {
                y[nxy] = fl;            /* Y spatial address */
                nxy++;
            }
        }

        /* If end of record, write out record and reinitialize */
        if (status == 3 || status == 4) {
            seq++;
            for (i = 1; i < nxy; i++) {
                if (i == 1) {
                    SWX = x[i];
                    SWY = y[i];
                } else if (i == 2) {
                    NWX = x[i];
                    NWY = y[i];
                } else if (i == 3) {
                    NEX = x[i];
                    NEY = y[i];
                } else if (i == 4) {
                    SEX = x[i];
                    SEY = y[i];
                }
            }
            nxy = 0;
        }
    } while (status != 4);      /* Break out of loop at end of file */

done:
    end123file(&fpin);
    return;
}

/*********************************************/
void cell_range(void) {
    int recid = 0;
    strcpy(file_name, base_name);
    strcat(file_name, "DDOM.DDF");


    if (!beg123file(file_name, 'R', &int_level, &ice, ccs, &fpin)) {
        fprintf(stderr, "\nERROR OPENING FILE %s", file_name);
        exit(0);
    }

    if (!rd123ddrec(fpin,               /* file pointer */
                    string,             /* DDR record returned */
                    &status)) {         /* status returned */
        fprintf(stderr, "\n*** ERROR READING DDR ***");
        goto done;
    }
    status = -1;

    /* Loop to process each subfield in CATS module */
    do {

        /* Read data record subfield */
        if (!rd123sfld(fpin,            /* file pointer */
                       tag,             /* field tag returned */
                       &leadid,         /* leader identifier returned */
                       string,          /* subfield contents returned */
                       &str_len,        /* string length */
                       &status)) {      /* status returned */
            fprintf(stderr,
                    "\nERROR READING DATA RECORD SUBFIELD (AHDR MODULE)");
            goto done;
        }

        /* Retrieve description of current subfield */
        if (!chk123sfld(fpin,           /* file pointer */
                        tag,            /* tag output */
                        descr,          /* subfield descriptions output */
                        frmts)) {       /* subfield format control */
            fprintf(stderr, "\nERROR CHECKING DATA RECORD SUBFIELD");
            goto done;
        }

        /* Check subfield name and extract contents for each subfield */
        if (!strcmp(tag, "DDOM") && !strcmp(descr, "RCID")) {
            recid = atoi(string);
        } else if (recid == 1 && !strcmp(descr, "DVAL")) {
            voidvalue = atoi(string);
        } else if (recid == 2 && !strcmp(descr, "DVAL")) {
            fillvalue = atoi(string);
        } else if (recid == 3 && !strcmp(descr, "DVAL")) {
            minvalue = atof(string);
        } else if (recid == 4 && !strcmp(descr, "DVAL")) {
            maxvalue = atof(string);
        }

    } while (status != 4);      /* Break out of loop at end of file */

done:
    end123file(&fpin);
    return;
}

/*********************************************/
int fill_cells(unsigned short cells[], int max_cells) {
    int cellval;
    int ncells = 0;
    int pnow, psent;
    strcpy(file_name, base_name);
    strcat(file_name, "CE");
    strcat(file_name, cellid);
    strcat(file_name, ".DDF");

    if (!beg123file(file_name, 'R', &int_level, &ice, ccs, &fpin)) {
        fprintf(stderr, "\nCAN'T OPEN CELL VALUES FILE %s.", file_name);
        return 0;
    }

    /* Read data descriptive record (DDR) */
    if (!rd123ddrec(fpin,               /* file pointer */
                    string,             /* DDR record returned */
                    &status)) {         /* status returned */

        fprintf(stderr, "\n*** ERROR READING DDR ***");
        fprintf(fpdem, "\n*** ERROR READING DDR ***");
        goto done;
    }
    status = -1;
    fprintf(stderr, "Reading input SDTS file ");
    psent = 0;

    /* Loop to process each subfield */
    do {

        /* Read data record subfield */
        if (!rd123sfld(fpin,            /* file pointer */
                       tag,             /* field tag returned */
                       &leadid,         /* leader identifier returned */
                       string,          /* subfield contents returned */
                       &str_len,        /* string length */
                       &status)) {      /* status returned */

            fprintf(stderr, "\nERROR READING DATA RECORD SUBFIELD");
            goto done;
        }

        /* Retrieve description of current subfield */
        if (!chk123sfld(fpin,           /* file pointer */
                        tag,            /* tag output */
                        descr,          /* subfield descriptions output */
                        frmts)) {       /* subfield format control */

            fprintf(stderr,
                    "\nERROR CHECKING DATA RECORD SUBFIELD, fill_cells\n");
            fprintf(fpdem,
                    "\nERROR CHECKING DATA RECORD SUBFIELD, fill_cells\n");
            goto done;
        }

        if (ncells >= max_cells) {
            fprintf(stderr,
                    "Too many cells in DEM: max permitted is %d.\n",
                    max_cells);
            return (-1);
        }

        /* Handle only 2-byte integers or 4-byte floats */
        if (strstr(frmts, "B") != NULL) {
            if (str_len == 2) {
                cellval = vscale * ((string[0] << 8) | (string[1] & 0xFF));
            } else if (str_len == 4) {
                union {
                    long i;
                    float f;
                } u;
                s123tol(string, &u.i, !order);
                cellval = (int) (vscale * u.f);
            } else {
                cellval = MISSING_VALUE;
            }
            if (cellval < -ELEVATION_BIAS)
                cells[ncells] = MISSING_STORED;
            else
                cells[ncells] = cellval + ELEVATION_BIAS;
            ncells++;

            pnow = (ncells * PDOTS) / (nrows * ncols);
            while (psent < pnow) {
                fprintf(stderr, ".");
                fflush(stderr);
                psent++;
            }
        }

    } while (status != 4);      /* Break out of loop at end of file */

done:
    fprintf(stderr, "\n");
    /* Close input ISO 8211 file */
    status = end123file(&fpin);
    return (ncells);
}

/*************************************************************************

            s u b r o u t i n e     d e m o u t
 function:
             this routine reads in the cell values for a cell map
             and writes it out in dem format
 routines
 called:

  programmer
  sol katz, blm/sc344, sept 93

  revised by:
   sol katz, blm, march 1998  to work with generic grids

*************************************************************************/

void dem_out(unsigned short cells[], int *number_cells) {

    char *p;
    double xgo, ygo;
    double elodat, dphi, xcor[5], ycor[5];
    int cmin, cmax, gmax, gmin, psent, pnow;

    int row, col, imrow, nvals, incol;
    int ifirst, ilast, nfirst, nlast, i, n, irow, icol;
    int irec, icell;
    int cellval, ncells;
    int iaccu, ilevel, ipttrn, icoord, isides;

    double xmin, ymin;
    int missing = 0;

    int crecord[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    double resolu[4] = { 0.0, 0.0, 0.0, 1.0 };
    double dproj[16] =
        { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. };

    fprintf(stderr, "Writing output DEM file ");

    /* Initialize */
    ncells = *number_cells;
    ilevel = 1;
    ipttrn = 1;
    icoord = 1;
    isides = 4;
    iaccu = 0;
    irow = 1;
    imrow = 1;
    incol = 1;
    dphi = 0.0;
    elodat = 0.0;

    irec = 1;
    icol = ncols;

    xmin = upperlx;
    ymin = upperly - (nrows - 1) * height;

    /* Ensure that profile aligns with grid (for pre-y2001 files) */
    if (fmod(xmin, width) > 0.0001 || fmod(ymin, height) > 0.0001) {
        xmin = (SWX < NWX) ? SWX : NWX;
        ymin = (SWY < SEY) ? SWY : SEY;
        xmin = width * (long) (xmin / width + 1);
        ymin = height * (long) (ymin / height + 1);
    }

    /* South west corner */
    ycor[1] = SWY;
    xcor[1] = SWX;
    /* North west corner */
    ycor[2] = NWY;
    xcor[2] = NWX;
    /* North east corner */
    ycor[3] = NEY;
    xcor[3] = NEX;
    /* South east corner */
    ycor[4] = SEY;
    xcor[4] = SEX;

    gmin = INT_MAX;
    gmax = INT_MIN;
    resolu[1] = width;
    resolu[2] = height;
    resolu[3] = 1.0 / vscale;

    /* Read in the cell values for each column, starting at the bottom row */
    irec = 1;
    sprintf(buffer, "%-144s%6d%6d%6d%6ld", title, ilevel, ipttrn, icoord,
            izone);
    for (i = 1; i <= 15; i++) {
        sprintf(short_buff, "%24.15e", dproj[i]);
        e2d(short_buff);        /* changes 'e' format to Fortran 'D' format */
        strcat(buffer, short_buff);
    }
    sprintf(short_buff, "%6d%6d%6d", gnd_units, elev_units, isides);
    strcat(buffer, short_buff);

    for (i = 1; i <= 4; i++) {
        sprintf(short_buff, "%24.15e%24.15e", xcor[i], ycor[i]);
        e2d(short_buff);        /* changes 'e' format to Fortran 'D' format */
        strcat(buffer, short_buff);
    }

    sprintf(short_buff,"%24.15e%24.15e%24.15e%6d",minvalue,maxvalue,dphi,iaccu);
    e2d(short_buff);            /* changes 'e' format to Fortran 'D' format */
    strcat(buffer, short_buff);

    for (i = 1; i <= 3; i++) {
        sprintf(short_buff, "%12.5e", resolu[i]);
        strcat(buffer, short_buff);
    }

    sprintf(short_buff, "%6d%6d", irow, icol);
    strcat(buffer, short_buff);
    /* The following is to support arcinfo extensions for datum info */
    strcat(buffer, "                         2");
    sprintf(short_buff, "%2d", datum);
    strcat(buffer, short_buff);
    fprintf(fpdem, "%-1024s", buffer);
    if (newlines) {
        fprintf(fpdem, "\n");
    }
    fflush(fpdem);

    psent = 0;
    irec = irec + 1;
    for (col = 1; col <= ncols; col++) {
        pnow = PDOTS * col / ncols;
        while (psent < pnow) {
            fprintf(stderr, ".");
            fflush(stderr);
            psent++;
        }
        strcpy(buffer, "");

        for (row = nrows; row >= 1; row--) {
            /* Get the cell values at the row and column */
            icell = (row - 1) * ncols + (col - 1);
            cellval = (int) cells[icell];
            if (cellval == MISSING_STORED)
                cellval = MISSING_VALUE;
            else
                cellval = cellval - ELEVATION_BIAS;

            /* Check for sea level values */
            if (cellval == 0)
               cellval = MISSING_VALUE;

            /* Assign to dem format */
            idem[nrows - row + 1] = cellval;
        }

        /* Figure out the bounds of the valid (not missing) data */
        for (ilast = nrows; ilast>1 && idem[ilast]==MISSING_VALUE; ilast--)
            ;
        for (ifirst = 1; ifirst<ilast && idem[ifirst]==MISSING_VALUE; ifirst++)
            ;
        nvals = ilast - ifirst + 1;

        /* Calculate column min and max */
        cmin = INT_MAX;
        cmax = INT_MIN;
        for (row = ifirst; row <= ilast; row++) {
            if (idem[row] != MISSING_VALUE) {
                if (cmin > idem[row])  cmin = idem[row];
                if (cmax < idem[row])  cmax = idem[row];
            }
        }
        if (cmin > cmax)
            cmin = cmax = MISSING_VALUE;
        else {
            if (gmin > cmin)  gmin = cmin;
            if (gmax < cmax)  gmax = cmax;
        }

        xgo = xmin + (float) (col - 1) * width;
        ygo = ymin + (float) (ifirst - 1) * height;
        /*
          Write b records. Note that the first record for each column
          will include the header and, at most, 146 elevations. Each
          subsequent record for the column can hold 170 elevations.
        */
        nfirst = ifirst;
        nlast = ifirst + 146 - 1;
        if (nlast > ilast)
            nlast = ilast;
        sprintf(buffer, "%6d%6d%6d     1%24.15e%24.15e%24.15e%24.15e%24.15e",
            imrow, col, nvals, xgo, ygo, elodat,
            cmin / (float) vscale, cmax / (float) vscale);
        e2d(buffer + 24);       /* changes 'e' or 'E' to Fortran 'D' */
        p = buffer + 144;       /* p points to end of string so far */
        for (n = nfirst; n <= nlast; n++) {
            sprintf(p, "%6d", idem[n]);
            p += 6;
            if (idem[n] == MISSING_VALUE)
                missing++;
        }
        while (p < buffer + 1024)
            *p++ = ' ';
        *p = '\0';
        fputs(buffer, fpdem);
        if (newlines) {
            fprintf(fpdem, "\n");
        }

        irec = irec + 1;
        while (nlast != ilast) {
            /* We have to process addition output lines */
            nfirst = nlast + 1;
            nlast = nlast + 170;
            if (nlast > ilast)
                nlast = ilast;

            p = buffer;
            for (n = nfirst; n <= nlast; n++) {
                sprintf(p, "%6d", idem[n]);
                p += 6;
                if (idem[n] == MISSING_VALUE)
                    missing++;
            }
            while (p < buffer + 1024)
                *p++ = ' ';
            *p = '\0';
            fputs(buffer, fpdem);
            if (newlines) {
                fprintf(fpdem, "\n");
            }
            irec = irec + 1;

        }
    }

    /* Write record c */
    strcpy(buffer, " ");
    for (i = 1; i <= 10; i++) {
        sprintf(short_buff, "%6d", crecord[i]);
        strcat(buffer, short_buff);
    }
    fprintf(fpdem, "%-1024s", buffer);
    if (newlines) {
        fprintf(fpdem, "\n");
    }

    /* Write a summary to stderr */
    fprintf(stderr, "\n\n   Final Output USGS DEM:\n");
    fprintf(stderr, "Min:           %8d\n", gmin);
    fprintf(stderr, "Max:           %8d\n", gmax);
    if (missing > 0) {
        fprintf(stderr, "Missing Value: %8d\n", MISSING_VALUE);
        fprintf(stderr, "Missing Count: %8d\n", missing);
    }
    fprintf(stderr, "Rows:          %8ld\n", nrows);
    fprintf(stderr, "Cols:          %8ld\n\n", ncols);
    fprintf(stderr, "   Quad Boundary \n");
    fprintf(stderr, "SW %12.2f %12.2f\n", SWX, SWY);
    fprintf(stderr, "NW %12.2f %12.2f\n", NWX, NWY);
    fprintf(stderr, "NE %12.2f %12.2f\n", NEX, NEY);
    fprintf(stderr, "SE %12.2f %12.2f\n", SEX, SEY);
}

/*************************************************************************

            s u b r o u t i n e     p g m o u t
 function:
             this routine reads in the cell values for a cell map
             and writes it out in pgm format
 routines
 called:

  programmer
  sol katz, blm/sc344, sept 93

  revised by:
   sol katz, blm, march 1998  to work with generic grids
   stefan roettger, january 2005

*************************************************************************/

void pgm_out(unsigned short cells[], int *number_cells) {

    double xcor[5], ycor[5];
    double resolu[4];

    int psent, pnow;

    int row, col;
    int icell, cellval;

    fprintf(stderr, "Writing output PGM file ");

    /* South west corner */
    xcor[1] = upperlx;
    ycor[1] = upperly - (nrows - 1) * height;
    /* North west corner */
    xcor[2] = upperlx;
    ycor[2] = upperly;
    /* North east corner */
    xcor[3] = upperlx + (ncols - 1) * width;
    ycor[3] = upperly;
    /* South east corner */
    xcor[4] = upperlx + (ncols - 1) * width;
    ycor[4] = upperly - (nrows - 1) * height;

    resolu[1] = width;
    resolu[2] = height;
    resolu[3] = 1.0 / vscale;

    if (elev_units==1) resolu[3] *= 0.3048; /* conversion from feet to meters */

    psent = 0;

    /* Write PGM header: */

    fprintf(fppgm, "P5\n");

    fprintf(fppgm, "# DEM\n");
    fprintf(fppgm, "# description=%s\n", title);

    fprintf(fppgm, "# coordinate system=UTM\n");
    fprintf(fppgm, "# coordinate zone=%d\n", izone);
    fprintf(fppgm, "# coordinate datum=%d\n", datum);

    fprintf(fppgm, "# SW corner=%12f/%12f meters\n", xcor[1], ycor[1]);
    fprintf(fppgm, "# NW corner=%12f/%12f meters\n", xcor[2], ycor[2]);
    fprintf(fppgm, "# NE corner=%12f/%12f meters\n", xcor[3], ycor[3]);
    fprintf(fppgm, "# SE corner=%12f/%12f meters\n", xcor[4], ycor[4]);

    fprintf(fppgm, "# cell size=%g/%g meters\n", resolu[1], resolu[2]);
    fprintf(fppgm, "# vertical scaling=%g meters\n", resolu[3]);

    fprintf(fppgm, "# missing value=%d\n", MISSING_VALUE);

    fprintf(fppgm, "%d %d\n", ncols, nrows);
    fprintf(fppgm, "32767\n");

    /* Read in the cell values for each column */
    for (row = 1; row <= nrows; row++) {
        pnow = PDOTS * row / nrows;
        while (psent < pnow) {
            fprintf(stderr, ".");
            fflush(stderr);
            psent++;
        }

        for (col = 1; col <= ncols; col++) {
            /* Get the cell values at the row and column */
            icell = (row - 1) * ncols + (col - 1);
            cellval = (int) cells[icell];
            if (cellval == MISSING_STORED)
                cellval = MISSING_VALUE;
            else
                cellval = cellval - ELEVATION_BIAS;

            /* Check for sea level values */
            if (cellval == 0)
               cellval = MISSING_VALUE;

            /* Output elevation */
            if (cellval>=0) {
               fputc(cellval/256, fppgm);
               fputc(cellval%256, fppgm);
            }
            else {
               fputc((65536+cellval)/256, fppgm);
               fputc((65536+cellval)%256, fppgm);
            }
        }
    }

    fprintf(stderr, "\n");
}

/*********************************************/
void get_nw_corner(void) {
    long sadr_x, sadr_y;

    strcpy(file_name, base_name);
    strcat(file_name, "RSDF.DDF");
    if (!beg123file(file_name, 'R', &int_level, &ice, ccs, &fpin)) {
        fprintf(stderr, "\nERROR OPENING FILE %s", file_name);
        exit(0);
    }
    if (!rd123ddrec(fpin,               /* file pointer */
                    string,             /* DDR record returned */
                    &status)) {         /* status returned */
        fprintf(stderr, "\n*** ERROR READING DDR ***");

        goto done;

    }
    status = -1;

    /* Loop to process each subfield in Raster Definition module */
    do {

        /* Read data record subfield */
        if (!rd123sfld(fpin,            /* file pointer */
                       tag,             /* field tag returned */
                       &leadid,         /* leader identifier returned */
                       string,          /* subfield contents returned */
                       &str_len,        /* string length */
                       &status)) {      /* status returned */

            fprintf(stderr,
                    "\nERROR READING DATA RECORD SUBFIELD (IDEN MODULE)");
            goto done;

        }

        /* Retrieve description of current subfield */
        if (!chk123sfld(fpin,           /* file pointer */
                        tag,            /* tag output */
                        descr,          /* subfield descriptions output */
                        frmts)) {       /* subfield format control */
            fprintf(stderr, "\nERROR CHECKING DATA RECORD SUBFIELD");
            goto done;
        }

        /* Check subfield name and extract contents for each subfield */
        if (!strcmp(tag, "SADR") && !strcmp(descr, "X")) {

            /* Binary data, convert character string returned by rd123sfld
               to a long integer, changing bit order if necessary */
            if (strstr(frmts, "B") != NULL) {
                if (!order)
                    s123tol(string, &sadr_x, 1);
                else
                    s123tol(string, &sadr_x, 0);
            } else if (strstr(frmts, "R")) {
                sadr_x = (long) atof(string);
            }
        } else if (!strcmp(tag, "SADR") && !strcmp(descr, "Y")) {

            /* Binary data, convert character string returned by rd123sfld
               to a long integer, changing bit order if necessary */
            if (strstr(frmts, "B") != NULL) {
                if (!order)
                    s123tol(string, &sadr_y, 1);
                else
                    s123tol(string, &sadr_y, 0);
            } else if (strstr(frmts, "R")) {
                sadr_y = (long) atof(string);
            }
        }
    } while (status != 4);      /* Break out of loop at end of file */

done:
    end123file(&fpin);

    upperlx = (sadr_x * sfax) + xorg;
    upperly = (sadr_y * sfay) + yorg;
    return;
}

/*********************************************/
void get_iref(void) {

    /* Set some default values */
    sfax = 1.0;
    sfay = 1.0;
    xorg = 0.0;
    yorg = 0.0;
    strcpy(file_name, base_name);
    strcat(file_name, "IREF.DDF");
    if (!beg123file(file_name, 'R', &int_level, &ice, ccs, &fpin)) {
        fprintf(stderr, "\nERROR OPENING FILE %s", file_name);
        exit(0);
    }
    if (!rd123ddrec(fpin,               /* file pointer */
                    string,             /* DDR record returned */
                    &status)) {         /* status returned */
        fprintf(stderr, "\n*** ERROR READING DDR ***");
        goto done;

    }
    status = -1;

    /* Loop to process each subfield in Identification module */
    do {

        /* Read data record subfield */
        if (!rd123sfld(fpin,            /* file pointer */
                       tag,             /* field tag returned */
                       &leadid,         /* leader identifier returned */
                       string,          /* subfield contents returned */
                       &str_len,        /* string length */
                       &status)) {      /* status returned */

            fprintf(stderr,
                    "\nERROR READING DATA RECORD SUBFIELD (IDEN MODULE)");
            goto done;

        }

        /* Retrieve description of current subfield */
        if (!chk123sfld(fpin,           /* file pointer */
                        tag,            /* tag output */
                        descr,          /* subfield descriptions output */
                        frmts)) {       /* subfield format control */
            fprintf(stderr, "\nERROR CHECKING DATA RECORD SUBFIELD");
            goto done;
        }

        /* Check subfield name and extract contents for each subfield */
        if (!strcmp(tag, "IREF") && !strcmp(descr, "SFAX"))
            sfax = atof(string);
        else if (!strcmp(tag, "IREF") && !strcmp(descr, "SFAY"))
            sfay = atof(string);
        else if (!strcmp(tag, "IREF") && !strcmp(descr, "XORG"))
            xorg = atof(string);
        else if (!strcmp(tag, "IREF") && !strcmp(descr, "YORG"))
            yorg = atof(string);
        else if (!strcmp(tag, "IREF") && !strcmp(descr, "XHRS"))
            xhrs = atof(string);
        else if (!strcmp(tag, "IREF") && !strcmp(descr, "YHRS"))
            yhrs = atof(string);

    } while (status != 4);      /* Break out of loop at end of file */

    width = xhrs;
    height = yhrs;

done:
    end123file(&fpin);
    return;
}

/*********************************************/
void get_elev_units(void) {

    strcpy(file_name, base_name);
    strcat(file_name, "DDSH.DDF");
    if (!beg123file(file_name, 'R', &int_level, &ice, ccs, &fpin)) {
        fprintf(stderr, "\nERROR OPENING FILE %s", file_name);
        exit(0);
    }
    if (!rd123ddrec(fpin,               /* file pointer */
                    string,             /* DDR record returned */
                    &status)) {         /* status returned */
        fprintf(stderr, "\n*** ERROR READING DDR ***");
        goto done;

    }
    status = -1;

    /* Default elevation to meters */
    elev_units = 2;

    /* Loop to process each subfield in Identification module */
    do {

        /* Read data record subfield */
        if (!rd123sfld(fpin,            /* file pointer */
                       tag,             /* field tag returned */
                       &leadid,         /* leader identifier returned */
                       string,          /* subfield contents returned */
                       &str_len,        /* string length */
                       &status)) {      /* status returned */

            fprintf(stderr,
                    "\nERROR READING DATA RECORD SUBFIELD (IDEN MODULE)");
            goto done;

        }

        /* Retrieve description of current subfield */
        if (!chk123sfld(fpin,           /* file pointer */
                        tag,            /* tag output */
                        descr,          /* subfield descriptions output */
                        frmts)) {       /* subfield format control */
            fprintf(stderr, "\nERROR CHECKING DATA RECORD SUBFIELD");
            goto done;
        }

        /* Check subfield name and extract contents for each subfield */
        if (!strcmp(tag, "DDSH") && !strcmp(descr, "UNIT")) {
            if (!strcmp("FEET", string))
                elev_units = 1;
            else if (!strcmp("METERS", string))
                elev_units = 2;
            else
                fprintf(stderr, "\nUnknown elevation units:       %s\n",
                        string);
        } else if (!strcmp(tag, "DDSH") && !strcmp(descr, "FMT")) {
            if (!strcmp("BFP32", string))
                vscale = 10;
            else
                vscale = 1;
        }
    } while (status != 4);      /* Break out of loop at end of file */

done:
    end123file(&fpin);
    if (elev_units == 1)        /* if elevations in feet */
        vscale = 1;             /* don't dare scale feet by 10 */
    return;
}

/******************************************************************

   e2d  - converts all occurrences of an "e" or an "E" in a string
          to a "D". Designed to convert C-type exponential notations
          to Fortran-type notations [the specifications for the
          7.5-min DEMs use "D" type].

          Return value: number of instances of e-->D conversion

*******************************************************************/
int e2d(char *s) {
    int k = 0;
    while (*s != '\0') {
        if ((*s == 'e') || (*s == 'E')) {
            *s = 'D';
            k++;
        }
        s++;
    }
    return (k);
}

/*********************************************
 chomp - removes a new line from the end of a string
*********************************************/
void chomp(char *s) {
    int nc;
    nc = strlen(s);
    if (nc > 0) {
        if (*(s + nc - 1) == '\n') {
            *(s + nc - 1) = '\0';
            nc--;
        }
    }
}

/*******************************************************
 units(n) - translates an integer unit code to string
*******************************************************/
char *units(int n) {
    switch (n) {
        case 0:  return "radians";
        case 1:  return "feet";
        case 2:  return "meters";
        case 3:  return "arc-seconds";
        default: return "units";
    }
}
