#ifndef PUSHER_H_INCLUDED
#define PUSHER_H_INCLUDED

#include <thread>
#include <mutex>
#include <list>
#include <map>
#include <pqxx/pqxx>
#include <unistd.h>
#include <time.h>


#include "DelTask.h"
#include "TableData.h"

void insertionSorter(std::list<DelTask> &ls);


void pusher(
             pqxx::connection *p_D,
             std::list<DelTask> &p_DelTasks,
             std::map<std::string, TableData> &p_Tables,
             std::mutex &TableMutex,
             std::mutex &ListMutex,
             bool *can
           );


#endif // PUSHER_H_INCLUDED
