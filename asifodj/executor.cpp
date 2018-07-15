#include "executor.h"

void logger(const std::string what_to_log); // declare the logging function
void executor(pqxx::connection *p_D, std::list<DelTask> &ls, std::mutex &ListMutex, bool * const can)
//can used as flag controlled by main() and allowing to work
{
    std::string     sql; // this is sql expression to carry out
    time_t          now; // the current time
    unsigned long   sleepTime = 100, // the default sleep
                    ulnow; // used to contain time_t value as unsigned long value
                           // necessary for arithmetic ops


    while(*can) {// if no command to end
        while(!ListMutex.try_lock()) // if we can't gain the List
            usleep(rand()%500); // wait random time

        if (!ls.empty()) {
            time(&now);
            // if current time > the time for carrying out the task
            for (auto current = ls.begin(); current != ls.end() && current->get_exetime() < now;) {
                sql = current->execute(); // generate the sql expression

                if (sql != "") {
                    try { // try to carry out the sql expression
                       pqxx::work W(*p_D); // it is gonna be the work transaction
                       W.exec(sql); // forming sql transaction
                       W.commit(); // commit
                       std::thread logger_thread(logger, sql); // generate the independent thread to log
                       logger_thread.detach();
                    }
                    catch(...) {
                        sql += " not committed";
                        std::thread logger_thread(logger, sql); // log the failure
                        logger_thread.detach();
                    }
                    current = ls.erase(current); // pop the executed task
                }
            }

            if(!ls.empty()) {// if empty we put the iterator on the begining and wait
                auto current = ls.begin();
                ulnow        = now;
                sleepTime    = current->get_exetime() - ulnow; // sleep until the next task's exetime
            } else {
                sleepTime = 100; // default sleep
            }
        } else {
            sleepTime = 100;  // default sleep
        }
        ListMutex.unlock(); // release the tasks' mutex
        sleep(sleepTime); //sleeping
    }
    return;
}
