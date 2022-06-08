#include "dataServer.h"

void perror_exit ( string message )
{
    perror ( message.c_str() ) ;
    exit ( EXIT_FAILURE ) ;
}

void* communication_with_client(void* ptr){
    int n, socket = *((int*) ptr);
    delete ((int*)ptr);
    char buff[100];
    n = read(socket, buff, 100);
    cout << "server receved = "<< buff << n << endl;
    strcpy(buff, "text from server");
    write(socket, buff, strlen(buff)+1);
    close(socket);
    return NULL;
}


int main ( int argc , char * argv []) {
    int port, sock, newsock, error;
    int* sock_ptr;
    pthread_t thread_id;
    struct sockaddr_in server, client ;
    socklen_t clientlen;
    struct sockaddr * serverptr =( struct sockaddr *) & server ;

    struct sockaddr * clientptr =( struct sockaddr *) & client ;
    struct hostent * rem ;
    if ( argc != 2) {
        printf ("Please give port number\n") ; 
        exit (1) ;
    }
    port = atoi ( argv [1]) ;
    

    /* Create socket */
    if (( sock = socket ( AF_INET, SOCK_STREAM, 0) ) < 0)
        perror_exit( "socket" );
    server.sin_family = AF_INET ; /* Internet domain */
    server.sin_addr.s_addr = htonl ( INADDR_ANY ) ;
    server.sin_port = htons ( port ) ; /* The given port */
    /* Bind socket to address */
    if ( bind ( sock, serverptr, sizeof(server) ) < 0)
        perror_exit("bind");

        /* Listen for connections */
    if ( listen ( sock, 5) < 0) perror_exit ("listen") ;
    printf ( "Listening for connections to port %d \n ", port) ;
    while (1) {
        /* accept connection */
        if (( newsock = accept ( sock, clientptr, & clientlen ) ) < 0)
            perror_exit ( "accept" ) ;
        
        sock_ptr = new int;
        *sock_ptr = newsock;


        if(error=pthread_create(&thread_id, NULL, communication_with_client, (void*) sock_ptr)){   //xreiazetai pointer
            perror_exit ("create thread");
        }
    }

    pthread_exit(NULL);

}