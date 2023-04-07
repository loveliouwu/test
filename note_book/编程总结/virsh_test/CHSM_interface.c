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
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "CHSM_interface.h"

#define DEBUG_OFF_ON
#ifdef DEBUG_OFF_ON 
#define DEBUG(format,...) printf("FILE: "__FILE__", LINE: %d: "format"", __LINE__, ##__VA_ARGS__)
#else
#define DEBUG(format,...) 
#endif


#define CSHM_SH     "chsm_shell.sh"  //chsm shell 脚本文件名


/**
 * CHSM_GetVersion: 获取CHSM的版本
 * @version: 【output】输出的版本信息
 * @version_len: 【output】输出的版本信息长度
 * @return: return 0 if successful, ErrorNO otherwise
 */
int CHSM_GetVersion(char * version, unsigned int *version_len)
{

}


/**
 * CHSM_GetIP: 获取CHSM的ip
 * @ip: 【output】输出的ip地址信息
 * @ip_len: 【output】输出的ip长度
 * @return: return 0 if successful, ErrorNO otherwise
 */
int CHSM_GetIP(char * ip, unsigned int *ip_len)
{
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[128];
    int num = 0;
    ip[0] = '\0';
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return -1;
    }
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;

        // IPv4
        if (family == AF_INET) {
            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                            host, 128, NULL, 0, 1);
            if (s != 0) {
                return -1;
            }
            // printf("IPv4 address for interface %s: %s\n", ifa->ifa_name, host);
            sprintf(ip, "%s%s:%s\n", ip, ifa->ifa_name, host);
            num++;
        }
    }
    freeifaddrs(ifaddr);
    return num;
}



/**
 * CHSM_GetAllNetwork: 获取CHSM的所有网络信息 name0, ip0, mask0, gateway0, name1, ip1, mask1, gateway1 依次排序，每项以逗号分隔
 * @network_list: 【output】输出的信息列表
 * @list_len: 【output】输出的列表长度
 * @return: return  >0 if successful, <0 otherwise, 输出列表中存放的网口数目，一个网口包含四条信息
 */
int CHSM_GetAllNetwork(char * network_list, unsigned int * list_len)
{
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *addr_st, *mask_st;
    int list_num = 0;
    char ip[64] = {0},name[64] = {0},mask[64] = {0},addr[64] = {0},gateway[64] = {0};
    network_list[0] = '\0';
    if (getifaddrs(&ifap) == -1) {
        perror("getifaddrs");
        DEBUG("get cshm's ip error!\n");
    }
    else{
        for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr->sa_family == AF_INET) {
                addr_st = (struct sockaddr_in *)ifa->ifa_addr;
                mask_st = (struct sockaddr_in *)ifa->ifa_netmask;

                //get ip
                strcpy(name, ifa->ifa_name);
                strcpy(ip, inet_ntoa(addr_st->sin_addr));
                strcpy(mask, inet_ntoa(mask_st->sin_addr));

                //get gateway information
                {
                    // int fd;
                    // struct ifreq ifr;
                    // fd = socket(AF_INET, SOCK_DGRAM, 0);
                    // // 获取接口的网关地址
                    // ifr.ifr_addr.sa_family = AF_INET;
                    // strncpy(ifr.ifr_name, ifa->ifa_name, IFNAMSIZ-1);
                    // ioctl(fd, SIOCGIFDSTADDR, &ifr);
                    // strcpy(gateway, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
                    // close(fd);
                    // printf("Gateway address of %s: %s\n", ifa->ifa_name, gateway);

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
                }
                // DEBUG("name: %s\n", name);
                // DEBUG("ip: %s\n", ip);
                // DEBUG("netmask: %s\n", mask);
                // DEBUG("gateway: %s\n", gateway);

                sprintf(network_list, "%s%s:%s:%s:%s\n", network_list, name, ip, mask, gateway);
                list_num++;
            }
        }
    }
    return list_num;
}


/**
 * CHSM_GetDNSList: 获取CHSM的DNS
 * @dns_list: 【output】输出的dns信息,以逗号分隔
 * @dns_list_len: 【output】输出的dns长度
 * @return: return >0(dns number) if successful, ErrorNO otherwise 
 */
int CHSM_GetDNSList(char * dns_list)
{
    dns_list[0]='\0';
    FILE *fp = fopen("/etc/resolv.conf", "r");
    if (fp == NULL) {
        printf("Failed to open /etc/resolv.conf: %s\n", strerror(errno));
        return -1;
    }

    unsigned int num = 0;
    char line[32];
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (line[0] == '#' || line[0] == '\n') {
            // skip comment and empty lines
            continue;
        }
        if (strncmp(line, "nameserver", 10) == 0) {
            // found a nameserver entry
            char *ip_str = strtok(line + 10, " \t\n");
            if (ip_str != NULL) {
                sprintf(dns_list, "%s%s\n", dns_list, ip_str);
                num++;
            }
        }
    }
    fclose(fp);
    return num;
}


/**
 * CHSM_GetExInfo: 获取CHSM的扩展信息
 * @info: 【output】输出的扩展信息
 * @info_len: 【output】输出的扩展信息长度
 * @return: return >0(ex_info number) if successful, ErrorNO otherwise 
 */
int CHSM_GetExInfo(char * info, unsigned int *info_len)
{

    //TODO
    return 0;
}




int CHSM_SetNetworks(char * name, char *ip, char *mask, char *gateway)
{
    if(0)
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
        struct sockaddr_in *addr = (struct sockaddr_in *) &ifr.ifr_addr;
        inet_pton(AF_INET, ip, &addr->sin_addr); // 替换为您的新IP地址
        inet_pton(AF_INET, mask, &((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr); // 替换为您的新子网掩码

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

        inet_pton(AF_INET, gateway, &((struct sockaddr_in *)&route.rt_gateway)->sin_addr); // 替换为您的网关地址

        if (ioctl(sockfd, SIOCADDRT, &route) < 0) {
            perror("ioctl");
            close(sockfd);
            return -1;
        }
        // 关闭套接字
        close(sockfd);
    }
    if(1)
    {
        FILE *fp;
        char cmd[128] = {0};
        char buffer[512] ={0};
        sprintf(cmd,"./chsm_");

        fp = popen("./chsm_backup.sh  --set", "r");

        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            // DEBUG("%s\n", buffer);
        }
        DEBUG("CHSM_BackUpData get packup file name: %s\n",buffer);

        pclose(fp);
        return 0;
    }
}




int CHSM_SetDNS(char *dns)
{
    FILE *fp = fopen("/etc/resolv.conf", "aw");
    if (fp == NULL) {
        perror("fopen");
        return -1;
    }

    char dns_str[128] = {'\0'};
    // 写入新的DNS服务器列表
    sprintf(dns_str, "nameserver %s\n", dns);
    fprintf(fp, "%s", dns_str); // 替换为您的DNS服务器地址1

    // 关闭文件
    fclose(fp);
    // system("service NetworkManager restart");
    return 0;
}



int CHSM_SetLocalTime(int year, int month, int day, int hour, int min, int second)  //需要关闭本机NTP，时间自动同步服务，否则无法完成设置
{
    struct timeval tv;
    struct timezone tz;

    // 获取当前时间
    time_t now = time(NULL);

    // 设置时间结构体
    struct tm *tm = localtime(&now);
    tm->tm_year = year - 1900; // 设置年份
    tm->tm_mon = month - 1; // 设置月份，注意月份从0开始
    tm->tm_mday = day; // 设置日期
    tm->tm_hour = hour; // 设置小时
    tm->tm_min = min; // 设置分钟
    tm->tm_sec = second; // 设置秒数
    time_t newtime = mktime(tm);

    // 设置时间
    tv.tv_sec = newtime;
    tv.tv_usec = 0;
    tz.tz_minuteswest = 0;
    tz.tz_dsttime = 0;
    if (settimeofday(&tv,NULL) == -1) {
        perror("settimeofday");
        return -1;
    }
    printf("New time set: %s", ctime(&newtime));
    return 0;
}



int CHSM_BackUpData()
{
    FILE *fp;
    char buffer[1024];

    fp = popen("./chsm_backup.sh  --backup", "r");

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // DEBUG("%s\n", buffer);
    }
    DEBUG("CHSM_BackUpData get packup file name: %s\n",buffer);

    pclose(fp);
    return 0;
}

int CHSM_Restart()
{
    //TODO  store file

    //restart 
    system("reboot -h now");
    return 0;
}