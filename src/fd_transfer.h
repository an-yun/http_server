#ifndef FD_TRANSFER_H
#define FD_TRANSFER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#define MAXLINE 1024                    /* max line length */
/* size of control buffer to send/recv one file descriptor */
#define	CONTROLLEN	CMSG_LEN(sizeof(int))

#ifdef LINUX
#define RELOP <
#else
#define RELOP !=
#endif

extern struct cmsghdr	*cmptr;	/* malloc'ed first time */

/*
 * Pass a file descriptor to another process.
 * If fd<0, then -fd is sent back instead as the error status.
 */
int send_fd(int fd, int fd_to_send);

/*
 * Used when we had planned to send an fd using send_fd(),
 * but encountered an error instead.  We send the error back
 * using the send_fd()/recv_fd() protocol.
 */
int send_err(int fd, int errcode, const char *msg);

/*
 * Receive a file descriptor from a server process.  Also, any data
 * received is passed to (*userfunc)(STDERR_FILENO, buf, nbytes).
 * We have a 2-byte protocol for receiving the fd from send_fd().
 */
int recv_fd(int fd, ssize_t (*userfunc)(int, const void *, size_t));


/* Read "n" bytes from a descriptor  */
ssize_t  readn(int fd, void *ptr, size_t n);

/* Write "n" bytes to a descriptor  */
ssize_t writen(int fd, const void *ptr, size_t n);

/*
 * Returns a full-duplex pipe (a UNIX domain socket) with
 * the two file descriptors returned in fd[0] and fd[1].
 */
int fd_pipe(int fd[2]);

#endif