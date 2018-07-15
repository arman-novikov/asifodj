#ifndef EXECUTOR_H_INCLUDED
#define EXECUTOR_H_INCLUDED


#include <unistd.h>
#include <time.h>
#include <list>
#include <mutex>
#include <pqxx/pqxx>
#include <thread>

#include "DelTask.h"

void executor(pqxx::connection *p_D, std::list<DelTask> &ls, std::mutex &ListMutex, bool *const can);


#endif // EXECUTOR_H_INCLUDED
