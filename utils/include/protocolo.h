#ifndef UTILS_PROTOCOLO_H_
#define UTILS_PROTOCOLO_H_

#include <stdlib.h>
#include <commons/log.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

typedef enum
{
    KERNEL,
    CPU,
    MEMORIA,
    FILESYSTEM
} op_module;

bool send_handshake(t_log *logger, int fd, const char *connection_name);
bool recv_handshake(t_log *logger, int server_fd);

#endif