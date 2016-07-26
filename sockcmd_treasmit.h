#ifndef SOCKCMD_TREASMIT_H
#define SOCKCMD_TREASMIT_H
#include "qslidingwindow.h"
#include "qslidingwindowconsume.h"
#include "net_protocol.h"
#include "pthread.h"
#include "errno.h"
#include "sys/time.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "log_function.h"
#ifdef __cplusplus
}
#endif

#define SEND_USER  1
#define RECV_USER  2
#define CMD_BUFFER_LEN  (2*1024*1024)
  int READ(int sk, char *buf, int len);
  int WRITE(int sk, char *buf, int len);
class sockcmd_treasmit
{
public:
    sockcmd_treasmit();
    sockcmd_treasmit(int sock);
    void set_socket(int sock);
    ~sockcmd_treasmit();
    void bound(QSlidingWindow *sliding_wnd);
    void start_send();
    void start_recv();
    void start_treasmit();
    void init_cmd();
    void start();
    int quite;
private:

    FILE *file;
    char *filepath;
    char *filename;
    char *fullname;
  //  static sockcmd_treasmit *sock_pthis;

    static void *run_send_cmd(void *ptr);
    static void *run_recv_cmd(void *ptr);

    QSlidingWindow *slidingwnd_recv,*slidingwnd_send;
    QSlidingWindowConsume  *consume_recv,*consume_send;
    pthread_t recv_pthread_id;
    pthread_t send_pthread_id;
    pthread_t treasmit_pthread_id;
    char *send_buffer;
    char *recv_buffer;
    int socket;
    struct _frame_info_t *frame;
    static void *run_cmd_process(void *ptr);

    void do_cmd_process(u16 cmdtype,u32 cmdsubtype,u32 len,char *data);
    void do_ctrl_process(u32 cmdsubtype,u32 len,char *data);
    void do_file_process(u32 cmdsubtype,u32 len,char *data);
    /*
    void do_xxx_process(u32 cmdsubtype,u32 len,char *data);
     */

    struct cmd_link_t *head_cmd_link;


    void append_sub_cmd_link(u16 cmd_type,struct sub_cmd_link_t *sub_cmd_link);
    static void logout_ack(void *ptr);
    static void heart_ack(void *ptr);
    static void filelist_ack(void *ptr);
    static void filesend_ack(void *ptr);
    static void filepath_ack(void *ptr);
    static void filename_ack(void *ptr);
    static void filestart_ack(void *ptr);
    /*
    static void xxxx_ack(void *ptr);
     */


    struct sub_cmd_link_t *search_head_subcmd_link(u16 cmd_type);
    void append_cmd_link(u16 cmdtype);
    struct sub_cmd_link_t *search_subcmd_type(u16 cmd_type,u32 sub_cmd_type);

    struct itimerval itimer,oitimer;


};

#endif // SOCKCMD_TREASMIT_H
