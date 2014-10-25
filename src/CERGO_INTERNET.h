#include <forward_list>
#include <unistd.h>
#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include "CLog.h"
#include <mutex>          // std::mutex
class CERGO_INTERNET
{
public:
    CERGO_INTERNET(int);
    CERGO_INTERNET();

    bool send_string(const std::string & data_string);
    void manage_list();
    std::string URLEncode(const char*);
    void reset_internet(clock_t &, int );
    virtual ~CERGO_INTERNET();

    bool get_internet_availiable()
    {
      return internet_connection;
    }
    void set_check_archive(bool CHECK_ARCHIVE_BOOL)
    {
      check_archive = CHECK_ARCHIVE_BOOL;
    }
protected:
private:
    std::mutex mtx;           // mutex for critical section
    bool internet_outage;
    std::atomic<bool> internet_connection;
    std::atomic<bool> check_archive;
    int internet_timeout;
    bool first_pass;
    clock_t internet_timer;
    clock_t log_check; // last time it checked if the log was open.
    CLog * Log;
    int MAX_INTERNET_TIMEOUT;
    int DEBUG_LEVEL;

};

