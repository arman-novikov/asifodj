#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <cstring>

#include "sh_mem_buffer.h"

#define DEBUG true

using namespace std;
using namespace boost::interprocess;


int main(int argc, char *argv[])
{
    try
    {
        shared_memory_object shm(open_only, sh_mem_name, read_write);
        mapped_region region(shm,read_write); //Map the whole shared memory in this process
        void * addr = region.get_address(); //get the region address
        sh_mem_buffer * data = static_cast<sh_mem_buffer*>(addr); //obtain a cell of the shared array
        srand(time(NULL)); //for rand() to make it independent on system time
        unsigned short int n = 16; //n is for linean waiting and eventual exit if n = 0

        if(argc == 1) { //if no additional params finish shared memory interconnection throu the flag
            while(!data->is_vacant) {
                if(!n) {
                    cout<<"\nthe waiting time has expired\n";
                    return -1;
                }
                usleep(rand()%1000);
                n--;
            }
            data->can = false;
            return 0;
        }

        if(argc != 4) {
            cout<<"wrong parameters number\n";
            return -1;
        }

        while (!data->is_vacant) {
            if(!n) {
                cout<<"\nthe waiting time has expired\n";
                return -1;
            }
            usleep(rand()%1000);
            n--;
        }

        // occupy the *data
        data->is_vacant = false;
        data->canwrite.wait();

        // write values into *data
        strcpy(data->table_name, argv[1]);
        strcpy(data->field_name, argv[2]);
        data->offset =      atoi(argv[3]);

        //that's it: manager can read
        data->canread.post();
        return 0;
    }
    catch(...) {
        cout<<"can't open shared memory\n";
        return -1;
    }

   return 0;
}
