#ifndef DBOPEN_H_INCLUDED
#define DBOPEN_H_INCLUDED

#include <thread>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <pqxx/pqxx>
#include <unistd.h>
#include <time.h>

#define configsnum      4
#define databasename    0
#define username        1
#define password        2
#define logdirectory    3

#define ConfigFileOpeningError  "the config file is not found:\ncheck the existence of the config.ini in the directory of the asifodj\n"\
                                "the 1st string is for db name\n"\
                                "the 2nd one is for username\n"\
                                "the 3nd one is for password\n"\
                                "the 4th one is for asifodj.log\n"\
                                "make sure there are 4 strings in correct order\n"

#define ConfigGettingError      "no one protocol of config data transmitting used correctly\n"


#define connectiontoken         "dbname = "+ini[databasename]+" user = "+ini[username]+" password = "+ini[password]+" hostaddr = 127.0.0.1 port = 5432"

pqxx::connection* dbopen(const int argc,const char *argv[], pqxx::connection *C);
void logger(const std::string what_to_log);

#endif // DBOPEN_H_INCLUDED
