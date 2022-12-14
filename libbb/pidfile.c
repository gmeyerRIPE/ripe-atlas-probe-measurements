/* vi: set sw=4 ts=4: */
/*
 * pid file routines
 *
 * Copyright (C) 2007 by Stephane Billiart <stephane.billiart@gmail.com>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this source tree.
 */

/* Override ENABLE_FEATURE_PIDFILE */
#define WANT_PIDFILE 1
#include "libbb.h"
#include <errno.h>
#include <signal.h>
#include <sys/types.h>

smallint wrote_pidfile;

int FAST_FUNC write_pidfile(const char *path)
{
	int pid_fd;
	char *end;
	char buf[sizeof(int)*3 + 2];
	struct stat sb;
	ssize_t written;
	int errno_save;

	written = 0;

	if (!path) {
		errno = EINVAL;
		return -1;
	}

	/* we will overwrite stale pidfile */
	pid_fd = open(path, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	if (pid_fd < 0) {
		/* errno is updated by open() */
		return -1;
	}

	/* path can be "/dev/null"! Test for such cases */
	wrote_pidfile = (fstat(pid_fd, &sb) == 0) && S_ISREG(sb.st_mode);

	if (wrote_pidfile) {
		/* few bytes larger, but doesn't use stdio */
		end = utoa_to_buf(getpid(), buf, sizeof(buf));
		*end = '\n';
		written = full_write(pid_fd, buf, end - buf + 1);
		if ( !written )
			errno = ENOSPC;
	}
	errno_save = errno;
	close(pid_fd);
	errno = errno_save;

	return written > 0 ? 0 : -1;
}

int FAST_FUNC check_pidfile(const char* path)
{
	int r;
	pid_t pid;
	char buf[sizeof(int)*3];

	r = -1;

	if (path == NULL) {
		errno = ENOENT;
		goto exit;
	}

	if (open_read_close(path, buf, sizeof(buf)) < 0)
		goto exit;
	
	errno = 0;
	pid = strtol(buf, NULL, 10);
	if (errno) 
		goto exit;

	r = kill(pid, 0);
	if ((r < 0) && (errno == EPERM)) {
		errno = 0;
		r = 0;
	}

exit:
	return r;
}
