#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <netinet/tcp.h> //Provides declarations for tcp header
#include <arpa/inet.h>   // inet_addr
#include <unistd.h>      // sleep()
#define PCKT_LEN 8192
struct pseudo_header
{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t tcp_length;
};
unsigned short csum(unsigned short *buf, int nwords)
{
    unsigned long sum;
    for (sum = 0; nwords > 0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}
int main(int argc, char const *argv[])
{
    // Checksum
    struct pseudo_header psh;
    u_int16_t src_port;
    u_int32_t src_addr, dst_addr;
    src_addr = inet_addr(argv[1]);
    dst_addr = inet_addr(argv[3]);
    src_port = atoi(argv[2]);
    // dst_port = atoi(argv[4]);

    int sd;
    char buffer[PCKT_LEN], *pseudogram;
    //zero out the packet buffer
    memset(buffer, 0, PCKT_LEN);
    struct iphdr *ip = (struct iphdr *)buffer;
    struct tcphdr *tcph = (struct tcphdr *)(buffer + sizeof(struct iphdr));
    struct sockaddr_in sin;
    int one = 1;
    const int *val = &one;
    memset(buffer, 0, PCKT_LEN);
    // create a raw socket with TCP protocol
    sd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sd < 0)
    {
        perror("socket() error");
        exit(2);
    }
    printf("OK: a TCP raw socket is created.\n");

    // inform the kernel do not fill up the packet structure, we will build our own
    if (setsockopt(sd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
    {
        perror("setsockopt() error");
        exit(2);
    }
    printf("OK: socket option IP_HDRINCL is set.\n");

    int i;
    for (i = 20; i <= 30; i++)
    {

        //Data part
        char *data;
        data = buffer + sizeof(struct iphdr) + sizeof(struct tcphdr);
        strcpy(data, "PONTEP THAWEESUP");

        // the IP header
        ip->ihl = 5;
        ip->version = 4;
        ip->tos = 16; // low delay
        ip->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr) + strlen(data);
        ip->id = htons(54321);
        ip->ttl = 64;               // hops
        ip->protocol = IPPROTO_TCP; // TCP
        // source IP address, can use spoofed address here
        ip->saddr = src_addr;
        ip->daddr = dst_addr;
        ip->check = 0;

        //TCP Header
        tcph->source = htons(src_port);
        tcph->seq = 0;
        tcph->ack_seq = 0;
        tcph->doff = 5; //tcp header size
        tcph->fin = 0;
        tcph->syn = 1;
        tcph->rst = 0;
        tcph->psh = 0;
        tcph->ack = 0;
        tcph->urg = 0;
        tcph->window = htons(5840); /* maximum allowed window sizehdr */
        tcph->check = 0;            //leave checksum 0 now, filled later by pseudo header
        tcph->urg_ptr = 0;
        tcph->dest = htons(i);

        //To calculate the checksum for integrity
        psh.source_address = src_addr;
        psh.dest_address = sin.sin_addr.s_addr;
        psh.placeholder = 0;
        psh.protocol = IPPROTO_TCP;
        psh.tcp_length = htons(sizeof(struct tcphdr) + strlen(buffer));
        int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr) + strlen(buffer);
        pseudogram = malloc(psize);
        memcpy(pseudogram, (char *)&psh, sizeof(struct pseudo_header));
        memcpy(pseudogram + sizeof(struct pseudo_header), tcph, sizeof(struct tcphdr) + strlen(buffer));
        tcph->check = csum((unsigned short *)pseudogram, psize);

        if (sendto(sd, buffer, ip->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0)
        {
            perror("send()");
            exit(3);
        }
        printf("OK: one packet is sent.\n");
    }

    close(sd);
    return 0;
}
