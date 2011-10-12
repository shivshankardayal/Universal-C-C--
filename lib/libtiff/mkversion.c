/* "$Header: /public/ext3/ivm/lib/libtiff/mkversion.c,v 1.1.1.1 2001/08/22 11:50:09 bob Exp $ */

/*
 * Copyright (c) 1995-1997 Sam Leffler
 * Copyright (c) 1995-1997 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 * 
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

/*
 * Generate a library version string for systems that
 * do not have a shell (by default this is done with
 * awk and echo from the Makefile).
 *
 * This was written by Peter Greenham for Acorn systems.
 *
 * Syntax: mkversion [-v version-file] [-a alpha-file] [<outfile>]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
usage(void)
{
    fprintf(stderr,
	"usage: mkversion [-v version-file] [-a alpha-file] [outfile]\n");
    exit(-1);
}

static FILE*
openFile(char* filename)
{
    FILE* fd = fopen(filename, "r");
    if (fd == NULL) {
	fprintf(stderr, "mkversion: %s: Could not open for reading.\n",
	    filename);
	exit(-1);
    }
    return (fd);
}

int
main(int argc, char* argv[])
{
    char* versionFile = "../VERSION";
    char* alphaFile = "../dist/tiff.alpha";
    char version[128];
    char alpha[128];
    FILE* fd;
    char* cp;

    argc--, argv++;
    while (argc > 0 && argv[0][0] == '-') {
	if (strcmp(argv[0], "-v") == 0) {
	    if (argc < 1)
		usage();
	    argc--, argv++;
	    versionFile = argv[0];
	} else if (strcmp(argv[0], "-a") == 0) {
	    if (argc < 1)
		usage();
	    argc--, argv++;
	    alphaFile = argv[0];
	} else
	    usage();
	argc--, argv++;
    }
    fd = openFile(versionFile);
    if (fgets(version, sizeof (version)-1, fd) == NULL) {
	fprintf(stderr, "mkversion: No version information in %s.\n",
	    versionFile);
	exit(-1);
    }
    cp = strchr(version, '\n');
    if (cp)
	*cp = '\0';
    fclose(fd);
    fd = openFile(alphaFile);
    if (fgets(alpha, sizeof (alpha)-1, fd) == NULL) {
	fprintf(stderr, "mkversion: No alpha information in %s.\n", alphaFile);
	exit(-1);
    }
    fclose(fd);
    cp = strchr(alpha, ' ');		/* skip to 3rd blank-separated field */
    if (cp)
	cp = strchr(cp+1, ' ');
    if (cp) {				/* append alpha to version */
	char* tp;
	for (tp = strchr(version, '\0'), cp++; (*tp = *cp) != 0; tp++, cp++)
	    ;
	if (tp[-1] == '\n')
	    tp[-1] = '\0';
    } else {
	fprintf(stderr, "mkversion: Malformed alpha information in %s.\n",
	    alphaFile);
	exit(-1);
    }
    if (argc > 0) {
	fd = fopen(argv[0], "w");
	if (fd == NULL) {
	    fprintf(stderr, "mkversion: %s: Could not open for writing.\n",
		argv[0]);
	    exit(-1);
	}
    } else
	fd = stdout;
    fprintf(fd, "#define VERSION \"LIBTIFF, Version %s\\n", version);
    fprintf(fd, "Copyright (c) 1988-1996 Sam Leffler\\n");
    fprintf(fd, "Copyright (c) 1991-1996 Silicon Graphics, Inc.\"\n");

    if (fd != stdout)
	fclose(fd);
    return (0);
}
