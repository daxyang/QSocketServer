#ifndef QSOCKETSERVER_H
#define QSOCKETSERVER_H

#include "qsocketserver_global.h"
#include "sockcmd_treasmit.h"
#include "pthread.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "log_function.h"
#ifdef __cplusplus
}
#endif

#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while (0)
class QSOCKETSERVERSHARED_EXPORT QSocketServer
{

public:
    QSocketServer();
    void sock_init();
    void start();
private:

    static void *server_run(void *ptr);
    pthread_t server_pthread_id;

    struct sockaddr_in server_addr;
    int server_socket ;
    void do_cmd_process(u16 cmd_type,u32 cmd_sub_type);
    int login(int socket);
};

#endif // QSOCKETSERVER_H
