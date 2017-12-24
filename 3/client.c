#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>
#include <sys/time.h>

struct sockaddr_in serv_addr;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void send_msg(void * data){
	int sockfd = *(int*)data;
	int n;
	char buffer[256];
	
//	char *buffer="test";
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

	n = write(sockfd, "test", strlen("test"));
	n = read(sockfd, buffer, 255);
//	printf("%s\n", buffer);	
//	if(n!=0)
//	printf("Write complete\n");
//	n = write(sockfd, buffer, strlen(buffer);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n, stat;
    struct hostent *server;
	struct timeval st, end;

    char buffer[256];
	pthread_t worker[1024];

    if (argc < 4) {
       fprintf(stderr,"usage %s hostname port threads#\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
//    sockfd = socket(AF_INET, SOCK_STREAM, 0);
//    if (sockfd < 0) 
//        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
//    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
//        error("ERROR connecting");
//    printf("Please enter the message: ");
//    bzero(buffer,256);
//    fgets(buffer,255,stdin);
	gettimeofday(&st, NULL);

	for(int i=0; i<atoi(argv[3]) && i<1024; i++)
		pthread_create(&worker[i], NULL, &send_msg, (void*)&sockfd);

	for(int i=0; i<atoi(argv[3]) && i<1024; i++)
		pthread_join(worker[i], (void**)&stat);

	gettimeofday(&end, NULL);

	printf("Elapsed Time : %d.%d\n", end.tv_sec-st.tv_sec, end.tv_usec-st.tv_usec);
//	sprintf(buffer, "get /test udp\n");
//    n = write(sockfd,buffer,strlen(buffer));
//    if (n < 0) 
//         error("ERROR writing to socket");
//    bzero(buffer,256);
//  n = read(sockfd,buffer,255);
//  if (n < 0) 
//       error("ERROR reading from socket");
//  printf("%s\n",buffer);
    close(sockfd);
    return 0;
}
