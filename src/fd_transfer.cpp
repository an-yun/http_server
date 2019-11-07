#include "fd_transfer.h"

struct cmsghdr *cmptr=nullptr;

/*
 * Pass a file descriptor to another process.
 * If fd<0, then -fd is sent back instead as the error status.
 */
int
send_fd(int fd, int fd_to_send)
{
	struct iovec	iov[1];
	struct msghdr	msg;
	char			buf[2];	/* send_fd()/recv_fd() 2-byte protocol */

	iov[0].iov_base = buf;
	iov[0].iov_len  = 2;
	msg.msg_iov     = iov;
	msg.msg_iovlen  = 1;
	msg.msg_name    = nullptr;
	msg.msg_namelen = 0;

	if (fd_to_send < 0) {
		msg.msg_control    = nullptr;
		msg.msg_controllen = 0;
		buf[1] = -fd_to_send;	/* nonzero status means error */
		if (buf[1] == 0)
			buf[1] = 1;	/* -256, etc. would screw up protocol */
	} else {
		if (cmptr == nullptr && (cmptr = (cmsghdr *)malloc(CONTROLLEN)) == nullptr)
			return(-1);
		cmptr->cmsg_level  = SOL_SOCKET;
		cmptr->cmsg_type   = SCM_RIGHTS;
		cmptr->cmsg_len    = CONTROLLEN;
		msg.msg_control    = cmptr;
		msg.msg_controllen = CONTROLLEN;
		*(int *)CMSG_DATA(cmptr) = fd_to_send;		/* the fd to pass */
		buf[1] = 0;		/* zero status means OK */
	}

	buf[0] = 0;			/* null byte flag to recv_fd() */
	if (sendmsg(fd, &msg, 0) != 2)
		return(-1);
	return(0);
}


/*
 * Used when we had planned to send an fd using send_fd(),
 * but encountered an error instead.  We send the error back
 * using the send_fd()/recv_fd() protocol.
 */
int
send_err(int fd, int errcode, const char *msg)
{
	int		n;

	if ((n = strlen(msg)) > 0)
		if (writen(fd, msg, n) != n)	/* send the error message */
			return(-1);

	if (errcode >= 0)
		errcode = -1;	/* must be negative */

	if (send_fd(fd, errcode) < 0)
		return(-1);

	return(0);
}

/*
 * Receive a file descriptor from a server process.  Also, any data
 * received is passed to (*userfunc)(STDERR_FILENO, buf, nbytes).
 * We have a 2-byte protocol for receiving the fd from send_fd().
 */
int
recv_fd(int fd, ssize_t (*userfunc)(int, const void *, size_t))
{
	int				newfd, nr, status;
	char			*ptr;
	char			buf[MAXLINE];
	struct iovec	iov[1];
	struct msghdr	msg;

	status = -1;
	for ( ; ; ) {
		iov[0].iov_base = buf;
		iov[0].iov_len  = sizeof(buf);
		msg.msg_iov     = iov;
		msg.msg_iovlen  = 1;
		msg.msg_name    = NULL;
		msg.msg_namelen = 0;
		if (cmptr == NULL && (cmptr = (cmsghdr	*)malloc(CONTROLLEN)) == NULL)
			return(-1);
		msg.msg_control    = cmptr;
		msg.msg_controllen = CONTROLLEN;
		if ((nr = recvmsg(fd, &msg, 0)) < 0) {
            printf("recvmsg error");
            // err_ret("recvmsg error");
            return(-1);
		} else if (nr == 0) {
            printf("connection closed by server");
            // err_ret("connection closed by server");
            return(-1);
		}

		/*
		 * See if this is the final data with null & status.  Null
		 * is next to last byte of buffer; status byte is last byte.
		 * Zero status means there is a file descriptor to receive.
		 */
		for (ptr = buf; ptr < &buf[nr]; ) {
			if (*ptr++ == 0) {
				if (ptr != &buf[nr-1])
                {
                    printf("message format error");
                    abort();
                    //err_dump("message format error");
                }
                
 				status = *ptr & 0xFF;	/* prevent sign extension */
 				if (status == 0) {
					if (msg.msg_controllen RELOP CONTROLLEN)
                    {
                        printf("status = 0 but no fd");
                        abort();
                        //err_dump("status = 0 but no fd");
                    }
					newfd = *(int *)CMSG_DATA(cmptr);
				} else {
					newfd = -status;
				}
				nr -= 2;
			}
		}
		if (nr > 0 && (*userfunc)(STDERR_FILENO, buf, nr) != nr)
			return(-1);
		if (status >= 0)	/* final data has arrived */
			return(newfd);	/* descriptor, or -status */
	}
}

ssize_t             /* Read "n" bytes from a descriptor  */
readn(int fd, void *ptr, size_t n)
{
	size_t		nleft;
	ssize_t		nread;

	nleft = n;
	while (nleft > 0) {
		if ((nread = read(fd, ptr, nleft)) < 0) {
			if (nleft == n)
				return(-1); /* error, return -1 */
			else
				break;      /* error, return amount read so far */
		} else if (nread == 0) {
			break;          /* EOF */
		}
		nleft -= nread;
        ptr   = (char*)(ptr) + nread;
	}
	return(n - nleft);      /* return >= 0 */
}

ssize_t             /* Write "n" bytes to a descriptor  */
writen(int fd, const void *ptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;

	nleft = n;
	while (nleft > 0) {
		if ((nwritten = write(fd, ptr, nleft)) < 0) {
			if (nleft == n)
				return(-1); /* error, return -1 */
			else
				break;      /* error, return amount written so far */
		} else if (nwritten == 0) {
			break;
		}
		nleft -= nwritten;
		ptr   = (char*)(ptr) + nwritten;
	}
	return(n - nleft);      /* return >= 0 */
}