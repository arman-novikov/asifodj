#include "pusher.h"

void insertionSorter(std::list<DelTask> &ls)
{
    // classic realization of insertion sorter algorithm
    // adjusted to the srd::list
    auto begn = ls.begin(); begn++; //now "begn" is the second cell
    auto tempit1      = ls.begin(), tempit2 = ls.begin(); //iterator buffers for swapping
    auto iterSelf     = *begn; // container for a temp value

    for (auto i = begn; i != ls.end();i++) {
        for (auto j = i; j != ls.begin() && (j)->get_exetime() < (--j)->get_exetime();j--) {
            j++;
            tempit1 = j;
            j--;
            tempit2 = j;
            j++;

            iterSelf = *tempit1;
            *tempit1 = *tempit2;
            *tempit2 = iterSelf;
        }
    }
    return;
}

void pusher(
             pqxx::connection *p_D,
             std::list<DelTask> &p_DelTasks,
             std::map<std::string, TableData> &p_Tables,
             std::mutex &TableMutex,
             std::mutex &ListMutex,
             bool *can // used as flag controlled by main() and allowing to work
           )
{
    std::map<std::string, TableData> pusher_Tables; // create a map to copy a critical section map into
                                                    // it's for releasing the TableMutex as soon as possible
    unsigned long sleepTime = 0; // time to sleep
    std::string sql; // contains a sql expression
    tm *tk = new tm; // a structure contains a date in a special format
                     // look throu the time.h library's struct tm

    while(*can) { // if we can work
        sleep(sleepTime*86400); // sleep minimum offset days in secs
        while(!TableMutex.try_lock()) // try to access to the TableMutex
            usleep(rand()%500); // wait if can't

        while(p_Tables.empty()) {// if empty we wait
            TableMutex.unlock();
            usleep(rand()%500);
            while(!TableMutex.try_lock())
                usleep(rand()%500);
        }

        pusher_Tables = p_Tables; // copy the map containing tables info
        TableMutex.unlock(); // release the map
        sleepTime = pusher_Tables.begin()->second.offset; // initial sleeping = the first table's offset

        for (auto i = pusher_Tables.begin(); i != pusher_Tables.end(); ++i) { // begin analyze the tables
            if(sleepTime > i->second.offset)
                sleepTime = i->second.offset; // we can sleep for the minimum offset

            sql = "SELECT id, ";
            sql += i->second.field;
            sql += " FROM ";
            sql += i->first;
            sql += " WHERE id > ";
            sql += std::to_string(i->second.maxid); // i's supposed we have processed previous id
            sql += " ORDER BY id";

            // it is nontransactional order
            // we only read info
            pqxx::nontransaction N(*p_D);
            pqxx::result R(N.exec(sql));

            if (R.begin() != R.end()) {// the query result is not empty
                // we iterate it
                for (pqxx::result::const_iterator constIter = R.begin(); constIter != R.end(); ++constIter ) {
                    // we queried id and time_field
                    // so the result is consisted of
                    // 2d array , smth like this:
                    // [ [id, time], [id, time], ... ]

                    std::string datefield = constIter[1].as<std::string>(); // make each query result's register string
                    // datefield is  like "2017-12-01 21:02:43"
                    datefield.resize(19); // we need the first 19 position in a query's register

                    // look throu time.h struct tm if smth is unclear
                    // get the time of execution according to the datefield and offset
                    tk->tm_sec  = stoi(datefield.substr(17,2));
                    tk->tm_min  = stoi(datefield.substr(14,2));
                    tk->tm_hour = stoi(datefield.substr(11,2));
                    tk->tm_mday = stoi(datefield.substr(8,2)) + (i->second.offset);
                    tk->tm_mon  = stoi(datefield.substr(5,2))-1; // because the enumeration from zero
                    tk->tm_year = stoi(datefield.substr(0,4))-1900; // because the enumeration from year 1900
                    unsigned long theEXEtime = mktime(tk); // mktime counts how many seconds have passed since 1970 to the exetime

                    while(!ListMutex.try_lock()) //  wait while trying to gain access to the tasklists
                        usleep(rand()%500);

                    // forming a DelTask object
                    DelTask x = DelTask(i->first, constIter[0].as<unsigned long>(), theEXEtime);
                    p_DelTasks.push_back(x);

                    ListMutex.unlock(); // release the control over the List of tasks
                }

                auto reend = R.end(); //reend is necessary to write the max_id
                reend--; // because the last iterator is empty in pqxx::result
                while(!TableMutex.try_lock()) // sleep while trying to reaccess the map of tables
                    usleep(rand()%500);

                p_Tables[i->first].maxid = reend[0].as<unsigned long>(); // write the max id as unsigned long into the table info
                TableMutex.unlock(); // release the tables map
            }

            while(!ListMutex.try_lock()) // sorting the list of tasks
                usleep(rand()%500);

            insertionSorter(p_DelTasks); // call the sorter
            ListMutex.unlock(); // release the mutex
        }
    }
    return;
}
