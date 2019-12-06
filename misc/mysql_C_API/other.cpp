#include<string.h>
#include<cstdio>

using namespace std;

BOOL CreatUser(HSMDeviceUserType type)
{
	int i;
	char sql[512] = { 0 };
	char TmpSql[256] = { 0 };
	char pin_bin[64];
	char hash_bin[32];
	
	for (i = 0; i < 6; i++)
	{
		pin_bin[i] = (char)(('1') + i);
	}
	memset(hash_bin, 0x55, 32);

	sprintf(sql, "INSERT INTO USER(type,pin,pin_length,default_pin,hash_data) VALUE(%d,\'", type);
	mysql_real_escape_string(mysql, TmpSql, pin_bin, 6);
	strcat(sql, TmpSql);
	sprintf(TmpSql,"\',6,%d,\'",DefultSet);
	strcat(sql, TmpSql);
	mysql_real_escape_string(mysql, TmpSql, hash_bin, 32);
	strcat(sql, TmpSql);
	strcat(sql, "\')");

	printf("sql len=%d,%s\n", strlen(sql), sql);
	if (mysql_real_query(mysql, sql, strlen(sql)) == 0)
	{
		printf("CreatDefaultUser  success\n");
		return TRUE;
	}
	else
	{
		printf("CreatDefaultUser fail\n");
		return FALSE;
	}
}

BOOL ChangePin(HSMDeviceUserType type, unsigned char* pin, unsigned int pinLen)
{
	char sql[512] = { 0 };
	char TmpSql[256] = { 0 };
	/*   UPDATE `USER` SET pin='22222222' ,pin_length =8 WHERE type = 1;   */
	mysql_real_escape_string(mysql, TmpSql, (const char*)pin, pinLen);

	sprintf(sql, "UPDATE `USER` SET pin='%s' ,pin_length =%d ,default_pin = %d WHERE type = %d", TmpSql,pinLen, NonDefultSet,type);
	printf("sql:%s\n",sql);
	if (mysql_real_query(mysql, sql, strlen(sql)) == 0)
	{
		printf("HSMChangeUserPin  success\n");
		return TRUE;
	}
	else
	{
		printf("HSMChangeUserPin fail\n");
		return FALSE;
	}
}


BOOL CreatKey(UINT32 VHSMID, pSymKey key, UINT32 KeyLen, UINT32 index)
{
	int i;
	char sql[1024] = { 0 };
	char TmpSql[256] = { 0 };
	char in[32];
	char out[32];
	memset(in, '1', 32);
	sm3(in, 32, out);
	char key_name[10] = "key_name";
	sprintf(sql, "INSERT INTO SYM_KEY(device_id,key_index,key_ciphertext_length,user_key,key_length,creat_time,key_name,hash_data) VALUE(%d,%d,%d,\'", VHSMID,index,32);

	//user_key
	mysql_real_escape_string(mysql, TmpSql, in, 32);
	strcat(sql, TmpSql);

	//key_length create_time 
	sprintf(TmpSql, "\',%d,NOW(),\'",32);
	strcat(sql, TmpSql);
	
	//key_name
	mysql_real_escape_string(mysql, TmpSql, key_name, strlen(key_name));
	strcat(sql, TmpSql);
	sprintf(TmpSql, "\',\'");
	strcat(sql, TmpSql);

	//hash_data
	mysql_real_escape_string(mysql, TmpSql, out, 32);
	strcat(sql, TmpSql);
	sprintf(TmpSql, "\'\)\;");
	strcat(sql, TmpSql);

	printf("SQL_Lang:  ");
	for (i = 0; i < 500; i++)
	{
		printf("%c",sql[i]);
	}
	printf("\n");
	if (mysql_real_query(mysql, sql, strlen(sql)) == 0)
	{
		printf("VHSMCreatUserSymKey  success\n");
		key->ID = index;
		memcpy(key->Key, in, 32);
		key->KeyLen = 32;
		key->Type = SymKeyOK;

		return TRUE;
	}
	else
	{
		printf("VHSMCreatUserSymKey fail\n");
		return FALSE;
	}
}

BOOL GetKey(UINT32 VHSMID, pSymKey key, UINT32 index)
{
	MYSQL_ROW row;
	MYSQL_RES* res;
	MYSQL_FIELD* field;
	unsigned long *len = 0;
	char sql[500];
	sprintf(sql, "SELECT user_key,key_index,key_name,key_state,key_length FROM SYM_KEY WHERE key_index = %d AND device_id = %d", index,VHSMID);


	if (mysql_real_query(mysql, sql, strlen(sql)))
	{
		printf("VHSMGetUserSymKey fail\n");
		return FALSE;
	}
	res = mysql_store_result(mysql);
	if (!res)
	{
		printf("VHSMGetUserSymKey fail\n");
		return FALSE;
	}
	if (res->row_count > 1)
	{
		printf("Error ! There are duplicate keys .\n");//有重复的密钥
		return FALSE;
	}

	row = mysql_fetch_row(res);//获取一行值
	if (row)
	{
		int t = 0;
		for (t = 0; t < mysql_num_fields(res); ++t)//获取列数
		{
			printf("%s ", row[t]);//显示每列的值
			switch (t)
			{
			case 0:
				memcpy(key->Key, row[t], strlen(row[t]));
				break;
			case 1:
				key->ID = atoi(row[t]);
				break;
			case 2:
				memcpy(key->Name, row[t], strlen(row[t]));
				break;
			case 3:
				key->Type = (SymKeyState)atoi(row[t]);
				break;
			case 4:
				key->KeyLen = atoi(row[t]);
				break;
			default:
				break;
			}
		}			
	}
	else
	{
		printf("VHSMGetUserSymKey fail\n");
		return FALSE;
	}
	mysql_free_result(res);//释放结果集
	return TRUE;

}

BOOL UpdateKey(UINT32 KeyID, pSymKey key)
{
	char sql[500];
	char tmp[500];
	sprintf(sql, "UPDATE SYM_KEY SET id=%d,key_state=%d,key_length=%d,user_key=\'", key->ID,key->Type,key->KeyLen);
	mysql_real_escape_string(mysql, tmp, (const char *)(key->Key), key->KeyLen);
	strcat(sql, tmp);
	sprintf(tmp, "\', key_name=\'");
	strcat(sql, tmp);
	mysql_real_escape_string(mysql, tmp, (const char*)(key->Name), key->NameLen);
	strcat(sql, tmp);
	sprintf(tmp, "\' WHERE id=%d",KeyID);
	strcat(sql, tmp);
	if (mysql_real_query(mysql, sql, strlen(sql)))
	{
		printf("VHSMUpdateSymKey fail\n");
		return FALSE;
	}

}

BOOL ImportKey(UINT32 KeyID, pSymKey Newkey,UINT32 index)
{
	char sql[1024] = { 0 };
	char tmp[1024] = { 0 };
	char hash[33];
	sprintf(sql, "INSERT INTO SYM_KEY(device_id,key_ciphertext_length,key_length,key_index,creat_time,user_key,key_name,hash_data) VALUE(%d,%d,%d,%d,NOW(),\'", KeyID, 32,Newkey->KeyLen,index);

	//user_key
	mysql_real_escape_string(mysql, tmp, (const char *)(Newkey->Key), Newkey->KeyLen);
	strcat(sql, tmp);
	sprintf(tmp, "\',\'");
	strcat(sql, tmp);

	//key_name
	mysql_real_escape_string(mysql, tmp, (const char *)(Newkey->Name), Newkey->NameLen);
	strcat(sql, tmp);
	sprintf(tmp, "\',\'");
	strcat(sql, tmp);

	//hash_data
	sm3((char*)(Newkey->Key), Newkey->KeyLen, hash);
	mysql_real_escape_string(mysql, tmp, hash, 32);
	strcat(sql, tmp);
	sprintf(tmp, "\'\)\;");
	strcat(sql, tmp);

	int i = 0;
	printf("SQL_Lang:  ");
	for (i = 0; i < 500; i++)
	{
		printf("%c", sql[i]);
	}
	printf("\n");
	if (mysql_real_query(mysql, sql, strlen(sql)) == 0)
	{
		printf("VHSMImportSymKey  success\n");
		return TRUE;
	}
	else
	{
		printf("VHSMImportSymKey fail\n");
		return FALSE;
	}
}

BOOL ImportKey(UINT32 VHSMID, pSymKey Newkey,UINT32 index)
{
	char sql[1024] = { 0 };
	char tmp[1024] = { 0 };
	char hash[33];

	sprintf(sql, "INSERT INTO SYM_KEY(device_id,key_ciphertext_length,key_length,key_index,creat_time,user_key,key_name,hash_data) VALUE(%d,%d,%d,%d,NOW(),\'", VHSMID, 32, Newkey->KeyLen,index);

	//user_key
	mysql_real_escape_string(mysql, tmp, (const char*)(Newkey->Key), Newkey->KeyLen);
	strcat(sql, tmp);
	sprintf(tmp, "\',\'");
	strcat(sql, tmp);
	//key_name
	mysql_real_escape_string(mysql, tmp, (const char*)(Newkey->Name), Newkey->NameLen);
	strcat(sql, tmp);
	sprintf(tmp, "\',\'");
	strcat(sql, tmp);

	//hash_data
	sm3((char*)(Newkey->Key), Newkey->KeyLen, hash);
	mysql_real_escape_string(mysql, tmp, hash, 32);
	strcat(sql, tmp);
	sprintf(tmp, "\'\)\;");
	strcat(sql, tmp);

	int i = 0;
	printf("SQL_Lang:  ");
	for (i = 0; i < 500; i++)
	{
		printf("%c", sql[i]);
	}
	printf("\n");
	if (mysql_real_query(mysql, sql, strlen(sql)) == 0)
	{
		printf("VHSMImportNewSymKey  success\n");
		return TRUE;
	}
	else
	{
		printf("VHSMImportNewSymKey fail\n");
		return FALSE;
	}
}

BOOL DestoryKey(UINT32 VHSMID,UINT32 index)
{
	char sql[1024];
	sprintf(sql, "DELETE FROM SYM_KEY WHERE  device_id = %d AND key_index = %d",VHSMID,index);

	if (mysql_real_query(mysql, sql, strlen(sql)) == 0)
	{
		printf("VHSMDestorySymKey  success\n");
		return TRUE;
	}
	else
	{
		printf("VHSMDestorySymKey fail\n");
		return FALSE;
	}
	

}





int sm3(char* in, UINT32 in_len, char* out)
{
	memset(out, '2', 32);
	return 0;
}
int sm4_enc(char* key, char* in, UINT32 in_len, char* out)
{
	memset(out, 2, in_len);
	return 0;
}
int sm4_dec(char* key, char* in, UINT32 in_len, char* out)
{
	memset(out, 3, in_len);
	return 0;
}
