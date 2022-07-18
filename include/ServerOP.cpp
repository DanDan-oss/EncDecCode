#include "ServerOP.h"
#include <cstring>
#include<fstream>
#include <memory>
#include <jsoncpp/json/json.h>


// ===================

CDbMysqlOP::CDbMysqlOP()
{
	return;
}

CDbMysqlOP::CDbMysqlOP(const std::string& user, const std::string& pass, const std::string& database)
{
	this->InitDBServer(user, pass, database);
	return;
}

CDbMysqlOP::~CDbMysqlOP()
{
	if (this->m_resmysql)
	{
		mysql_free_result(this->m_resmysql);
		this->m_resmysql = nullptr;
	}
	if (this->m_mysql_db)
	{
		mysql_close(this->m_mysql_db);
		this->m_mysql_db = nullptr;
	}
}

bool CDbMysqlOP::InitDBServer(const std::string& user, const std::string& pass, const std::string& database)
{
	this->m_db_user = user;
	this->m_db_pass = pass;
	this->m_db_database = database;
	this->StartDBServer();
	return true;
}

bool CDbMysqlOP::StartDBServer()
{
	if (this->m_mysql_db)
	{
		mysql_close(this->m_mysql_db);
		this->m_mysql_db = nullptr;
	}

	this->m_mysql_db = mysql_init(NULL);

	if (!mysql_real_connect(this->m_mysql_db, NULL, this->m_db_user.data(), this->m_db_pass.data(), this->m_db_database.data(), 0, NULL, 0))
	{
		mysql_close(this->m_mysql_db);
		this->m_mysql_db = nullptr;
		return false;
	}
	return true;
}

MYSQL_RES* CDbMysqlOP::MysqlSelect(const std::string& query)
{
	int status = 0;

	if (!this->m_mysql_db)
		return (MYSQL_RES*)-1;
	if (this->m_resmysql)
	{
		mysql_free_result(this->m_resmysql);
		this->m_resmysql = nullptr;
	}


	status = mysql_query(this->m_mysql_db, query.data());
	if (status)
		return (MYSQL_RES*)-1;

	this->m_resmysql = mysql_store_result(this->m_mysql_db);
	return this->m_resmysql;
}


void CDbMysqlOP::DBMysqlTest(const std::string& cmd)
{
	MYSQL_RES* mysqlres = this->MysqlSelect(cmd);
	if (mysqlres == (void*)-1)
		return;
	MYSQL_FIELD* filed = mysql_fetch_fields(mysqlres);

	int lines = mysql_num_fields(mysqlres);
	for (int i = 0; i < lines; ++i)
		std::cout << filed[i].name << " ";
	std::cout << std::endl;

	MYSQL_ROW row;
	while (row = mysql_fetch_row(mysqlres))
	{
		for (int i = 0; i < lines; ++i)
			if (row[i])
				std::cout << row[i] << " ";
			else
				std::cout << "NULL" << " ";
		std::cout << std::endl;
	}
}

// =================================
CServerOP::CServerOP(const Server_Config& conf)
	: m_serverid(conf.sc_server_id), m_server_shmkey(conf.sc_server_shmkey), 
	CDbMysqlOP(conf.sc_db_user, conf.sc_db_pass, conf.sc_db_database),
	CServerSocket(AF_INET, conf.sc_server_port, conf.sc_maxnode)
{
	return;
}

CServerOP::CServerOP(const std::string& conf_path)
{
	Json::Value conf;
	Json::Reader reader;
	std::ifstream json_file(conf_path, std::ios::in);

	reader.parse(json_file, conf);

	this->m_serverid = conf["serverID"].asString();
	this->m_server_shmkey = conf["shmkey"].asString();

	this->InitDBServer(conf["dbUse"].asString(), conf["dbPasswd"].asString(), conf["db_database"].asString());
	this->InitServerSocket(AF_INET, conf["port"].asInt(), conf["maxnode"].asInt());

	std::cout << "SERVER ID: " << this->m_serverid << "   port: " << this->m_port <<
		"   maxnode: " << this->m_maxnode << "   server shm: " << this->m_server_shmkey << std::endl;

	std::cout << "dbUser: " << this->m_db_user << "   db_pass: " << this->m_db_pass <<
		"   db_database: " << this->m_db_database << std::endl;

	json_file.close();
	return;
}

CServerOP::~CServerOP()
{


	return;
}

bool CServerOP::initServer(const Server_Config& conf)
{
	this->m_serverid = conf.sc_server_id;
	this->m_server_shmkey = conf.sc_server_shmkey;
	this->InitDBServer(conf.sc_db_user, conf.sc_db_pass, conf.sc_db_database);
	this->InitServerSocket(AF_INET, conf.sc_server_port, conf.sc_maxnode);
	return true;
}
