#include "remoteClient.h"


void perror_exit ( string message ){
    perror ( message.c_str() ) ;
    exit ( EXIT_FAILURE ) ;
}

int main ( int argc, char * argv[]) {
    int port, sock, i;
    char buf [256] = "this is a test";
    struct sockaddr_in server ;
    struct sockaddr * serverptr = (struct sockaddr *) & server ;
    struct hostent * rem ;
    if ( argc != 3) {
        printf ( " Please give host name and port number \n " ) ;
        exit (1) ;}
    /* Create socket */
    if (( sock = socket ( AF_INET , SOCK_STREAM , 0) ) < 0)
        perror_exit ( " socket " ) ;
    /* Find server address */
    if (( rem = gethostbyname( argv[1]) ) == NULL ) {
        perror ( "gethostbyname" ); 
        exit (1) ;
    }
    port = atoi( argv[2]) ; /* Convert port number to integer */
    server.sin_family = AF_INET ; /* Internet domain */
    memcpy (&server.sin_addr, rem->h_addr, rem->h_length ) ;
    server.sin_port = htons(port); /* Server port */
    /* Initiate connection */
    if ( connect( sock, serverptr, sizeof(server) ) < 0)
        perror_exit ( "connect" );
    printf ( "Connecting to %s port %d \n", argv[1], port ) ;

    write(sock, buf, strlen(buf)+1);
    read(sock, buf, 100);

    cout << "from server =" << buf << endl;
    close(sock);


}