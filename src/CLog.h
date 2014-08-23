#ifndef CLOG_H
#define CLOG_H
#include <ctime>
#include <string.h>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <forward_list>
class CLog
{
public:
    CLog();

    void data_add(std::string &, std::string &, std::string &, std::string & ,std::string &,std::string &,std::string &);  //date, time, unit_id,  lat, on , alt, nanoseconds
    void archive_save(std::forward_list <std::string> &  );
    void archive_load(std::forward_list <std::string> &  );

    void add(const char * text , ...);
    std::string get_time();

    bool is_empty(std::ifstream& pFile);

    virtual ~CLog();
protected:
private:
    CLog * Log;




};

#endif // CLOG_H
