#include "dbopen.h"

using namespace pqxx;
using namespace std;

std::string logdir;

// canlog is used to control access to the logfile
static recursive_mutex *canlog = new recursive_mutex;
//canlog->unlock();

void logger(const std::string what_to_log)
{
    time_t      today; //time_t is a struct to keep amount of seconds passed since 1970year
    tm          *today_struct; //tm is a structure to keep year (after 1900), month, day and etc
    ofstream    out; //out is used for writing logs into a log file
    int         n = 10; // n is used to stop further trials after n trials

    time(&today); //throu the time() there are seconds passed after year 1970 in today
    today_struct = localtime(&today); //localtime insert into a struct of tm type values according the passed time

    //capture the log file
    while( !canlog->try_lock() ){ //returns true if succeeded to capture
        usleep(1000); //wait a random period of time
        n--;

        if(n<=0) //if can't log for a certain period we exit
            return;
    }

    //opening the logfile in the certain directory pointed in ini[logdirectory]
    out.open(logdir +
             '/' + std::to_string(today_struct->tm_year+1900) +
             '_' + std::to_string(today_struct->tm_mon + 1) +
             '_' + std::to_string(today_struct->tm_mday) +
             '_' + "asifodg_logs.txt"
             , ios_base::app);

    //put in the logfile info
    out << setw(10) //stable 10 char sized padding from the left
        << left
        << setfill(' ') // extra space filled with ' '
        <<(
           std::to_string(today_struct->tm_hour) + ':' + //what hour of the record
           std::to_string(today_struct->tm_min)  + ':' + //what minute of the record
           std::to_string(today_struct->tm_sec)  + ' '   //what seconds of the record
           );

    out<<(what_to_log + '\n');  //the message itself

    out.close(); //release the file
    canlog->unlock(); //make the mutex unlock

    return; //exit after successful logging
}

connection* dbopen(const int argc, const char *argv[], connection *C)
{
    string ini[configsnum];

    if (argc == 1 || argc == 2) {
        ifstream in;

        if (argc == 1) in.open("config.ini"); // try  to find configs file in the working directory
        if (argc == 2) in.open(argv[1]);      // try  to find configs file in the directory defined in argv[1]
        if (in.is_open()) {
            for (int i = 0; i<configsnum; i++){ // read the configs file
                getline(in, ini[i]);
            }
            in.close();

            if (!(ini[configsnum-1][0])) { // if the incorrect number of configs
                cout<<ConfigFileOpeningError;
                in.close();
                return nullptr;
            }

        } else {// if no file is opened
            cout<<ConfigFileOpeningError<<endl;
            return nullptr;
        }
    } else if (argc == configsnum + 1) {// try get configs from the command line args
        ini[databasename]   = argv[databasename+1];
        ini[username]       = argv[username+1];
        ini[password]       = argv[password+1];
        ini[logdirectory]   = argv[logdirectory+1];
    } else {// no configs in argv
        cout<<ConfigGettingError;
        return nullptr;
    }

    //writing logdirectory into the global string var named logdir
    logdir = ini[logdirectory]; // where is the directory for the log file

    try {
        C = new connection(connectiontoken); // create a new connection with PostgreSQL
        if (C->is_open()) {
            cout << "Opened database successfully: "
                 << C->dbname()
                 << endl;

            string forlog =     "Opened database successfully: ";
                   forlog +=    C->dbname();

            thread logger_thread(logger, forlog);
            logger_thread.detach();
        } else {
            cout << "Can't open database" << endl;
            return nullptr;
        }
    }
    catch (const std::exception &e) {
       cerr << e.what() << std::endl;
       return nullptr;
    }

    return C;
}
