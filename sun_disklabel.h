/*-
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 * Copyright (c) 2004, Joerg Wunsch
 * Copyright (c) 2005, Wojciech A. Koszek
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)sun_disklabel.h	8.1 (Berkeley) 6/11/93
 *	$NetBSD: disklabel.h,v 1.2 1998/08/22 14:55:28 mrg Exp $
 *
 * $FreeBSD: src/sys/sys/sun_disklabel.h,v 1.9.2.1 2005/01/31 23:26:57 imp Exp $ 
 */

#ifndef _SYS_SUN_DISKLABEL_H_
#define	_SYS_SUN_DISKLABEL_H_

/*
 * SunOS/Solaris disk label layout (partial).
 * 
 * Suns disk label format contains a lot of historical baggage which we 
 * ignore entirely.  The structure below contains the relevant bits and the
 * _enc/_dec functions encode/decode only these fields. Layout of labels
 * differ between platforms, so separate structure is needed to describe
 * SunOS/Solaris labels on x86.
 */

#define	SUN_DKMAGIC	55998
#define	SUN_NPART	8
#define	SUN_X86NPART	16
#define	SUN_RAWPART	2
#define	SUN_SIZE	512
#define	SUN_VTOC_VERSION 1
#define	SUN_VTOC_SANE	0x600DDEEE /* SVR4-compatible VTOC is "sane". */
/*
 * XXX: I am actually not sure if this should be "16 sectors" or "8192 bytes".
 * XXX: Considering that Sun went to the effort of getting 512 byte compatible
 * XXX: CDROM drives produced my guess is that Sun computers stand little or
 * XXX: even no chance of running, much less booting on !=512 byte media.
 * XXX: Define this is in terms of bytes since that is easier for us.
 */
#define	SUN_BOOTSIZE	8192
#define	SUN_VOLNAME_LEN 8
#define	SUN_TEXT_LEN	128
/*
 * To get label information we need to skip first 512 bytes.
 */
#define	SUN_VTOC_OFFSET	512	

/* partition info */
struct sun_dkpart {
	u_int32_t	sdkp_cyloffset;		/* starting cylinder */
	u_int32_t	sdkp_nsectors;		/* number of sectors */
};

struct sun_vtoc_info {
	u_int16_t	svtoc_tag;		/* partition tag */
	u_int16_t	svtoc_flag;		/* partition flags */
};

/* For x86 VTOC holds partition information */
struct sun_x86dkpart {
	struct sun_vtoc_info	svti;
	struct sun_dkpart	sdkp;
};

/* known partition tag values */
#define	VTOC_UNASSIGNED	0x00
#define	VTOC_BOOT	0x01
#define	VTOC_ROOT	0x02
#define	VTOC_SWAP	0x03
#define	VTOC_USR	0x04
#define	VTOC_BACKUP	0x05	/* "c" partition, covers entire disk */
#define	VTOC_STAND	0x06
#define	VTOC_VAR	0x07
#define	VTOC_HOME	0x08
#define	VTOC_ALTSCTR	0x09	/* alternate sector partition */
#define	VTOC_CACHE	0x0a	/* Solaris cachefs partition */
#define	VTOC_VXVM_PUB	0x0e	/* VxVM public region */
#define	VTOC_VXVM_PRIV	0x0f	/* VxVM private region */

/* VTOC partition flags */
#define	VTOC_UNMNT	0x01	/* unmountable partition */
#define	VTOC_RONLY	0x10	/* partition is read/only */

struct sun_disklabel {
	char		sl_text[SUN_TEXT_LEN];

	/* SVR4 VTOC information */
	u_int32_t	sl_vtoc_vers;		/* == SUN_VTOC_VERSION */
	u_int16_t	sl_vtoc_nparts;		/* == SUN_NPART */
	struct sun_vtoc_info sl_vtoc_map[SUN_NPART]; /* partition tag/flag */
	u_int32_t	sl_vtoc_sane;		/* == SUN_VTOC_SANE */

	/* Sun label information */
	u_int16_t	sl_rpm;			/* rotational speed */
	u_int16_t	sl_pcylinders;		/* number of physical cyls */
	u_int16_t	sl_sparespercyl;	/* spare sectors per cylinder */
	u_int16_t	sl_interleave;		/* interleave factor */
	u_int16_t	sl_ncylinders;		/* data cylinders */
	u_int16_t	sl_acylinders;		/* alternate cylinders */
	u_int16_t	sl_ntracks;		/* tracks per cylinder */
	u_int16_t	sl_nsectors;		/* sectors per track */
	struct sun_dkpart sl_part[SUN_NPART];	/* partition layout */
	u_int16_t	sl_magic;		/* == SUN_DKMAGIC */
};

struct sun_x86vtoc {
	u_int32_t	slvc_binfo[3];	/* information used by mboot (unused) */
	u_int32_t	slvc_sane;	/* Sanity number */
	u_int32_t	slvc_version; /* VTOC version */
	char		slvc_volname[SUN_VOLNAME_LEN]; /* volume name*/
	u_int16_t	slvc_secsize;	/* sector size */
	u_int16_t	slvc_npart;	/* number of partitions */
	u_int32_t	slvc_unused[10]; /* unused (reserved?) */
	struct	sun_x86dkpart slvc_part[SUN_X86NPART];
	u_int32_t	slvc_tstamp[SUN_X86NPART]; /* timestamp */
	char		slvc_text[SUN_TEXT_LEN]; /* ascii name */
};

/*
 * Solaris label layout for x86 platform
 */
struct sun_x86disklabel {
	struct	sun_x86vtoc	sl_vtoc;
	u_int32_t	sl_pcylinders;		/* number of physical cyls */
	u_int32_t	sl_ncylinders;		/* data cylinders */
	u_int16_t	sl_acylinders;		/* alternate cylinders */
	u_int16_t	sl_bcylinders;		/* alternate cylinders */
	u_int32_t	sl_ntracks;		/* tracks per cylinder */
	u_int32_t	sl_nsectors;		/* sectors per track */
	u_int16_t	sl_interleave;		/* interleave factor */
	u_int16_t	sl_trackskew;		/* XXX: ? */
	u_int16_t	sl_alterc;		/* SCSI */
	u_int16_t	sl_rpm;			/* rotations per minute */
	u_int16_t	sl_skipwrite;		/* skipped sectors while writing */
	u_int16_t	sl_skipread;		/* skipped sectors while reading */
	u_int16_t	sl_unused[4];
	char		sl_pad[12];		/* padding */
	u_int16_t	sl_magic;		/* should be equal to SUN_DKMAGIC */
	u_int16_t	sl_cksum;		/* checksum */
};
int sunlabel_dec(void const *pp, struct sun_disklabel *sl);
int sunx86label_dec(void const *pp, struct sun_x86disklabel *sl);
void sunlabel_enc(void *pp, struct sun_disklabel *sl);
void sunx86label_enc(void *pp, struct sun_x86disklabel *sl);
void sunx86label_print(struct sun_x86disklabel *sl);
void sunx86label_print_part(struct sun_x86disklabel *sl);
#endif /* _SYS_SUN_DISKLABEL_H_ */
