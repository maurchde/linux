#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

int main(int argc, char **argv)
{
    int i;
    int socketId;
    
    for(i=0u; i<AF_MAX; i++)
    {
        socketId = socket(i, SOCK_STREAM, 0);
        if((socketId < 0) && (errno == EAFNOSUPPORT))
        {
            printf("Address Family %u: Not supported\n", i);            
        }
        else
        {
            printf("Address Family %u: OK\n", i);
        }        
        close(socketId);
    }
}