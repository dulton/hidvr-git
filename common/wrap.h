
#ifndef WRAP_H
#define WRAP_H
#ifdef __cplusplus
extern "C" {
#endif

#include <sys/socket.h>

extern int     Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
extern void    Bind(int fd, const struct sockaddr *sa, socklen_t salen);
extern int     Connect(int fd, const struct sockaddr *sa, socklen_t salen);
extern void    Listen(int fd, int backlog);
extern int     Socket(int family, int type, int protocol);
extern ssize_t Read(int fd, void *ptr, size_t nbytes);
extern ssize_t Write(int fd, const void *ptr, size_t nbytes);
extern void    Close(int fd);
extern ssize_t Readn(int fd, void *vptr, size_t n);
extern ssize_t Writen(int fd, const void *vptr, size_t n);

#ifdef __cplusplus
}
#endif
#endif
