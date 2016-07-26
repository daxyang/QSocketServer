#include "qsocketserver.h"

void reaper(int sig)
{
    int status;
    while(wait3(&status,WNOHANG,(struct rusage*)0)  >=0)
        ;
}

QSocketServer::QSocketServer()
{

}
int QSocketServer::login(int socket)
{
  int loginstate = -1;
  u32 len = NET_HEAD_SIZE + sizeof(app_net_ctrl_login);
  char *buffer = (char *)malloc(sizeof(char) * len);
  app_net_head_pkg_t *head = (app_net_head_pkg_t *)buffer;
  app_net_ctrl_login *login = (app_net_ctrl_login *)(buffer + NET_HEAD_SIZE);
  READ(socket,buffer,len);
  u16 cmdtype = ntohs(head->CmdType);
  u32 cmdsubtype = ntohl(head->CmdSubType);

  u32 ack_len = NET_HEAD_SIZE + sizeof(app_net_ctrl_ack_login);
  char *buffer_ack = (char *)malloc(sizeof(char) * ack_len);
  head = (app_net_head_pkg_t *)buffer_ack;
  app_net_ctrl_ack_login *ack_login = (app_net_ctrl_ack_login *)(buffer_ack + NET_HEAD_SIZE);

  if((cmdtype == NET_TCP_TYPE_CTRL) && (cmdsubtype == NET_CTRL_LOGIN))
  {
      if(strcmp(login->name,"admin") == 0 && strcmp(login->passwd,"123456") == 0)
      {
        ack_login->state = htons(APP_NET_LOGIN_SUCCESS);
        loginstate = 0;
      }
      else
      {
        ack_login->state = htons(APP_NET_LOGIN_PASSWD_FAILE);
        loginstate = -1;
      }
  }
  else
    loginstate = -1;
  HEAD_PKG(head,NET_TCP_TYPE_CTRL,NET_CTRL_LOGIN,0,ack_len);
  WRITE(socket,buffer_ack,ack_len);
  printf("---------------------------------\n");
  printf("login:%d\n",loginstate);
  printf("---------------------------------\n");
  free(buffer);
  free(buffer_ack);
  return loginstate;
}
void QSocketServer::sock_init()
{
    set_log_file("/tmp/log_server.txt");
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);

    server_socket = socket(AF_INET,SOCK_STREAM,0);
    if( server_socket < 0){
        LOG0_ERROR("Create Socket Failed!\n");
        exit(1);
    }

    if( bind(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr))){
        LOG1_ERROR("Server Bind Port : %d Failed!", HELLO_WORLD_SERVER_PORT);
        exit(1);
    }

    fprintf(stderr,"Before Main Process listen.\n");
    if ( listen(server_socket, LENGTH_OF_LISTEN_QUEUE) ){
        LOG0_ERROR("Server Listen Failed!");
        exit(1);
    }
    //LOG0_INFO("After Main Process Listen.\n");
    printf("After Main Process Listen.\n");
}
/*
 * 服务器端处理线程
*/
void *QSocketServer::server_run(void *ptr)
{

    QSocketServer *pthis = (QSocketServer *)ptr;
    (void)signal(SIGCHLD,reaper);
    while(1)
    {
        struct sockaddr_in client_addr;
        socklen_t length = sizeof(client_addr);
        usleep(100000);
        //fprintf(stderr,"Before accept. In %d.\n",getpid());
        printf("7777777777777777777\n");

        int new_client_socket = accept(pthis->server_socket,(struct sockaddr*)&client_addr,&length);
        if ( new_client_socket < 0){
            if(errno == EINTR)
            {
              LOG0_WARING("Server Accept Failed!\n");
              usleep(10000);
              continue;
          }
          else
            break;  //注意，一定用break;
        }

        printf("After accept. In  socket:.\n");
        if(pthis->login(new_client_socket) == 0)
        {
          int pid = fork();
          if(pid == -1)
          {
            LOG0_ERROR("fork error\n");
            ERR_EXIT("fork error\n");

          }
          else if(pid == 0)
          {
            close(pthis->server_socket);
            sockcmd_treasmit *server = new sockcmd_treasmit(new_client_socket);
            server->start();
            while(server->quite == 0)
            {
              //pause();
              sleep(1);

            }
            //
            printf("888888888888888888888888888\n");
            printf("888888888888888888888888888\n");
            printf("888888888888888888888888888\n");
            printf("888888888888888888888888888\n");
            printf("888888888888888888888888888\n");
            close(new_client_socket);
            free(server);
          //  exit(EXIT_SUCCESS);

          }
          else
            close(new_client_socket);
        }
        printf("quit fork\n");
        printf("quit fork\n");
        printf("quit fork\n");
        printf("quit fork\n");
    }

    close(pthis->server_socket);
    return NULL;
}
void QSocketServer::start()
{
    pthread_attr_t attr;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&server_pthread_id,&attr,server_run,this);
    pthread_attr_destroy (&attr);
    //server_run(this);
}
