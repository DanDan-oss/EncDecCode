#include <cstdio>
#include "ServerOP.h"

#define  JsonPath "/home/mohui/projects/config.json"

int main()
{
    CServerOP server(JsonPath);
    //printf("%s 向你问好!\n", "LinServer");
    server.DBMysqlTest("select * from emp");

    std::string cc;
    std::cin >> cc;
    return 0;
}