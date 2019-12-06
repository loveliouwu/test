

BOOL Init()
{
	int ret = 0;
	int ConnectTimeout = 2;
	mysql = mysql_init(NULL);//初始化;
	if (!mysql)
	{
		printf("mysql_init failed!\n");
		return FALSE;
	}
	ret = mysql_options(mysql, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&ConnectTimeout);//设置超时选项
	if (ret)
	{
		printf("Options Set ERRO!\n");
		free (mysql);
		return FALSE;
	}
	mysql = mysql_real_connect(mysql, MYSQLADDR, MYSQLUSER, MYSQLPW, DATABASENAME, 0, NULL, 0);//连接MySQL testdb数据库
	if (mysql)
	{
		printf("Connection Succeed!\n");
		return TRUE;
	}
	else    //错误处理
	{
		printf("Connection Failed!\n");
		if (mysql_errno(mysql))
		{
			printf("Connect Erro:%d %s\n", mysql_errno(mysql), mysql_error(mysql));//返回错误代码、错误消息
		}
		return FALSE;
	}
}

void Exit()
{
	if (mysql != NULL)
	{
		mysql_close(mysql);
		//free(mysql);
	}
}