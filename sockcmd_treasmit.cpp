#include "sockcmd_treasmit.h"
int time_cnt;

//sockcmd_treasmit *sockcmd_treasmit::sock_pthis;
sockcmd_treasmit::sockcmd_treasmit()
{
//  sock_pthis = this;
  quite = 0;
}
sockcmd_treasmit::sockcmd_treasmit(int sock)
{
    socket = sock;
    file = fopen("/home/qianzhengyang/nfs/test.mp4","r");
}

sockcmd_treasmit::~sockcmd_treasmit()
{

}
void sockcmd_treasmit::set_socket(int sock)
{
  socket = sock;
}

/*  添加通讯协议（1)
 * 初始化通讯协议
 *    将通讯协议添加到链表中
 */
void sockcmd_treasmit::init_cmd()
{
    //初始化链表
    head_cmd_link = new struct cmd_link_t;
    head_cmd_link->no = 0;
    head_cmd_link->cmd_type = 0;
    head_cmd_link->sub_cmd = NULL;
    head_cmd_link->next = NULL;
    append_cmd_link(NET_TCP_TYPE_CTRL);
    append_cmd_link(NET_TCP_TYPE_FILE);
    append_cmd_link(NET_TCP_TYPE_VID);
    append_cmd_link(NET_TCP_TYPE_AID);
    printf("cmd num:%d\n",head_cmd_link->no);

    // struct sub_cmd_link_t *sub_cmd_login = new struct sub_cmd_link_t;
    // sub_cmd_login->no = NET_CTRL_LOGIN;
    // sub_cmd_login->cmd_sub_type = NET_CTRL_LOGIN;
    // sub_cmd_login->callback = this->login_ack;
    // sub_cmd_login->next = NULL;
    // append_sub_cmd_link(NET_TCP_TYPE_CTRL,sub_cmd_login);

    struct sub_cmd_link_t *sub_cmd_logout = new struct sub_cmd_link_t;
    sub_cmd_logout->no = NET_CTRL_LOGOUT;
    sub_cmd_logout->cmd_sub_type = NET_CTRL_LOGOUT;
    sub_cmd_logout->callback = this->logout_ack;
    sub_cmd_logout->next = NULL;
    append_sub_cmd_link(NET_TCP_TYPE_CTRL,sub_cmd_logout);

    struct sub_cmd_link_t *sub_cmd_heart = new struct sub_cmd_link_t;
    sub_cmd_heart->no = NET_CTRL_HEART;
    sub_cmd_heart->cmd_sub_type = NET_CTRL_HEART;
    sub_cmd_heart->callback = this->heart_ack;
    sub_cmd_heart->next = NULL;
    append_sub_cmd_link(NET_TCP_TYPE_CTRL,sub_cmd_heart);

    struct sub_cmd_link_t *sub_cmd_filelist = new struct sub_cmd_link_t;
    sub_cmd_filelist-> no = NET_FILE_LIST;
    sub_cmd_filelist->cmd_sub_type = NET_FILE_LIST;
    sub_cmd_filelist->callback = this->filelist_ack;
    sub_cmd_filelist->next = NULL;
    append_sub_cmd_link(NET_TCP_TYPE_FILE,sub_cmd_filelist);

    struct sub_cmd_link_t *sub_cmd_filesend = new struct sub_cmd_link_t;
    sub_cmd_filesend-> no = NET_FILE_SEND;
    sub_cmd_filesend->cmd_sub_type = NET_FILE_SEND;
    sub_cmd_filesend->callback = this->filesend_ack;
    sub_cmd_filesend->next = NULL;
    append_sub_cmd_link(NET_TCP_TYPE_FILE,sub_cmd_filesend);

    struct sub_cmd_link_t *sub_cmd_filepath = new struct sub_cmd_link_t;
    sub_cmd_filepath-> no = NET_FILE_PATH;
    sub_cmd_filepath->cmd_sub_type = NET_FILE_PATH;
    sub_cmd_filepath->callback = this->filepath_ack;
    sub_cmd_filepath->next = NULL;
    append_sub_cmd_link(NET_TCP_TYPE_FILE,sub_cmd_filepath);

    /*
     * 添加新的协义(例)
     struct sub_cmd_link_t *sub_cmd_newname = new struct sub_cmd_link_t;
     sub_cmd_newname-> no = NET_xxx_xxx;
     sub_cmd_newname->cmd_sub_type = NET_xxx_xxx;
     sub_cmd_newname->callback = this->Fun;
     sub_cmd_newname->next = NULL;
     append_sub_cmd_link(NET_TCP_TYPE_xxx,sub_cmd_newname);
     */

    struct sub_cmd_link_t *sub_head = search_head_subcmd_link(NET_TCP_TYPE_CTRL);
    printf("cmd sub num:%d \n",sub_head->no);
    struct sub_cmd_link_t *p1;
    p1 = sub_head->next;
    for(int i = 0;i < sub_head->no;i++)
    {
        printf("SUB CMD:%d\n",p1->cmd_sub_type);
        p1 = p1->next;
    }

}
/*
 * 根据cmd_type和sub_cmd_type查找链表对应的节点
 */
struct sub_cmd_link_t *sockcmd_treasmit::search_subcmd_type(u16 cmd_type,u32 sub_cmd_type)
{
    struct sub_cmd_link_t *p1;
    struct sub_cmd_link_t *head_subcmd;
    head_subcmd = search_head_subcmd_link(cmd_type);
    if(head_subcmd == NULL)
        printf("head_subcmd is NULL\n");

    p1 = head_subcmd->next;
    for(int i = 0;i < head_subcmd->no;i++)
    {
        if(p1->cmd_sub_type == sub_cmd_type)
            return p1;
        p1 = p1->next;
    }
    return NULL;
}
/*
 * 添加子命令链表节点
 */
void sockcmd_treasmit::append_sub_cmd_link(u16 cmd_type,struct sub_cmd_link_t *sub_cmd_link)
{
      struct sub_cmd_link_t *head_subcmd_link;
      head_subcmd_link = search_head_subcmd_link(cmd_type);

      struct sub_cmd_link_t *p1;
      p1 = head_subcmd_link;
      while(p1->next != NULL)
      {
          p1 = p1->next;
      }
      if(p1->next == NULL)
          p1->next = sub_cmd_link;
      head_subcmd_link->no++;
}
/*
 * 添加主命令链表节点
 */
void sockcmd_treasmit::append_cmd_link(u16 cmdtype)
{
    struct sub_cmd_link_t *head_sub_cmd_link = new sub_cmd_link_t;
    head_sub_cmd_link->no = 0;
    head_sub_cmd_link->cmd_sub_type = 0;
    head_sub_cmd_link->callback = NULL;
    head_sub_cmd_link->next = NULL;

    struct cmd_link_t *new_cmd_link = new struct cmd_link_t;
    new_cmd_link->no = cmdtype;
    new_cmd_link->cmd_type = cmdtype;
    new_cmd_link->sub_cmd = head_sub_cmd_link;
    new_cmd_link->next = NULL;

    struct cmd_link_t *p1;
    p1 = head_cmd_link;
    while(p1->next != NULL)
    {
        p1 = p1->next;
    }
    if(p1->next == NULL)
    {
        p1->next = new_cmd_link;
    }
    head_cmd_link->no++;
}
/*
 * 查找主命令对应的子命令的链表的头
 */
struct sub_cmd_link_t *sockcmd_treasmit::search_head_subcmd_link(u16 cmd_type)
{
    struct cmd_link_t *p1;
    p1 = head_cmd_link->next;
    for(int i = 0;i < head_cmd_link->no;i++)
    {
        if(p1->cmd_type == cmd_type)
        {
            return p1->sub_cmd;
        }
        p1 = p1->next;
    }
    return NULL;

}
/*
 * 启动命令分析线程
 */
void sockcmd_treasmit::start_treasmit()
{
  pthread_attr_t attr;
  pthread_attr_init (&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_create(&treasmit_pthread_id,&attr,run_cmd_process,this);
  pthread_attr_destroy (&attr);
}
/*
 * 启动接受线程
 */
void sockcmd_treasmit::start_recv()
{
    slidingwnd_recv = new QSlidingWindow();
    recv_buffer = (char *)malloc(sizeof(char) * CMD_BUFFER_LEN);

    slidingwnd_recv->sliding_init(CMD_BUFFER_LEN,recv_buffer);
    slidingwnd_recv->consume_linklist_append(RECV_USER);
    consume_recv = slidingwnd_recv->consume_linklist_getConsume(RECV_USER);

    frame = new _frame_info_t;
    frame->frame_type = 20;

    pthread_attr_t attr;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&recv_pthread_id,&attr,run_recv_cmd,this);
    pthread_attr_destroy (&attr);

}
/*
 * 启动发送线程
 *   （这是发送缓冲区:send_buffer的消费者)
 */
void sockcmd_treasmit::start_send()
{
    slidingwnd_send = new QSlidingWindow();
    send_buffer = (char *)malloc(sizeof(char) * CMD_BUFFER_LEN);

    slidingwnd_send->sliding_init(CMD_BUFFER_LEN,send_buffer);
    slidingwnd_send->consume_linklist_append(SEND_USER);
    consume_send = slidingwnd_send->consume_linklist_getConsume(SEND_USER);

    pthread_attr_t attr;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&send_pthread_id,&attr,run_send_cmd,this);
    pthread_attr_destroy (&attr);

}
void sockcmd_treasmit::bound(QSlidingWindow *sliding_wnd)
{

}
/*
 * 发送线程
 */
void *sockcmd_treasmit::run_send_cmd(void *ptr)
{
    printf("send pthread start!\n");
    sockcmd_treasmit *pthis = (sockcmd_treasmit *)ptr;
    char *buffer = (char *)malloc(sizeof(char) * 1024 * 1024);
    pthis->consume_send->read_init();

    while(pthis->quite == 0)
    {
        //从发送缓冲区中读取命令进行发送
        int len = pthis->consume_send->read_data_to_buffer(buffer);
        if(len > 0)
        {
            WRITE(pthis->socket,buffer,len);
            //printf("send len:%d\n", len);
        }
          #if defined(Q_OS_WIN32)
                    usleep(1000);
          #elif defined(Q_OS_MACX)
                    pthread_yield_np();
          #elif defined(Q_OS_UNIX)
                    usleep(5000);
                    pthread_yield();
          #endif
    }
    free(buffer);
}
/*
 * 接受线程
 *    （这是接受缓冲区:recv_buffer的生产者）
 */
void *sockcmd_treasmit::run_recv_cmd(void *ptr)
{
    printf("receive pthread start!\n");
    sockcmd_treasmit *pthis = (sockcmd_treasmit *)ptr;

    while(pthis->quite == 0)
    {
        struct timeval tv;
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        fd_set set,reset;
        FD_ZERO(&set);
        FD_SET(pthis->socket,&set);

        FD_ZERO(&reset);
        FD_SET(pthis->socket,&reset);
        int ret = select(pthis->socket + 1,&set,NULL,NULL,&tv);

        if(ret <= 0)
        {
            printf("select over timer! %d\n",getpid());
            pthis->quite = 1;
          //  usleep(100000);
            close(pthis->socket);
            exit(0);
            //return NULL;

        }

        //接受到头信息
        char *head_buffer = (char *)malloc(sizeof(char) * NET_HEAD_SIZE);
        app_net_head_pkg_t *head = (app_net_head_pkg_t *)head_buffer;
        int rlen = READ(pthis->socket,head_buffer,NET_HEAD_SIZE);
        if(rlen == -1)
        {
          printf("read data len : 0\n");
          pthis->quite = 1;
          //usleep(100000);
          //close(pthis->socket);
          //exit(0);
          return NULL;
        }
        u32 len = ntohl(head->Length);
        //接受到数据
        char *data_buffer = (char *)malloc(sizeof(char) * len);
        memcpy(data_buffer,head_buffer,NET_HEAD_SIZE);
        rlen = READ(pthis->socket,data_buffer + NET_HEAD_SIZE,len - NET_HEAD_SIZE);
        if(rlen == -1)
        {
          printf("read data len : 0\n");
          pthis->quite = 1;
          //usleep(100000);
          //close(pthis->socket);
          //exit(0);
          return NULL;
        }
        //printf("read:%d,%d\n",ntohs(head->CmdType),ntohl(head->CmdSubType));
        //将数据(含包头)写入接受缓冲区
        pthis->slidingwnd_recv->write_data_to_buffer(len,data_buffer,pthis->frame);
        free(head_buffer);
        free(data_buffer);
    //  }
        #if defined(Q_OS_WIN32)
                  usleep(1000);
        #elif defined(Q_OS_MACX)
                  pthread_yield_np();
        #elif defined(Q_OS_UNIX)
                //  usleep(5000);
                  pthread_yield();
        #endif
    }
}

/*
 * 协议解析线程
 *   （它是接受缓冲区:recv_buffer的消费者和发送缓冲区的生产者)
 */
void *sockcmd_treasmit::run_cmd_process(void *ptr)
{
      printf("cmd_process pthread start!\n");
      sockcmd_treasmit *pthis = (sockcmd_treasmit *)ptr;
      pthis->consume_recv->read_init();
      char *buffer = (char *)malloc(sizeof(char) * 2 * 1024 * 1024);
      while(pthis->quite == 0)
      {
          int len = pthis->consume_recv->read_data_to_buffer(buffer);
          if(len > 0)
          {
              app_net_head_pkg_t *head = (app_net_head_pkg_t *)buffer;
              u16 cmd_type = ntohs(head->CmdType);
              u32 cmd_sub_type = ntohl(head->CmdSubType);
              u32 pkg_len =  ntohl(head->Length) - NET_HEAD_SIZE;
              //printf("cmd:%d sub:%d len:%d\n",cmd_type,cmd_sub_type,pkg_len);
              //将数据进行分析(不含头)
              pthis->do_cmd_process(cmd_type,cmd_sub_type,pkg_len,buffer+NET_HEAD_SIZE);
          }

          #if defined(Q_OS_WIN32)
                    usleep(1000);
          #elif defined(Q_OS_MACX)
                    pthread_yield_np();
          #elif defined(Q_OS_UNIX)
                    usleep(5000);
                    pthread_yield();
          #endif
      }
      free(buffer);
}
/*  添加通讯协议(2)
 * 根据cmdtype进行命令分发
 */
void sockcmd_treasmit::do_cmd_process(u16 cmdtype,u32 cmdsubtype,u32 len,char *data)
{
    switch (cmdtype) {
      case NET_TCP_TYPE_CTRL:
          do_ctrl_process(cmdsubtype,len,data);
      break;
      case NET_TCP_TYPE_FILE:
          do_file_process(cmdsubtype,len,data);
      break;
      case NET_TCP_TYPE_VID:
      break;
      case NET_TCP_TYPE_AID:
      break;
      /*添加主命令处理协议(例)
       * case NET_TCP_TYPE_xxx:
       * 		do_xxx_process();
       * break;
       */
      default:
      break;

    }
}
/*
 * 添加通讯协议 （3)
 * 子命令处理 do_xxx_process(u32 cmdsubtype,u32 len,char *data)
 */
void sockcmd_treasmit::do_ctrl_process(u32 cmdsubtype,u32 len,char *data)
{
    struct cmd_transmit_t cmd;
    cmd.data = data;
    cmd.ptr = this;
    cmd.len = len;
    switch (cmdsubtype) {
      // case NET_CTRL_LOGIN:
      // {
      //       printf("NET_CTRL_LOGIN\n");
      //       struct sub_cmd_link_t *login = search_subcmd_type(NET_TCP_TYPE_CTRL,NET_CTRL_LOGIN);
      //       if(login != NULL)
      //       {
      //           login->callback(&cmd);
      //       }
      // }
      // break;
      case NET_CTRL_LOGOUT:
      {
          struct sub_cmd_link_t *logout = search_subcmd_type(NET_TCP_TYPE_CTRL,NET_CTRL_LOGOUT);
          printf("NET_CTRL_LOGOUT\n");
          logout->callback(&cmd);
      }
      break;
      case NET_CTRL_HEART:
        {
            struct sub_cmd_link_t *heart = search_subcmd_type(NET_TCP_TYPE_CTRL,NET_CTRL_HEART);
            printf("NET_CTRL_HEART:%d\n",getpid());
            heart->callback(&cmd);
        }
      break;
      /*(例)
       * case NET_xxx_xxx:
       * break;
       */
      default:
      break;
    }
}
void sockcmd_treasmit::do_file_process(u32 subcmdtype,u32 len,char *data)
{
  struct cmd_transmit_t cmd;
  cmd.data = data;
  cmd.ptr = this;
  cmd.len = len;
  switch (subcmdtype) {
    case NET_FILE_LIST:
    {
        struct sub_cmd_link_t *filelist = search_subcmd_type(NET_TCP_TYPE_FILE,NET_FILE_LIST);
        printf("NET_FILE_LIST\n");
        filelist->callback(&cmd);
    }
    break;
    case NET_FILE_SEND:
    {
        struct sub_cmd_link_t *filesend = search_subcmd_type(NET_TCP_TYPE_FILE,NET_FILE_SEND);
        printf("NET_FILE_SEND\n");
        filesend->callback(&cmd);
    }
    break;
    case NET_FILE_PATH:
    {
        struct sub_cmd_link_t *filepath = search_subcmd_type(NET_TCP_TYPE_FILE,NET_FILE_PATH);
        printf("NET_FILE_PATH\n");
        filepath->callback(&cmd);
    }
    break;
  }

}
/********************************************************
 * 读取/写入指定长度的数据-网络
 * 输入:
 *    sk:soctk id
 *    buf:缓冲区
 *    len:数据长度
 ********************************************************
*/

int READ(int sk, char *buf, int len)
{
    int ret;
    int left = len;
    int pos = 0;

    while (left > 0)
    {
        if((ret = recv(sk,&buf[pos], left,0))<0)
        {
            printf("read data failed!ret,left: %d,%d,%s\n",ret,left,strerror(errno));
            return -1;
        }
        if(ret == 0)
        {
          printf("READ READ READ\n");
          return -1;
        }
        left -= ret;
        pos += ret;
    }
    //printf("READ len :%d \n",pos);
    return 0;
}
int WRITE(int sk, char *buf, int len)
{
    int ret;
    int left = len;
    int pos = 0;

    while (left > 0)
    {
        if((ret = send(sk,&buf[pos], left,0))<0)
        {
            printf("write data failed!\n");
            return -1;
        }

        left -= ret;
        pos += ret;
    }
    //printf("WRITE len :%d \n",pos);
    return 0;
}

void sockcmd_treasmit::logout_ack(void *ptr)
{
    struct cmd_transmit_t *cmd_ptr = (struct cmd_transmit_t *)ptr;
    sockcmd_treasmit *pthis = (sockcmd_treasmit *)cmd_ptr->ptr;

    u32 pkg_len = NET_HEAD_SIZE + sizeof(app_net_ctrl_ack_logout);
    char *buffer = (char *)malloc(sizeof(char) * pkg_len);
    app_net_head_pkg_t *head = (app_net_head_pkg_t *)buffer;
    app_net_ctrl_ack_logout *ack_logout = (app_net_ctrl_ack_logout *)(buffer + NET_HEAD_SIZE);
    ack_logout->state = htons(1);
    HEAD_PKG(head,NET_TCP_TYPE_CTRL,NET_CTRL_LOGOUT,0,pkg_len);
    pthis->slidingwnd_send->write_data_to_buffer(pkg_len,buffer,pthis->frame);
    printf("logout");
    free(buffer);
    usleep(20000);
    pthis->quite = 1;
    close(pthis->socket);


}
void sockcmd_treasmit::filelist_ack(void *ptr)
{
       struct cmd_transmit_t *cmd_ptr = (struct cmd_transmit_t *)ptr;
       sockcmd_treasmit *pthis = (sockcmd_treasmit *)cmd_ptr->ptr;  //类指针
       char *data = (char *)cmd_ptr->data; //接受到协议内容(不含头)
       u32 len = (u32)cmd_ptr->len; //按受到协议的长度
       //协议分析-->

       //<--
       u32 pkg_len = NET_HEAD_SIZE + sizeof(app_net_file_list);
       char *buffer = (char *)malloc(sizeof(char) * pkg_len);
       app_net_head_pkg_t *head = (app_net_head_pkg_t *)buffer;
       app_net_file_list *list_ack = (app_net_file_list *)(buffer + NET_HEAD_SIZE);
       //返回值设置 -->
       list_ack->file_number = 2;
       strcpy(&list_ack->file[0][0],"dvo_save_001");
       strcpy(&list_ack->file[1][0],"dvo_save_002");
       //<--
       HEAD_PKG(head,NET_TCP_TYPE_FILE,NET_FILE_LIST,0,pkg_len);
       pthis->slidingwnd_send->write_data_to_buffer(pkg_len,buffer,pthis->frame);
       free(buffer);

 }
 void sockcmd_treasmit::filesend_ack(void *ptr)
 {
        struct cmd_transmit_t *cmd_ptr = (struct cmd_transmit_t *)ptr;
        sockcmd_treasmit *pthis = (sockcmd_treasmit *)cmd_ptr->ptr;  //类指针
        char *data = (char *)cmd_ptr->data; //接受到协议内容(不含头)
        u32 len = (u32)cmd_ptr->len; //按受到协议的长度
        //协议分析-->
        app_net_file_send *file_send = (app_net_file_send *)data;
        u16 file_data_len = ntohs(file_send->len);
        u32 serial = ntohl(file_send->serial);
        //<--
        u32 pkg_len = NET_HEAD_SIZE + sizeof(app_net_file_ack_send);
        char *buffer = (char *)malloc(sizeof(char) * (pkg_len + file_data_len));
        app_net_head_pkg_t *head = (app_net_head_pkg_t *)buffer;
        app_net_file_ack_send *send_ack = (app_net_file_ack_send *)(buffer + NET_HEAD_SIZE);
        //返回值设置 -->
        send_ack->serial = htonl(serial);
        u32 file_read_len = fread(buffer+NET_HEAD_SIZE+sizeof(app_net_file_ack_send),sizeof(char),file_data_len,pthis->file);
        pkg_len += file_read_len;
        printf("pkg_len:%d,read_len:%d\n",pkg_len,file_read_len);
        if(file_read_len < file_data_len)
          fclose(pthis->file);
        //<--
  			HEAD_PKG(head,NET_TCP_TYPE_FILE,NET_FILE_SEND,0,pkg_len);
        pthis->slidingwnd_send->write_data_to_buffer(pkg_len,buffer,pthis->frame);
        free(buffer);
}
void sockcmd_treasmit::filepath_ack(void *ptr)
{
       struct cmd_transmit_t *cmd_ptr = (struct cmd_transmit_t *)ptr;
       sockcmd_treasmit *pthis = (sockcmd_treasmit *)cmd_ptr->ptr;  //类指针
       char *data = (char *)cmd_ptr->data; //接受到协议内容(不含头)
       u32 len = (u32)cmd_ptr->len; //按受到协议的长度
       //协议分析-->
       pthis->filepath = (char *)malloc(sizeof(char) * len);
       strcpy(pthis->filepath,data);
       //<--
       u32 pkg_len = NET_HEAD_SIZE + sizeof(app_net_file_ack_path);
       char *buffer = (char *)malloc(sizeof(char) * pkg_len);
       app_net_head_pkg_t *head = (app_net_head_pkg_t *)buffer;
       app_net_file_ack_path *path_ack = (app_net_file_ack_path *)(buffer + NET_HEAD_SIZE);
       //返回值设置 -->
       path_ack->state = htons(1);
       //<--
       HEAD_PKG(head,NET_TCP_TYPE_FILE,NET_FILE_PATH,0,pkg_len);
       pthis->slidingwnd_send->write_data_to_buffer(pkg_len,buffer,pthis->frame);
       free(buffer);

}
/*
 * 添加通讯协议 （4)
 *    添加协议处理函数
 *
 void sockcmd_treasmit::xxxx_ack(void *ptr)
 {
        struct cmd_transmit_t *cmd_ptr = (struct cmd_transmit_t *)ptr;
        sockcmd_treasmit *pthis = (sockcmd_treasmit *)cmd_ptr->ptr;  //类指针
        char *data = (char *)cmd_ptr->data; //接受到协议内容(不含头)
        u32 len = (u32)cmd_ptr->len; //按受到协议的长度
        //协议分析-->

        //<--
        u32 pkg_len = NET_HEAD_SIZE + sizeof(app_net_xxx_xxx_ack_xxx);
        char *buffer = (char *)malloc(sizeof(char) * pkg_len);
        app_net_head_pkg_t *head = (app_net_head_pkg_t *)buffer;
        app_net_xxx_xxx_ack_xxx *xxx_ack = (app_net_xxx_xxx_ack_xxx *)(buffer + NET_HEAD_SIZE);
        //返回值设置 -->

        //<--
  			HEAD_PKG(head,NET_TCP_TYPE_xxx,NET_xxx_xxx,0,pkg_len);
        pthis->slidingwnd_send->write_data_to_buffer(pkg_len,buffer,pthis->frame);
        free(buffer);

}
 */
void sockcmd_treasmit::heart_ack(void *ptr)
{
  struct cmd_transmit_t *cmd_ptr = (struct cmd_transmit_t *)ptr;
  sockcmd_treasmit *pthis = (sockcmd_treasmit *)cmd_ptr->ptr;  //类指针
  char *data = (char *)cmd_ptr->data; //接受到协议内容(不含头)
  u32 len = (u32)cmd_ptr->len; //按受到协议的长度

  app_net_ctrl_heart *heart = (app_net_ctrl_heart *)data;
  //printf("pid:%d heart time:%d-%d-%d %d:%d:%d\n",getpid(),ntohs(heart->yy),heart->MM,heart->dd,heart->hh,heart->mm,heart->ss);
  time_cnt = 0;
  u32 pkg_len = NET_HEAD_SIZE + sizeof(app_net_ctrl_ack_heart);
  char *buffer = (char *)malloc(sizeof(char) * pkg_len);
  app_net_head_pkg_t *head = (app_net_head_pkg_t *)buffer;
  app_net_ctrl_ack_heart *heart_ack = (app_net_ctrl_ack_heart *)(buffer + NET_HEAD_SIZE);

  heart_ack->state = 1;

  HEAD_PKG(head,NET_TCP_TYPE_CTRL,NET_CTRL_HEART,0,pkg_len);
  pthis->slidingwnd_send->write_data_to_buffer(pkg_len,buffer,pthis->frame);
  free(buffer);
}
void sockcmd_treasmit::start()
{
    init_cmd();
    start_recv();
    start_send();
    start_treasmit();
}
