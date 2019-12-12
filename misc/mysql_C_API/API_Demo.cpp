#include<string.h>
#include<cstdio>
#include<time.h>
#include<mysql/mysql.h>

typedef struct key_st{
	int		ID;
	char	Pin[16];
	int 	PIN_LEN;
	char 	Name[16];
	tm		create_time;
} key_st;


Insert_Key(key_st* key, UINT32 Index)
{
	char	Sql[SQL_LEN] = { 0 };
	char	TmpSql[SQL_LEN] = { 0 };


	sprintf(Sql, "INSERT INTO SM2_KEY(device_id,pin_length,create_tim) VALUE(%d,%d,NOW(),\'", 1, 2);
	mysql_real_escape_string(mysql, TmpSql, key->Pin, key->PinLen);
	strcat(Sql, TmpSql);
	sprintf(TmpSql, "\',\'");
	strcat(Sql, TmpSql);


	//name
	mysql_real_escape_string(mysql, TmpSql, key->Name, strlen(key->Name));
	strcat(Sql, TmpSql);
	sprintf(TmpSql, "\'\)\;");
	strcat(Sql, TmpSql);

	printf("SQL_Lang:  %s\n", Sql);

	if (mysql_real_query(mysql, Sql, strlen(Sql)) == 0)
	{
		printf("  success\n");
		return TRUE;
	}
	else
	{
		printf(" fail\n");
		return FALSE;
	}


}



Update_Key(key_st* key)
{
	char	Sql[SQL_LEN] = { 0 };
	char	TmpSql[SQL_LEN] = { 0 };


	sprintf(Sql, "UPDATE SM2_KEY SET device_id=%d, pin_length=%d, create_time=\"", 1, key->PinLen);

	//create_time
	strftime(TmpSql, sizeof(TmpSql), "%Y-%m-%d %T", &(key->CreatTime));
	strcat(Sql, TmpSql);
	sprintf(TmpSql, "\"\, pin=\"");
	strcat(Sql, TmpSql);

	//pin
	mysql_real_escape_string(mysql, TmpSql, key->Pin, key->PinLen);
	strcat(Sql, TmpSql);
	sprintf(TmpSql, "\" WHERE id = %d", key->ID);
	strcat(Sql, TmpSql);

	printf("SQL_Lang:  %s\n", Sql);

	if (mysql_real_query(mysql, Sql, strlen(Sql)) == 0)
	{
		printf("  success\n");
		return TRUE;
	}
	else
	{
		printf(" fail\n");
		return FALSE;
	}

}

//根据id销毁
Destroy_Key(key_st* key)
{
	char Sql[1024];
	sprintf(Sql, "DELETE FROM SM2_KEY WHERE  device_id = %d AND id = %d", 1, key->ID);

	if (mysql_real_query(mysql, Sql, strlen(Sql)) == 0)
	{
		printf("  success\n");
		return TRUE;
	}
	else
	{
		printf(" fail\n");
		return FALSE;
	}
}

//key-create_time 已知
//返回填充id
Insert_Key(key_st* key)
{
	char	Sql[SQL_LEN] = { 0 };
	char	TmpSql[SQL_LEN] = { 0 };

	sprintf(Sql, "INSERT INTO SM2_KEY(device_id,pin_length,pin,name, create_time) VALUE(%d,%d,\"", 1, key->PinLen);
	//pin
	mysql_real_escape_string(mysql, TmpSql, key->Pin, key->PinLen);
	strcat(Sql, TmpSql);
	sprintf(TmpSql, "\"\,\"");
	strcat(Sql, TmpSql);

	//name
	mysql_real_escape_string(mysql, TmpSql, key->Name, strlen(key->Name));
	strcat(Sql, TmpSql);
	sprintf(TmpSql, "\"\,\"");
	strcat(Sql, TmpSql);

	//create_time
	strftime(TmpSql, sizeof(TmpSql), "%Y-%m-%d %T", &(key->CreatTime));
	strcat(Sql, TmpSql);
	sprintf(TmpSql, "\"\)");
	strcat(Sql, TmpSql);

	printf("SQL_Lang:  %s\n", Sql);
	if (mysql_real_query(mysql, Sql, strlen(Sql)) == 0)
	{
		int id = mysql_insert_id(mysql);
		if (id != 0)
		{
			key->ID = id;
		}
		printf("  success\n");
		return TRUE;
	}
	else
	{
		printf(" fail\n");
		return FALSE;
	}
}


Select_Key(key_st* key)
{
	MYSQL_ROW row;
	MYSQL_RES* res;
	char	Sql[SQL_LEN] = { 0 };
	char	TmpSql[SQL_LEN] = { 0 };
	char	HashBin[SQL_LEN] = { 0 };
	char	Hash[HASH_LENGTH] = { 0 };
	char	Hash_Tmp[HASH_LENGTH] = { 0 };
	unsigned int OffSet = 0;

	sprintf(Sql, "SELECT pin_length,pin,name,create_time FROM SM2_KEY WHERE id = %d AND device_id = %d", key->ID, 1);

	if (mysql_real_query(mysql, Sql, strlen(Sql)))
	{
		printf(" fail\n");
		return FALSE;
	}
	res = mysql_store_result(mysql);
	if (!res)
	{
		printf(" fail\n");
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
			//printf("%s ", row[t]);//显示每列的值
			switch (t)
			{
			case 0://PIN_LEN
				key->PinLen = atoi(row[t]);
				break;
			case 1://Pin
				strcpy(key->Pin, row[t]);
				break;
			case 2://name
				strcpy(key->Name, row[t]);
				break;
			case 3://create_time
				memcpy(TmpSql, row[t], strlen(row[t]));
				strptime(TmpSql, "%Y-%m-%d %T", &(key->CreatTime));
				break;
			default:
				break;
			}
		}
	}
	else
	{
		printf(" fail\n");
		return FALSE;
	}
	mysql_free_result(res);//释放结果集
	printf(" success\n");
	return TRUE;

}



