#ifndef __CHSM_INTERFACE_H__
#define __CHSM_INTERFACE_H__




/**
 * CHSM_GetVersion: 获取CHSM的版本
 * @version: 【output】输出的版本信息
 * @version_len: 【output】输出的版本信息长度
 * @return: return 0 if successful, ErrorNO otherwise
 */
int CHSM_GetVersion(char * version, unsigned int *version_len);

/**
 * CHSM_GetIP: 获取CHSM的ip
 * @ip: 【output】输出的ip地址信息
 * @ip_len: 【output】输出的ip长度
 * @return: return 0 if successful, ErrorNO otherwise
 */
int CHSM_GetIP(char * ip, unsigned int *ip_len);

/**
 * CHSM_GetAllNetwork: 获取CHSM的所有网络信息 name0, ip0, mask0, gateway0, name1, ip1, mask1, gateway1 依次排序，每项以逗号分隔
 * @network_list: 【output】输出的信息列表
 * @list_len: 【output】输出的列表长度
 * @return: return  >0 if successful, <0 otherwise, 输出列表中存放的网口数目，一个网口包含四条信息
 */
int CHSM_GetAllNetwork(char * network_list, unsigned int * list_len);

/**
 * CHSM_GetDNSList: 获取CHSM的DNS
 * @dns_list: 【output】输出的dns信息,以逗号分隔
 * @return: return >0(dns number) if successful, ErrorNO otherwise 
 */
int CHSM_GetDNSList(char * dns_list);

/**
 * CHSM_GetExInfo: 获取CHSM的扩展信息
 * @info: 【output】输出的扩展信息
 * @info_len: 【output】输出的扩展信息长度
 * @return: return >0(ex_info number) if successful, ErrorNO otherwise 
 */
int CHSM_GetExInfo(char * info, unsigned int *info_len);




int CHSM_SetNetworks(char * name, char *ip, char *mask, char *gateway);
int CHSM_SetDNS(char *dns);//以逗号分隔
int CHSM_SetLocalTime(int year, int month, int day, int hour, int min, int second);


int CHSM_BackUpData();//TODO
int CHSM_Restart();



#endif