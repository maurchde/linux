#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/netlink.h>
#include <sys/socket.h>

int main(int argc, char **argv)
{
    int i;
    int socketId;
    
    for(i=0u; i<MAX_LINKS; i++)
    {
        socketId = socket(AF_NETLINK, SOCK_RAW, i);
        if((socketId < 0) && (errno == EPROTONOSUPPORT))
        {
            printf("Netlink Family %u: Not supported\n", i);            
        }
        else
        {
            printf("Netlink Family %u: OK\n", i);
        }        
        close(socketId);
    }
}