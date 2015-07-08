/*
 * BSDv2 License
 * Copyright 2005 Wojciech A. Koszek <wkoszek@FreeBSD.org>
 */
#include <sys/types.h>
#include <sys/param.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <sys/uio.h>
#include <unistd.h>

#include "sun_disklabel.h"

static char *prog = NULL;

static void
usage(void)
{
	(void) fprintf(stderr, "usage: %s <disk>\n", prog);
	exit(EX_USAGE);
}

int
block_differ(char *b1, char *b2, int len)
{
	int idx;
	for (idx = 0;idx < len; idx++)
		if (b1[idx] != b2[idx])
			return -idx;
	return (0);
}

#define BLOCKS 1024*1024

int
main (int argc, char **argv)
{
	char *p;
	int fd, rfd, r, error = 0;
	char bbuf[BLOCKS];
	char dbuf[BLOCKS];
	char sec0[512];
	struct sun_x86disklabel dl, *dlp;

	/* Open disk given in command line arguments*/
	if (argc != 2)
		prog = *argv, usage();
	p = *++argv;
	fd = open(p, O_RDONLY);
	if (fd == -1) {
		(void) fprintf(stderr, "Couldn't open file\n");
		exit(EXIT_FAILURE);
	}
	/* Sample disks fetched from local disk */
	rfd = open("data/disk/c1d1s1", O_RDONLY);
	if (rfd == -1) {
		(void) fprintf(stderr, "Couldn't open disk\n");
		exit(EXIT_FAILURE);
	}
	if (read(rfd, bbuf, sizeof(bbuf)) != sizeof(bbuf)) {
		(void) fprintf(stderr, "Couldn't read!\n");
		exit(EXIT_FAILURE);
	}
	/* Requires offset of 1 sector = 512 bytes */
	if (lseek(fd, 512, SEEK_CUR) < 0) {
		(void) fprintf(stderr, "Couldn't open file");
		return (EXIT_FAILURE);
	}
	/* Read first sector*/
	if (read(fd, sec0, sizeof(sec0)) != sizeof(sec0)) {
		(void) fprintf(stderr, "Couldn't get size of sector\n");
		exit(EXIT_FAILURE);
	}
	/* Back */
	if (lseek(fd, 512, SEEK_SET) < 0) {
		(void) fprintf(stderr, "Couldn't open file");
		return (EXIT_FAILURE);
	}
	/* Fetch needed information */
	bzero((struct sun_x86disklabel *) &dl, sizeof(dl));
	error = sunx86label_dec(sec0, &dl);
	sunx86label_print(&dl);
	sunx86label_print_part(&dl);

	dlp = &dl;
	printf("Offset: %d\n",
		dlp->sl_vtoc.slvc_part[0].sdkp.sdkp_cyloffset);
	lseek(fd, dlp->sl_vtoc.slvc_part[0].sdkp.sdkp_cyloffset * 512,
		SEEK_CUR);
	printf("Size: %d\n", dlp->sl_ntracks * dlp->sl_nsectors);
	printf("Size*off: %d\n", dlp->sl_ntracks * dlp->sl_nsectors * dlp->sl_vtoc.slvc_part[0].sdkp.sdkp_cyloffset);

/*	sl->sl_ntracks = le32dec(&t->sl_ntracks);
	sl->sl_nsectors = le32dec(&t->sl_nsectors);
*/
	
	read(fd, dbuf, BLOCKS);
	if (memcmp(dbuf, bbuf, BLOCKS) == 0) {
		fprintf(stderr, "THE SAME!\n");
	}
	else {
		fprintf(stderr, "OTHER!\n");
	}
	exit(0);
		
/*
sl->sl_vtoc.slvc_part[i].sdkp.sdkp_cyloffset,
sl->sl_vtoc.slvc_part[i].sdkp.sdkp_nsectors);
*/
	r = block_differ(bbuf, dbuf, BLOCKS);
	if (r == 0)
		printf("Disks the same!\n");
	else
		printf("Disks differ at %d\n", -r);
	if (close(rfd) != 0)
		(void) fprintf(stderr, "Couldn't close file\n");
	if (close(fd) != 0)
		(void) fprintf(stderr, "Couldn't close file\n");
	exit(EXIT_SUCCESS);
}
