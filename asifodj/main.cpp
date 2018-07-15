#include <thread>

#include "dbopen.h"
#include "sh_mem_buffer.h"
#include "pusher.h"
#include "executor.h"

#include <tuple>
#include <utility>
#include <memory>

using namespace std;
using namespace pqxx;


std::list<DelTask> DelTasks; // list of the deleting tasks [ critical section ]
std::map<std::string, TableData> Tables; // info about tables that are in work [ critical section ]
mutex ListMutex, TableMutex; // mutexes to organize access to critical sections declared above

int main(const int argc, const char* argv[])
{
    srand(time(NULL)); // to make rand() generate pseudo random numbers dependent on system time

    connection     *C   = nullptr; // pointer to BD connection
    connection     *D   = nullptr; // pointer to BD connection
    bool           *can = new bool(true); // the flag allows executor() and pusher() to work

    // dbopen() returns pointer to a formed connection
    C = dbopen(argc, argv, C); // opening the DB for pusher()
    D = dbopen(argc, argv, D); // opening the DB for executor()

    if (C == nullptr || D == nullptr) { // if dbopen fails to open a new connection it's return nullptr
        cout<<"something got wrong\n";  // the happened is described in pqxx functions
        return -1;
    }

    ListMutex.unlock(); // initially unlock the mutex
    TableMutex.unlock(); // initially unlock the mutex

     // create a thread for getting info about tables to process
    thread manager_thread(sh_mem_buffer_manager, ref(Tables), ref(TableMutex), can);

     // create a thread to form the list of tasks based on info about tables
    thread pusher_thread(pusher, C, ref(DelTasks), ref(Tables), ref(TableMutex), ref(ListMutex), can);

     // create a thread to execute the task formed by pusher_thread
    thread executor_thread(executor, D, ref(DelTasks), ref(ListMutex), can);

    cout<<"input \'Ctrl+C' to exit: ";
    char ch;
    cin>>ch;

    if (ch == 'q') {
        *can = false;

        if (manager_thread.joinable())
            manager_thread.join(); // wait the thread to join

        if (pusher_thread.joinable())
            pusher_thread.join(); // wait the thread to join

        if (executor_thread.joinable())
            executor_thread.join(); // wait the thread to join

        C->disconnect (); // close the connection with DB
        D->disconnect (); // close the connection with DB
        delete D; // return the memory to system
        delete C; // return the memory to system

        return 0;
    } else {
        *can = false;

        if (manager_thread.joinable())
            manager_thread.join(); // wait the thread to join

        if (pusher_thread.joinable())
            pusher_thread.join(); // wait the thread to join

        if (executor_thread.joinable())
            executor_thread.join(); // wait the thread to join

        C->disconnect (); // close the connection with DB
        D->disconnect (); // close the connection with DB
        delete D; // return the memory to system
        delete C; // return the memory to system

        return -1;
    }

    return 0;
}


