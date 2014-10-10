#ifndef CLOG_H
#define CLOG_H
#include <ctime>
#include <mutex>          // std::mutex
#include <string.h>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <atomic>         // std::atomic, std::atomic_flag, ATOMIC_FLAG_INIT
#include <forward_list>
class CLog
{
public:
    CLog();

    void data_add(std::string &, std::string &, std::string &, std::string & ,std::string &,std::string &,std::string &);  //date, time, unit_id,  lat, on , alt, nanoseconds
    void archive_save(std::forward_list <std::string> &  );
    void archive_load(std::forward_list <std::string> &  );
    std::streamoff last_sent_line_get();
    void last_sent_line_save(std::streamoff ls);

    void add(const char * text , ...);
    std::string get_time();

    bool is_empty(std::ifstream& pFile);

    virtual ~CLog();

protected:
private:
    CLog * Log;
    std::atomic<bool> check_archive;
    std::mutex check_archive_mute;           // mutex for critical section

};

#endif // CLOG_H
