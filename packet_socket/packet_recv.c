#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1024*16
char PACKET_BUFFER[BUFFER_SIZE];

int main(int argc, char* argv[]) {
  int packet_socket = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_IP)); // ETH_P_IP
  if (packet_socket == -1) {
    printf("open packet socket failed\n");
    return 1;
  }
  
  // bind to specific interface
  struct sockaddr_ll localaddr;
  memset(&localaddr, 0, sizeof(localaddr));
  localaddr.sll_family = AF_PACKET;
  localaddr.sll_ifindex = if_nametoindex("eth0");
  if (localaddr.sll_ifindex == 0) {
    printf("can't found interface eth0");
    return 1;
  }
  
  if (-1 == bind(packet_socket, (const struct sockaddr *)&localaddr, sizeof(localaddr))) {
    printf("bind failed");
    return 1;
  }
  
  // receive packet
  while (1) {
    ssize_t packlen = recv(packet_socket, PACKET_BUFFER, BUFFER_SIZE, 0);
    if (packlen == -1 || packlen == 0) {
      printf("receive packet failed\n");
      break;
    }
    
    if (packlen < sizeof(struct iphdr)) {
      printf("packet is too short");
      continue;
    }
    
    struct iphdr *pheader = (struct iphdr*)PACKET_BUFFER;
    struct in_addr saddr = {pheader->saddr};
    struct in_addr daddr = {pheader->daddr};
    
    char addr_src[16], addr_dst[16];
    strcpy(addr_src, inet_ntoa(saddr));
    strcpy(addr_dst, inet_ntoa(daddr));
    
    printf("an ip packet received. %s->%s len=%d\n",
           addr_src, addr_dst, packlen);
  }
  
  return 0;
}
