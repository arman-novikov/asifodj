#include "sh_mem_buffer.h"

using namespace std;
using namespace boost::interprocess;

void logger(string what_to_log);


int sh_mem_buffer_manager(std::map<std::string, TableData> &mapper, std::mutex &TableMutex, bool *can)
{
    shared_memory_object shm(open_or_create, sh_mem_name, read_write); //use old shared memory if exists else create a new one
    shm.truncate(sizeof(sh_mem_buffer)*buffer_array_size + 1); //set the size of the memory object
    mapped_region region(shm,read_write); //map the whole shared memory in this process
    void * addr = region.get_address(); //get the region address

    sh_mem_buffer *ipc_data = new (addr) sh_mem_buffer; //create a shared memory buffer in memory

    while(ipc_data->can && *can) {
            if(!ipc_data->is_vacant) {
                usleep(10);
                if(ipc_data->canread.try_wait()) {
                    while(!TableMutex.try_lock()) //insert into mapper new tabledata
                        usleep(rand()%500);

                    mapper[ipc_data->table_name] = TableData(ipc_data->field_name, ipc_data->offset);
                    TableMutex.unlock();

                    //get a string to log into a logfile
                    string forlog   = "table \'";
                    forlog          +=  ipc_data->table_name;
                    forlog          += "\' is in work";

                    thread logger_thread(logger, forlog);
                    logger_thread.detach();

                    //enable using of the ipc_data
                    ipc_data->canwrite.post();
                    ipc_data->is_vacant  = true;
                }
            }
    }
    //log
    string forlog = "tables manager got a stop signal and finished";
    thread logger_thread(logger, forlog);
    logger_thread.detach();

    region.~mapped_region();
    shm.remove(sh_mem_name);
    return 0;
}
