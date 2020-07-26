#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/netlink.h>
#include <sys/socket.h>

#define BUFFER_SIZE (512)
static char S_buffer[BUFFER_SIZE] = { 0 };

/*
 * Simple Netlink multicast example (Sender). The code creates a Netlink
 * USERSOCK socket and sends a message conatining the string given by
 * argv[3] to the groups given by argv[1] and the pid given by argv[2].
 * Please note that this is quick&dirty code which shows the basic concepts.
 * There is no check of the command line parameters etc.
 * Please also note that this example uses Netlink addressing but not the
 * Netlink message header (this will be subject of another example).
 */
int main(int argc, char **argv)
{
    struct sockaddr_nl nlAddress;

    /* Create Netlink socket */
    int socketId = socket(AF_NETLINK, SOCK_RAW, NETLINK_USERSOCK);  
    if(socketId < 0)
    {
        printf("Could not create NETLINK_USERSOCK (errno:%d)\n", errno);
        return 0;
    }

    /* Prepare message. */
    strncpy(S_buffer, argv[3], BUFFER_SIZE-1);
    nlAddress.nl_family = AF_NETLINK;
    nlAddress.nl_pad = 0;
    nlAddress.nl_pid = atoi(argv[2]);
    nlAddress.nl_groups = atoi(argv[1]);
    
    /* Send message. When sending several messages to the same receiver
       it might be easier to use 'connect' and then 'write' or 'send'. */ 
    if(sendto(socketId, S_buffer, strlen(S_buffer)+1, 0, (struct sockaddr*)&nlAddress, sizeof(nlAddress)) < 0)
    {
        printf("Could not send message (errno:%d)\n", errno);
        close(socketId);
        return 0;
    }
    
    printf("Message sent to: groups %d, pid %d\n", nlAddress.nl_groups, nlAddress.nl_pid);    
}
