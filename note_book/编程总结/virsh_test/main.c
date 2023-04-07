#include"CHSM_interface.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void main()
{
    int ret = 0;
    unsigned char ip[128];
    unsigned int ip_len = 128;
    ret = CHSM_GetIP(ip, &ip_len);
    printf("IP:\n%s\n", ip);


    unsigned char network[256];
    unsigned int network_len = 0;
    CHSM_GetAllNetwork(network, &network_len);
    printf("Network:\n%s\n", network);


    unsigned char dnslist[256];
    ret = CHSM_GetDNSList(dnslist);
    printf("DNS List:\n%s\n",dnslist);


    char set_name[] = "eno1";
    char set_ip[] = "192.168.248.111";
    char set_mask[] = "255.255.255.0";
    char set_gateway[] = "192.168.248.2";
    CHSM_SetNetworks(set_name, set_ip, set_mask, set_gateway);


    char set_dns[] = "8.8.8.8";
    // CHSM_SetDNS(set_dns);


    CHSM_SetLocalTime(2008,8,8,8,8,8);


    //CHSM_BackUpData();
    // CHSM_Restart();
}
