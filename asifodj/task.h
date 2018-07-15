#ifndef TASK_H_INCLUDED
#define TASK_H_INCLUDED

#include <string>

class Task
{
private:
    std::string     table;      // what table
    unsigned long   id,         // what id of the register
                    exetime;    // when to carry out the task ( amount of diapason of seconds from year 1970 to the date of execution

public:
    explicit Task(const std::string p_table = "", const unsigned long p_id = 0, const unsigned long p_exetime = 0):
    table(p_table), id(p_id), exetime(p_exetime){};

    virtual ~Task() {};

    virtual std::string execute()  =   0; // supposed to return a string used as a sql expression

    virtual void write_id       (const unsigned long n_id)      final {id       = n_id;}
    virtual void write_exetime  (const unsigned long n_exetime) final {exetime  = n_exetime;}
    virtual void write_table    (const std::string n_table)     final {table    = n_table;}

    const unsigned long get_id     () const    { return id;      }
    const unsigned long get_exetime() const    { return exetime; }
    const std::string   get_table  () const    { return table;   }
    //Task& operator=(const Task& right);
};


/*
Task& Task::operator=(const Task& right)
{
    this->id     = right.id;
    this->offset = right.offset;
    this->table  = right.table;
    return *this;
}
*/
#endif // TASK_H_INCLUDED
