#ifndef TABLEDATA_H_INCLUDED
#define TABLEDATA_H_INCLUDED

#include <string>

struct TableData
{
    std::string     field;  // name of the field that is criteria containing the datefield
    unsigned long   offset, // offset in days => in how many days it has to carry out the task
                    maxid;  // here we remember the id of the last register processed

    TableData():field(""),offset(0),maxid(0){};
    TableData(const std::string p_field,const unsigned long p_offset):field(p_field), offset(p_offset), maxid(0){};
};


#endif // TABLEDATA_H_INCLUDED
