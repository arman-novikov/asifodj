#ifndef SH_MEM_BUFFER_H_INCLUDED
#define SH_MEM_BUFFER_H_INCLUDED


#include <map>
#include <thread>

#include <iostream>
#include <string>

#include <unistd.h>
#include <time.h>
#include <mutex>

#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "DelTask.h"
#include "TableData.h"


#define sh_mem_name "qsdckufngisjsspqlc"

#define buffer_array_size 30


struct sh_mem_buffer
{
    char                                            table_name[32], field_name[32];
    boost::interprocess::interprocess_semaphore     canwrite, canread;
    unsigned long                                   offset;
    volatile bool                                   is_vacant, can;

    sh_mem_buffer():
         table_name(""), field_name(""), canwrite(1), canread(0), offset(0), is_vacant(true), can(true) {};

};


//typedef std::array<sh_mem_buffer, buffer_array_size> buffer_array;

int sh_mem_buffer_manager(std::map<std::string, TableData> &mapper, std::mutex &TableMutex, bool *can);

#endif // SH_MEM_BUFFER_H_INCLUDED


//obj/Debug/dbopen.o:/usr/include/x86_64-linux-gnu/c++/5/bits/gthr-default.h|248|first defined here|
