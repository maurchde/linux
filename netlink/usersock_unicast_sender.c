#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/netlink.h>
#include <sys/socket.h>

/*
 * Simple Netlink unicast example (Sender). The code creates a Netlink
 * USERSOCK socket and sends a message conatining the string given by
 * argv[2] to the pid given by argv[1]. Please note that this is quick&dirty
 * code which shows the basic concepts. There is no check of the command
 * line parameters etc.
 */
int main(int argc, char **argv)
{
    char buffer[1024] = { 0 };
    int targetPid;
    struct iovec iov;
    struct sockaddr_nl nlAddress;
    struct msghdr msgHeader;
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
    nlAddress.nl_groups = 0;
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
    
    /* Send message */
    targetPid = atoi(argv[1]);
    strcpy(buffer, argv[2]);
    nlAddress.nl_pid = targetPid;
    msgHeader.msg_name = &nlAddress;
    msgHeader.msg_namelen = sizeof(nlAddress);
    iov.iov_base = buffer;
    iov.iov_len = sizeof(buffer);
    msgHeader.msg_iov = &iov;
    msgHeader.msg_iovlen = 1;
    msgHeader.msg_control = NULL;
    msgHeader.msg_controllen = 0;
    msgHeader.msg_flags = 0;

    if(sendmsg(socketId, &msgHeader, 0) < 0)
    {
        printf("Could not send message (errno:%d)\n", errno);
        close(socketId);
        return 0;
    }
    
    printf("Message sent to: %d\n", targetPid);    
}
