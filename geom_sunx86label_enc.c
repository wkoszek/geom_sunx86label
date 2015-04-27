/*-
 * Copyright (c) 2005 Wojciech A. Koszek
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

 
#include <sys/cdefs.h>

#include <sys/types.h>
#include <sys/endian.h>
#include <sys/errno.h>

#include <stdio.h>

#include "sun_disklabel.h"

/*
 * Parts of this code were borrowed from geom_sunlabel_enc.c
 */
int
sunx86label_dec(void const *pp, struct sun_x86disklabel *sl)
{
	const uint8_t *p = pp;
	size_t i;
	uint32_t vtocsane;
	uint16_t npart, u;
	struct sun_x86disklabel *t;

	/*
	 * We use casting to make structure layout independent from hardcoded
	 * offsets.
	 */
	t = (struct sun_x86disklabel *) pp;
	for (i = u = 0; i < (SUN_SIZE - sizeof(sl->sl_cksum)); i += 2)
		u ^=  *((uint16_t *)(p + i));
	if (t->sl_magic != SUN_DKMAGIC || t->sl_cksum != u)
		return (EINVAL);
	for (i = 0; i < sizeof(sl->sl_vtoc.slvc_text); i++)
		sl->sl_vtoc.slvc_text[i] = t->sl_vtoc.slvc_text[i];
	sl->sl_pcylinders = le32dec(&t->sl_pcylinders);
	sl->sl_ncylinders = le32dec(&t->sl_ncylinders);
	sl->sl_acylinders = le16dec(&t->sl_acylinders);
	sl->sl_bcylinders = le16dec(&t->sl_bcylinders);
	sl->sl_ntracks = le32dec(&t->sl_ntracks);
	sl->sl_nsectors = le32dec(&t->sl_nsectors);
	sl->sl_interleave = le16dec(&t->sl_interleave);
	sl->sl_trackskew = le16dec(&t->sl_trackskew);
	sl->sl_alterc = le16dec(&t->sl_alterc);
	sl->sl_rpm = le16dec(&t->sl_rpm);
	sl->sl_skipwrite = le16dec(&t->sl_skipwrite);
	sl->sl_skipread = le16dec(&t->sl_skipread);
	sl->sl_magic = le16dec(&t->sl_magic);
	sl->sl_cksum = le16dec(&t->sl_cksum);

	vtocsane = le32dec(&t->sl_vtoc.slvc_sane);
	npart = le16dec(&t->sl_vtoc.slvc_npart);
	if (vtocsane == SUN_VTOC_SANE && npart == SUN_X86NPART) {
		/*
		 * Decode SVR4-compatible VTOC information
		 */
		sl->sl_vtoc.slvc_sane = vtocsane;
		sl->sl_vtoc.slvc_npart = npart;
		sl->sl_vtoc.slvc_binfo[0] = le32dec(&t->sl_vtoc.slvc_binfo[0]);
		sl->sl_vtoc.slvc_binfo[1] = le32dec(&t->sl_vtoc.slvc_binfo[1]);
		sl->sl_vtoc.slvc_binfo[2] = le32dec(&t->sl_vtoc.slvc_binfo[2]);
		sl->sl_vtoc.slvc_version = le32dec(&t->sl_vtoc.slvc_version);
		for (i = 0; i < SUN_VOLNAME_LEN; i++)
			sl->sl_vtoc.slvc_volname[i] = t->sl_vtoc.slvc_volname[i];
		sl->sl_vtoc.slvc_secsize = le16dec(&t->sl_vtoc.slvc_secsize);
		sl->sl_vtoc.slvc_npart = le16dec(&t->sl_vtoc.slvc_npart);
		for (i = 0; i < SUN_X86NPART; i++) {
			sl->sl_vtoc.slvc_part[i].svti.svtoc_tag =
			le32dec(&t->sl_vtoc.slvc_part[i].svti.svtoc_tag);
			sl->sl_vtoc.slvc_part[i].svti.svtoc_flag =
			le32dec(&t->sl_vtoc.slvc_part[i].svti.svtoc_flag);
			sl->sl_vtoc.slvc_part[i].sdkp.sdkp_cyloffset =
			le32dec(&t->sl_vtoc.slvc_part[i].sdkp.sdkp_cyloffset);
			sl->sl_vtoc.slvc_part[i].sdkp.sdkp_nsectors =
			le32dec(&t->sl_vtoc.slvc_part[i].sdkp.sdkp_nsectors);
			sl->sl_vtoc.slvc_tstamp[i] = le32dec(&t->sl_vtoc.slvc_tstamp[i]);
		}
	}
	return (0);
}

void
sunx86label_enc(void *pp, struct sun_x86disklabel *sl)
{
	const uint8_t *p = pp;
	size_t i;
	uint32_t vtocsane;
	uint16_t npart, u;
	struct sun_x86disklabel *t;

	t = (struct sun_x86disklabel *) p;
	
	for (i = 0; i < sizeof(sl->sl_vtoc.slvc_text); i++)
		sl->sl_vtoc.slvc_text[i] = t->sl_vtoc.slvc_text[i];
	le32enc(&sl->sl_pcylinders, t->sl_pcylinders);
	le32enc(&sl->sl_ncylinders, t->sl_ncylinders);
	le16enc(&sl->sl_acylinders, t->sl_acylinders);
	le16enc(&sl->sl_bcylinders, t->sl_bcylinders);
	le32enc(&sl->sl_ntracks, t->sl_ntracks);
	le32enc(&sl->sl_nsectors, t->sl_nsectors);
	le16enc(&sl->sl_interleave, t->sl_interleave);
	le16enc(&sl->sl_trackskew, t->sl_trackskew);
	le16enc(&sl->sl_alterc, t->sl_alterc);
	le16enc(&sl->sl_rpm, t->sl_rpm);
	le16enc(&sl->sl_skipwrite, t->sl_skipwrite);
	le16enc(&sl->sl_skipread, t->sl_skipread);
	le16enc(&sl->sl_magic, t->sl_magic);

	vtocsane = t->sl_vtoc.slvc_sane;
	npart = t->sl_vtoc.slvc_npart;
	if (vtocsane == SUN_VTOC_SANE && npart == SUN_X86NPART) {
		/*
		 * Decode SVR4-compatible VTOC information
		 */
		sl->sl_vtoc.slvc_sane = vtocsane;
		sl->sl_vtoc.slvc_npart = npart;
		le32enc(&sl->sl_vtoc.slvc_binfo[0], t->sl_vtoc.slvc_binfo[0]);
		le32enc(&sl->sl_vtoc.slvc_binfo[1], t->sl_vtoc.slvc_binfo[1]);
		le32enc(&sl->sl_vtoc.slvc_binfo[2], t->sl_vtoc.slvc_binfo[2]);
		le32enc(&(sl->sl_vtoc.slvc_version), t->sl_vtoc.slvc_version);
		for (i = 0; i < SUN_VOLNAME_LEN; i++)
			le32enc(&sl->sl_vtoc.slvc_volname[i], t->sl_vtoc.slvc_volname[i]);
		le16enc(&sl->sl_vtoc.slvc_secsize, t->sl_vtoc.slvc_secsize);
		le16enc(&sl->sl_vtoc.slvc_npart, t->sl_vtoc.slvc_npart);
		for (i = 0; i < SUN_X86NPART; i++) {
			le32enc(&sl->sl_vtoc.slvc_part[i].svti.svtoc_tag,
			t->sl_vtoc.slvc_part[i].svti.svtoc_tag);
			le32enc(&sl->sl_vtoc.slvc_part[i].svti.svtoc_flag,
			t->sl_vtoc.slvc_part[i].svti.svtoc_flag);
			le32enc(&sl->sl_vtoc.slvc_part[i].sdkp.sdkp_cyloffset,
			t->sl_vtoc.slvc_part[i].sdkp.sdkp_cyloffset);
			le32enc(&sl->sl_vtoc.slvc_part[i].sdkp.sdkp_nsectors,
			t->sl_vtoc.slvc_part[i].sdkp.sdkp_nsectors);
			le32enc(&sl->sl_vtoc.slvc_tstamp[i], t->sl_vtoc.slvc_tstamp[i]);
		}
	}
	p = (uint8_t *) sl;
	for (i = u = 0; i < (SUN_SIZE - sizeof(sl->sl_cksum)); i += 2)
		u ^=  *((uint16_t *)(p + i));
	le16enc(&sl->sl_cksum, u);
}
void
sunx86label_print(struct sun_x86disklabel *sl)
{
	printf("              Sanity: 0x%x\n", sl->sl_vtoc.slvc_sane);
	printf("       Label version: %d\n", sl->sl_vtoc.slvc_version);
	printf("         Volume name: <'%8s'>\n", sl->sl_vtoc.slvc_volname);
	printf("         Sector size: %d\n", sl->sl_vtoc.slvc_secsize);
	printf("Number of pertitions: %d\n", sl->sl_vtoc.slvc_npart);
	printf("          ASCII name: '%s'\n", sl->sl_vtoc.slvc_text);
	printf("  Label magic number: '0x%x'\n", sl->sl_magic);
	printf("      Label checksum: '0x%x'\n", sl->sl_cksum);
	printf("--\n");
	printf("(pcyl = %d, ncyl = %d, acyl = %d, bcyl = %d)\n",
			sl->sl_pcylinders, sl->sl_ncylinders, sl->sl_acylinders,
			sl->sl_bcylinders);	
	printf("(tracks = %d, sectors = %d, interleave = %d, alterc = %d)\n", sl->sl_ntracks,
			sl->sl_nsectors, sl->sl_interleave, sl->sl_alterc);
	printf("(rpm = %d, skipwrites = %d, skipreads = %d)\n", sl->sl_rpm,
			sl->sl_skipwrite, sl->sl_skipread);
}

void
sunx86label_print_part(struct sun_x86disklabel *sl)
{
	int i;
	printf("Partitions:\n");
	printf("Number	Tag	Flag	Cylinder offset		Number of sectors\n");
	for (i = 0; i < 16; i++) {
		printf("%d	%d	%d	%d			%d\n", i,
		sl->sl_vtoc.slvc_part[i].svti.svtoc_tag,
		sl->sl_vtoc.slvc_part[i].svti.svtoc_flag,
		sl->sl_vtoc.slvc_part[i].sdkp.sdkp_cyloffset,
		sl->sl_vtoc.slvc_part[i].sdkp.sdkp_nsectors);
	}
}
