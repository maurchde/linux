#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>

#define BUFFER_SIZE (512)
static char S_buffer[BUFFER_SIZE] = { 0 };

/*
 * Simple Netlink example for sending a message to the kernel and receiving
 * an error reply. The code creates a Netlink ROUTE socket and sends a message
 * containing an invalid message type in the Netlink header. Afterwards, the
 * error reply message is received. The raw data of the reply as well as the
 * received error code are printed to console.
 * Please note that this is quick&dirty code which shows the basic concepts.
 * There is no real check of the reply, we just cast it and hope the best.
 * Please also note that the recommended macros for message handling (to avoid
 * alignment issues) are not used. This is subject to another example.
 */
int main(int argc, char **argv)
{
    ssize_t i;
    ssize_t result;
    struct sockaddr_nl nlAddress;
    struct nlmsghdr nlHdr;    

    /* Create Netlink socket */
    int socketId = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);  
    if(socketId < 0)
    {
        printf("Could not create NETLINK_ROUTE (errno:%d)\n", errno);
        return 0;
    }

    /* Prepare message. */
    nlAddress.nl_family = AF_NETLINK;
    nlAddress.nl_pad = 0;
    nlAddress.nl_pid = 0;
    nlAddress.nl_groups = 0;    
    nlHdr.nlmsg_seq = 1;
    nlHdr.nlmsg_len = sizeof(nlHdr);
    nlHdr.nlmsg_pid = 0;
    nlHdr.nlmsg_type = RTM_MAX + 1;
    nlHdr.nlmsg_flags = NLM_F_REQUEST;
    
    /* Send message. When sending several messages to the same receiver
       it might be easier to use 'connect' and then 'write' or 'send'. */ 
    result = sendto(socketId, &nlHdr, sizeof(nlHdr), 0, (struct sockaddr*)&nlAddress, sizeof(nlAddress));
    if(result < 0)
    {
        printf("Could not send message (errno:%d)\n", errno);
        close(socketId);
        return 0;
    }
    
    printf("Message sent to kernel, waiting for reply...\n");    
    
    /* Receive message. We just use 'read' because we know it comes from kernel. */
    result = read(socketId, S_buffer, sizeof(S_buffer));
    if(result < 0)
    {
        printf("Could not receive message (errno:%d)\n", errno);
        close(socketId);
        return 0;
    }
        
    printf("Message received from kernel (%d byte)\n", (int)result);
    for(i=0; i<result; i++)
    {
        printf("0x%02x ", (unsigned char)S_buffer[i]);
        if(((i+1)%16) == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
        
    struct nlmsghdr* recHdr = (struct nlmsghdr*)S_buffer;
    if(recHdr->nlmsg_type == NLMSG_ERROR)
    {
        struct nlmsgerr* recErr = (struct nlmsgerr*)&S_buffer[sizeof(*recHdr)];
        printf("Error reply received (error code: %d)\n", recErr->error);
    }
    else
    {
        printf("Unexpected message type received (type: %d)\n", recHdr->nlmsg_type);
    }
}
