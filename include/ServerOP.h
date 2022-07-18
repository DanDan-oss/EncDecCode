#ifndef SERVEROP_H
#define  SERVEROP_H
#include "netsocket.h"
#include <iostream>
#include <mysql/mysql.h>

// MYSQL ==================================================
typedef struct Server_Config
{
	char sc_server_id[30];
	int sc_server_port;
	int sc_maxnode;
	char sc_server_shmkey[50];

	char sc_db_user[15];
	char sc_db_pass[15];
	int sc_db_port;
	char sc_db_database[30];
};


class CDbMysqlOP
{

public:
	CDbMysqlOP();
	CDbMysqlOP(const std::string& user, const std::string& pass, const std::string& database);
	bool InitDBServer(const std::string& user, const std::string& pass, const std::string& database);
	~CDbMysqlOP();

	bool StartDBServer();
	MYSQL_RES* MysqlSelect(const std::string& query);
	void DBMysqlTest(const std::string& cmd);
private:

protected:
	std::string m_db_user;
	std::string m_db_pass;
	std::string m_db_database;
	MYSQL_RES* m_resmysql = nullptr;
	MYSQL* m_mysql_db = nullptr;
};



class CServerOP : public CDbMysqlOP, public CServerSocket
{
public:
	CServerOP(const Server_Config& conf);
	CServerOP(const std::string& conf_path);
	~CServerOP();

	bool initServer(const Server_Config& conf);

	// 密钥协商
	// 密钥效验
	// 密钥注销
	// 密钥查看

private:
	
	std::string m_serverid;
	std::string m_server_shmkey;
};
#endif // !SERVEROP_H

