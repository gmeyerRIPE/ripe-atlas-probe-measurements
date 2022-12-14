/*
 * Copyright (c) 2013-2014 RIPE NCC <atlas@ripe.net>
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include "libbb.h"
int get_timesync(void)
{
	char *fn;
	FILE *fh;
	int lastsync;

	if (atlas_tests())
		return 123;

	fn= atlas_path(ATLAS_TIMESYNC_FILE_REL);
	fh= fopen(fn, "r");
	free(fn); fn= NULL;
	if (!fh)
		return -1;
	fscanf(fh, "%d", &lastsync);
	fclose(fh);
	return time(NULL)-lastsync;
}

