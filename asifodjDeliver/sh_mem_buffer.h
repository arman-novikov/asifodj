#ifndef SH_MEM_BUFFER_H_INCLUDED
#define SH_MEM_BUFFER_H_INCLUDED

#include <iostream>
#include <string>

#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>


#define sh_mem_name "qsdckufngisjsspqlc"

#define buffer_array_size 30


struct sh_mem_buffer
{
    char                                            table_name[32], field_name[32];
    boost::interprocess::interprocess_semaphore     canwrite, canread;
    unsigned long                                    offset;
    volatile bool                                   is_vacant, can;

    sh_mem_buffer(): table_name(""), field_name(""), canwrite(1), canread(0), offset(0), is_vacant(true){};

};


//typedef std::array<sh_mem_buffer, buffer_array_size> buffer_array;

#endif // SH_MEM_BUFFER_H_INCLUDED
