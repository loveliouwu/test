#ifndef __VSM_INTERFACE_H__
#define __VSM_INTERFACE_H__


int VSM_GetNetworks(char *name, char *ip, char *mask, char *gateway);

int VSM_SetNetworks(char *name, char *ip, char *mask, char *gateway);

int VSM_GetExInfo(char *ex_data);

int VSM_GetImgHash(char *hash);

int VSM_BackUpData(char *backup_data);

int VSM_UnPackData(char *backup_data, char *unpack_data);

int VSM_LoadKey();

int VSM_SaveKey();

int VSM_CleanUp();

int VSM_Restart();





#endif