/* micro_httpd - really small HTTP server
**
** Copyright © 1999,2005 by Jef Poskanzer <jef@mail.acme.com>.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
** OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
*/


#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define SERVER_NAME "micro_httpd"
#define SERVER_URL "http://www.acme.com/software/micro_httpd/"
#define PROTOCOL "HTTP/1.0"
#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"


/* Forwards. */
static void file_details( char* dir, char* name );
static void send_error( int status, char* title, char* extra_header, char* text );
static void send_headers( int status, char* title, char* extra_header, char* mime_type, off_t length, time_t mod );
static char* get_mime_type( char* name );
static void strdecode( char* to, char* from );
static int hexit( char c );
static void strencode( char* to, size_t tosize, const char* from );
void error(const char *msg);
void do_func(void* data);
void list_push_back(int fd);
int get_next();

struct queue{
	int fd;
	struct queue* next;
};

struct queue *head=NULL;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;

void list_push_back(int fd){
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	struct queue *p, *temp = (struct queue*)malloc(sizeof(struct queue));

	pthread_mutex_lock(&mutex);
	temp->fd = fd;
	temp->next = NULL;

	if( head == NULL){
		head = temp;
		pthread_cond_signal(&cv);
	}
	else{
		for(p=head; p->next!=NULL; p=p->next);
		p->next = temp;
	}
	pthread_mutex_unlock(&mutex);
}

int get_next(){
	int next;
	struct queue *temp;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

	pthread_mutex_lock(&mutex);

	if(head == NULL)
		pthread_cond_wait(&cv, &mutex);

	temp = head;
	next = head->fd;
	head = head->next;	
	free(temp);
	pthread_mutex_unlock(&mutex);

	return next;
}

void error(const char *msg){
	perror(msg);
	exit(1);
}

int
main( int argc, char** argv )
    {
//    char line[10000], method[10000], path[10000], protocol[10000], idx[20000], location[20000];
//    char* file;
//    size_t len;
//    int ich;
//    struct stat sb;
//    FILE* fp;
//    struct dirent **dl;
//    int i, n, option=1, l;

    int option=1;
    int sockfd, newsockfd, portno, numthreads;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
	pthread_t worker[128];
/*    int n; */
    if (argc < 4) {
        fprintf(stderr,"Usage: ./micro_httpd directory port# threads#\n");
        exit(1);
    }
	numthreads=atoi(argv[3]);
	for(int i=0; i<numthreads && i<128; i++){
		pthread_create(&worker[i], NULL, &do_func, (void*)argv[1]);
		pthread_detach(worker[i]);
	}

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    if (sockfd < 0) 
       error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[2]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0) 
             error("ERROR on binding");
    listen(sockfd,100);
    clilen = sizeof(cli_addr);

	while(1){
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) 
			error("ERROR on accept");
		list_push_back(newsockfd);
	}

   close(newsockfd);
   close(sockfd);
	return 0;
}

void do_func(void *data){
    char line[10000], method[10000], path[10000], protocol[10000], idx[20000], location[20000];
    char* file;
    size_t len;
    int ich;
    struct stat sb;
    FILE* fp;
    struct dirent **dl;
    int i, n, l;

char buffer[256];

	int sock;

//    bzero(buffer,256);
//    n = read(newsockfd,buffer,255);
//    if (n < 0) error("ERROR reading from socket");
//    printf("Here is the message: %s\n",buffer);
//    n = write(newsockfd,"I got your message",18);
//    if (n < 0) error("ERROR writing to socket");
//   close(newsockfd);
//    close(sockfd);

//    if ( argc < 3 )
//	send_error( 500, "Internal Error", (char*) 0, "Config error - no dir specified." );
//    if ( chdir( (char*)data ) < 0 ){
//	send_error( 500, "Internal Error", (char*) 0, "Config error - couldn't chdir()." );
//	close(sock);	
//	return;
//	}
//    if ( fgets( line, sizeof(line), stdin ) == (char*) 0 )
	while(1){
	sock = get_next();
   	n = read(sock,buffer,255);
	if(n == 0){
		send_error( 400, "Bad Request", (char*) 0, "No request found." );
		close(sock);	
		continue;
//	return -1;
	}

	l = strlen(buffer);
//	buffer[l]='\n';
	buffer[l]='\0';
	
//	printf("%s\n", buffer);

    if ( sscanf( line, "%[^ ] %[^ ] %[^ ]", method, path, protocol ) != 3 ){
	send_error( 400, "Bad Request", (char*) 0, "Can't parse request." );
	close(sock);	
	continue;
//	return;
	}
//    while ( fgets( line, sizeof(line), stdin ) != (char*) 0 )
//	{
//	if ( strcmp( line, "\n" ) == 0 || strcmp( line, "\r\n" ) == 0 )
//	    break;
//	}
    if ( strcasecmp( method, "get" ) != 0 ){
	send_error( 501, "Not Implemented", (char*) 0, "That method is not implemented." );
	close(sock);	
	continue;
//	return;
	}
    if ( path[0] != '/' ){
	send_error( 400, "Bad Request", (char*) 0, "Bad filename." );
	close(sock);	
	continue;
//	return;
	}
    file = &(path[1]);
    strdecode( file, file );
    if ( file[0] == '\0' )
	file = "./";
    len = strlen( file );
    if ( file[0] == '/' || strcmp( file, ".." ) == 0 || strncmp( file, "../", 3 ) == 0 || strstr( file, "/../" ) != (char*) 0 || strcmp( &(file[len-3]), "/.." ) == 0 )
	send_error( 400, "Bad Request", (char*) 0, "Illegal filename." );
    if ( stat( file, &sb ) < 0 )
	send_error( 404, "Not Found", (char*) 0, "File not found." );
    if ( S_ISDIR( sb.st_mode ) )
	{
	if ( file[len-1] != '/' )
	    {
	    (void) snprintf(
		location, sizeof(location), "Location: %s/", path );
	    send_error( 302, "Found", location, "Directories must end with a slash." );
	    }
	(void) snprintf( idx, sizeof(idx), "%sindex.html", file );
	if ( stat( idx, &sb ) >= 0 )
	    {
	    file = idx;
	    goto do_file;
	    }
	send_headers( 200, "Ok", (char*) 0, "text/html", -1, sb.st_mtime );
	(void) printf( "\
<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n\
<html>\n\
  <head>\n\
    <meta http-equiv=\"Content-type\" content=\"text/html;charset=UTF-8\">\n\
    <title>Index of %s</title>\n\
  </head>\n\
  <body bgcolor=\"#99cc99\">\n\
    <h4>Index of %s</h4>\n\
    <pre>\n", file, file );
	n = scandir( file, &dl, NULL, alphasort );
	if ( n < 0 )
	    perror( "scandir" );
	else
	    for ( i = 0; i < n; ++i )
		file_details( file, dl[i]->d_name );
	(void) printf( "\
    </pre>\n\
    <hr>\n\
    <address><a href=\"%s\">%s</a></address>\n\
  </body>\n\
</html>\n", SERVER_URL, SERVER_NAME );
	}
    else
	{
	do_file:
	fp = fopen( file, "r" );
	if ( fp == (FILE*) 0 )
	    send_error( 403, "Forbidden", (char*) 0, "File is protected." );
	send_headers( 200, "Ok", (char*) 0, get_mime_type( file ), sb.st_size, sb.st_mtime );
	while ( ( ich = getc( fp ) ) != EOF )
	    putchar( ich );
	}

	write(sock, "ACK", strlen("ACK"));

	close(sock);	
//	write(newsockfd, "ACK", 3);
    (void) fflush( stdout );
//	bzero(buffer, 256);
	}
//   close(newsockfd);
//   close(sockfd);
//    exit( 0 );
    }


static void
file_details( char* dir, char* name )
    {
    static char encoded_name[1000];
    static char path[2000];
    struct stat sb;
    char timestr[16];

    strencode( encoded_name, sizeof(encoded_name), name );
    (void) snprintf( path, sizeof(path), "%s/%s", dir, name );
    if ( lstat( path, &sb ) < 0 )
	(void) printf( "<a href=\"%s\">%-32.32s</a>    ???\n", encoded_name, name );
    else
	{
	(void) strftime( timestr, sizeof(timestr), "%d%b%Y %H:%M", localtime( &sb.st_mtime ) );
	(void) printf( "<a href=\"%s\">%-32.32s</a>    %15s %14lld\n", encoded_name, name, timestr, (long long) sb.st_size );
	}
    }


static void
send_error( int status, char* title, char* extra_header, char* text )
    {
    send_headers( status, title, extra_header, "text/html", -1, -1 );
    (void) printf( "\
<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n\
<html>\n\
  <head>\n\
    <meta http-equiv=\"Content-type\" content=\"text/html;charset=UTF-8\">\n\
    <title>%d %s</title>\n\
  </head>\n\
  <body bgcolor=\"#cc9999\">\n\
    <h4>%d %s</h4>\n", status, title, status, title );
    (void) printf( "%s\n", text );
    (void) printf( "\
    <hr>\n\
    <address><a href=\"%s\">%s</a></address>\n\
  </body>\n\
</html>\n", SERVER_URL, SERVER_NAME );
    (void) fflush( stdout );
//    exit( 1 );
    }


static void
send_headers( int status, char* title, char* extra_header, char* mime_type, off_t length, time_t mod )
    {
    time_t now;
    char timebuf[100];

    (void) printf( "%s %d %s\015\012", PROTOCOL, status, title );
    (void) printf( "Server: %s\015\012", SERVER_NAME );
    now = time( (time_t*) 0 );
    (void) strftime( timebuf, sizeof(timebuf), RFC1123FMT, gmtime( &now ) );
    (void) printf( "Date: %s\015\012", timebuf );
    if ( extra_header != (char*) 0 )
	(void) printf( "%s\015\012", extra_header );
    if ( mime_type != (char*) 0 )
	(void) printf( "Content-Type: %s\015\012", mime_type );
    if ( length >= 0 )
	(void) printf( "Content-Length: %lld\015\012", (long long) length );
    if ( mod != (time_t) -1 )
	{
	(void) strftime( timebuf, sizeof(timebuf), RFC1123FMT, gmtime( &mod ) );
	(void) printf( "Last-Modified: %s\015\012", timebuf );
	}
    (void) printf( "Connection: close\015\012" );
    (void) printf( "\015\012" );
    }


static char*
get_mime_type( char* name )
    {
    char* dot;

    dot = strrchr( name, '.' );
    if ( dot == (char*) 0 )
	return "text/plain; charset=UTF-8";
    if ( strcmp( dot, ".html" ) == 0 || strcmp( dot, ".htm" ) == 0 )
	return "text/html; charset=UTF-8";
    if ( strcmp( dot, ".xhtml" ) == 0 || strcmp( dot, ".xht" ) == 0 )
	return "application/xhtml+xml; charset=UTF-8";
    if ( strcmp( dot, ".jpg" ) == 0 || strcmp( dot, ".jpeg" ) == 0 )
	return "image/jpeg";
    if ( strcmp( dot, ".gif" ) == 0 )
	return "image/gif";
    if ( strcmp( dot, ".png" ) == 0 )
	return "image/png";
    if ( strcmp( dot, ".css" ) == 0 )
	return "text/css";
    if ( strcmp( dot, ".xml" ) == 0 || strcmp( dot, ".xsl" ) == 0 )
	return "text/xml; charset=UTF-8";
    if ( strcmp( dot, ".au" ) == 0 )
	return "audio/basic";
    if ( strcmp( dot, ".wav" ) == 0 )
	return "audio/wav";
    if ( strcmp( dot, ".avi" ) == 0 )
	return "video/x-msvideo";
    if ( strcmp( dot, ".mov" ) == 0 || strcmp( dot, ".qt" ) == 0 )
	return "video/quicktime";
    if ( strcmp( dot, ".mpeg" ) == 0 || strcmp( dot, ".mpe" ) == 0 )
	return "video/mpeg";
    if ( strcmp( dot, ".vrml" ) == 0 || strcmp( dot, ".wrl" ) == 0 )
	return "model/vrml";
    if ( strcmp( dot, ".midi" ) == 0 || strcmp( dot, ".mid" ) == 0 )
	return "audio/midi";
    if ( strcmp( dot, ".mp3" ) == 0 )
	return "audio/mpeg";
    if ( strcmp( dot, ".ogg" ) == 0 )
	return "application/ogg";
    if ( strcmp( dot, ".pac" ) == 0 )
	return "application/x-ns-proxy-autoconfig";
    return "text/plain; charset=UTF-8";
    }


static void
strdecode( char* to, char* from )
    {
    for ( ; *from != '\0'; ++to, ++from )
	{
	if ( from[0] == '%' && isxdigit( from[1] ) && isxdigit( from[2] ) )
	    {
	    *to = hexit( from[1] ) * 16 + hexit( from[2] );
	    from += 2;
	    }
	else
	    *to = *from;
	}
    *to = '\0';
    }


static int
hexit( char c )
    {
    if ( c >= '0' && c <= '9' )
	return c - '0';
    if ( c >= 'a' && c <= 'f' )
	return c - 'a' + 10;
    if ( c >= 'A' && c <= 'F' )
	return c - 'A' + 10;
    return 0;		/* shouldn't happen, we're guarded by isxdigit() */
    }


static void
strencode( char* to, size_t tosize, const char* from )
    {
    int tolen;

    for ( tolen = 0; *from != '\0' && tolen + 4 < tosize; ++from )
	{
	if ( isalnum(*from) || strchr( "/_.-~", *from ) != (char*) 0 )
	    {
	    *to = *from;
	    ++to;
	    ++tolen;
	    }
	else
	    {
	    (void) sprintf( to, "%%%02x", (int) *from & 0xff );
	    to += 3;
	    tolen += 3;
	    }
	}
    *to = '\0';
    }
