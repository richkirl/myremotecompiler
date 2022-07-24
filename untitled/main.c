#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>
void *get_client_addr(struct sockaddr *sa);
int create_socket(const char* t);
int main()
{

    int sock;
    sock=create_socket("8000");
    if(sock <0)
      {
        fprintf(stderr,"error create socket\n");
        return -1;
      }
    printf("socket created!\n");
    struct sockaddr_storage client_addr;
    int client_d;
    //char client_d;
    while(1){
      socklen_t s_size = sizeof(client_addr);
      client_d=accept(sock,(struct sockaddr*)&client_addr,&s_size);
      if(client_d==-1)
        {
          fprintf(stderr,"error accept\n");
          return -1;
        }
      char ip[INET6_ADDRSTRLEN];
      inet_ntop(client_addr.ss_family,get_client_addr((struct sockaddr*)&client_addr),ip,sizeof(client_addr));
      printf("connection created %s\n",ip);
      }
    return 0;
}
void *get_client_addr(struct sockaddr *sa){
  if(sa->sa_family==AF_INET){
      return &(((struct sockaddr_in*)sa)->sin_addr);
    }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int create_socket(const char *t){

  struct addrinfo hints;
  struct addrinfo *service;
  struct addrinfo *p;


  memset(&hints,0,sizeof(hints));
  hints.ai_family=AF_UNSPEC;
  hints.ai_socktype=SOCK_STREAM;
  hints.ai_flags=AI_PASSIVE;
  int r;
  r=getaddrinfo(NULL,t,&hints,&service);
  if(r!=0){
      fprintf(stderr,"error getaddrinfo!\n");
      return -1;
    }
  int sock;
  int yes;
  for(p=service;p!=NULL;p=p->ai_next){
      sock=socket(p->ai_family,p->ai_socktype,p->ai_protocol);
      if(sock == -1)
        continue;

      if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==1)
        {
          fprintf(stderr,"error setsoket\n");
          close(sock);
          freeaddrinfo(service);
          return -2;
        }
      if(bind(sock,p->ai_addr,p->ai_addrlen)==-1)
        {
          close(sock);
          continue;
        }
        break;
    }
  freeaddrinfo(service);
  if(p==NULL)
    {
     fprintf(stderr,"server fail to find address\n");
     return -3;
    }
  if(listen(sock,1000)==-1)
    {
      fprintf(stderr,"error listen\n");
      return -4;
    }

  return sock;
}
