#ifndef DELTASK_H_INCLUDED
#define DELTASK_H_INCLUDED

#include "task.h"


class DelTask : public Task
{
public:
    explicit DelTask(const std::string p_table,const unsigned long p_id,const unsigned long p_time):
    Task(p_table, p_id, p_time){};

    DelTask& operator=(const DelTask &right)
    {
        this->write_id      (right.get_id());
        this->write_exetime (right.get_exetime());
        this->write_table   (right.get_table());
        return *this;
    }
    virtual std::string execute()  override // create a sql expression
    {
        std::string sql("delete from ");
                    sql += get_table() ;
                    sql += " where id = ";
                    sql += std::to_string(get_id());
        return sql;
    };
};

#endif // DELTASK_H_INCLUDED
