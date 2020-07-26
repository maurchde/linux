#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/netlink.h>
#include <sys/socket.h>

#define MY_GROUP (8)
#define BUFFER_SIZE (512)
static unsigned char S_buffer[BUFFER_SIZE] = { 0 };

/*
 * Simple Netlink multicast example (Receiver). The code creates a Netlink
 * USERSOCK socket, binds to some pid (auto-assigned by kernel) and a group
 * and waits for messages. It expects a string as message payload and prints
 * it to console. Please note that this is quick&dirty code which shows the
 * basic concepts. There is no check of the payload etc.
 * Please also note that this example uses Netlink addressing but not the
 * Netlink message header (this will be subject of another example).
 */
int main(int argc, char **argv)
{
    struct sockaddr_nl nlAddress;
    socklen_t sockAddrLen = sizeof(nlAddress);
    
    /* Create Netlink socket */
    int socketId = socket(AF_NETLINK, SOCK_RAW, NETLINK_USERSOCK);
    if(socketId < 0)
    {
        printf("Could not create NETLINK_USERSOCK (errno:%d)\n", errno);
        return 0;
    }
    
    /* Bind Netlink socket. pid=0 => kernel will assign it for us */
    nlAddress.nl_family = AF_NETLINK;
    nlAddress.nl_pad = 0;
    nlAddress.nl_pid = 0;
    nlAddress.nl_groups = MY_GROUP;
    if(bind(socketId, (struct sockaddr*)&nlAddress, sizeof(nlAddress)) < 0)
    {
        printf("Could not bind NETLINK_USERSOCK (errno:%d)\n", errno);
        close(socketId);
        return 0;
    }
    
    /* Find out which pid was assigned */
    if(getsockname(socketId, (struct sockaddr*)&nlAddress, &sockAddrLen) < 0)
    {
        printf("Could not get NETLINK_USERSOCK address (errno:%d)\n", errno);
        close(socketId);
        return 0;
    }
    
    /* Check if the provided address data has expected size */
    if(sockAddrLen != sizeof(nlAddress))
    {
        printf("Invalid address len (%d)", sockAddrLen);
        close(socketId);
        return 0;
    }

    /* Print group/pid and wait for messages in endless loop */
    printf("My group: %d, my pid:%d\n", MY_GROUP, nlAddress.nl_pid);
    
    while(1)
    {
        /* Receive message */
        if(recvfrom(socketId, S_buffer, sizeof(S_buffer), 0, (struct sockaddr*)&nlAddress, &sockAddrLen) < 0)
        {
            printf("Could not receive message (errno:%d)\n", errno);
            close(socketId);
            return 0;
        }
        
        /* Check if the provided address data has expected size */
        if(sockAddrLen != sizeof(nlAddress))
        {
            printf("Invalid address len (%d)", sockAddrLen);
            close(socketId);
            return 0;
        }
        
        printf("Message received from pid %d: %s\n", nlAddress.nl_pid, S_buffer);
    }
}