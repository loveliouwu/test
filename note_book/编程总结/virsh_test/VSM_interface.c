#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <net/route.h>
#include <unistd.h>
#include <errno.h>
#include <sys/fcntl.h> 
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "VSM_interface.h"

#define DEBUG_OFF_ON
#ifdef DEBUG_OFF_ON 
#define DEBUG(format,...) printf("FILE: "__FILE__", LINE: %d: "format"", __LINE__, ##__VA_ARGS__)
#else
#define DEBUG(format,...) 
#endif



int VSM_GetNetworks(char *name, char *ip, char *mask, char *gateway)
{
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *addr_st, *mask_st;
    int list_num = 0;
    char addr[64] = {0};
    if (getifaddrs(&ifap) == -1) {
        perror("getifaddrs");
        DEBUG("get cshm's ip error!\n");
    }
    else{
        for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) 
        {
            if ((ifa->ifa_addr->sa_family == AF_INET) && (strcmp(ifa->ifa_name, name) == 0)) 
            {  //获取指定网卡的网络信息
                addr_st = (struct sockaddr_in *)ifa->ifa_addr;
                mask_st = (struct sockaddr_in *)ifa->ifa_netmask;
                //get ip
                strcpy(ip, inet_ntoa(addr_st->sin_addr));
                strcpy(mask, inet_ntoa(mask_st->sin_addr));
                char Cmd[256]={0};
                char readline[100]={0};
                memset( Cmd, 0, sizeof( Cmd ) );
                sprintf( Cmd,"route -n | grep %s | grep G |  awk \'{print $2}\'", name);
                FILE* fp = popen( Cmd, "r" );
                if ( NULL == fp )
                {
                    return -1;
                }
                memset( readline, 0, sizeof( readline ) );
                while ( NULL != fgets( readline,sizeof( readline ),fp ))
                {
                    if(readline[strlen(readline)-1] == '\n')
                    {
                        readline[strlen(readline)-1] = 0;
                    }
                    // printf("gateway=%s\n",readline);
                    strcpy(gateway, readline);
                    break;
                }
                if(gateway[0] == '\0')
                {
                    strcpy(gateway, "NULL");
                }
                pclose(fp);
                return 0;
            }
        }
    }
    return -1;
}

int VSM_SetNetworks(char *name, char *ip, char *mask, char *gateway)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    // 获取当前网卡的配置信息
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, name, IFNAMSIZ-1);

    if (ioctl(sockfd, SIOCGIFADDR, &ifr) < 0) {
        perror("ioctl");
        close(sockfd);
        return -1;
    }

    // 设置新的IP地址和子网掩码
    struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_addr;
    inet_pton(AF_INET, ip, &addr->sin_addr); // 替换为新IP地址
    inet_pton(AF_INET, mask, &((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr); // 替换为新子网掩码

    // 设置网卡的配置信息
    if (ioctl(sockfd, SIOCSIFADDR, &ifr) < 0) {
        perror("ioctl");
        close(sockfd);
        return -1;
    }

    //设置网关
    struct rtentry route;
    memset(&route, 0, sizeof(route));
    route.rt_dst.sa_family = AF_INET;
    route.rt_gateway.sa_family = AF_INET;
    route.rt_flags = RTF_UP | RTF_GATEWAY;

    inet_pton(AF_INET, gateway, &((struct sockaddr_in *)&route.rt_gateway)->sin_addr); // 替换为新的网关地址

    if (ioctl(sockfd, SIOCADDRT, &route) < 0) {
        perror("ioctl");
        close(sockfd);
        return -1;
    }

    // 关闭套接字
    close(sockfd);
}


int VSM_GetExInfo(char *ex_data)
{
    //TODO  
    //step 1:查询扩展信息
    //step 2:打包封装  

}


int VSM_BackUpData(char *backup_data)
{
    //TODO  
    //step 1:导出用户秘钥，使用管理秘钥进行加密
    //step 2:打包用户数据，使用设备秘钥加密
    //step 3:
}