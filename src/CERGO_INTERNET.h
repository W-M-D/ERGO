#include <forward_list>
#include <unistd.h>
#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include "CLog.h"

class CERGO_INTERNET
{
public:
    CERGO_INTERNET(int);
    bool send_string(const std::string & data_string);
    bool internet_availiable();
    void manage_list();
    std::string URLEncode(const char*);
    void reset_internet(clock_t &, int );
    virtual ~CERGO_INTERNET();

    int get_internet_availiable()
    {
      return internet_availiable();
    }
protected:
private:
    bool check_archive;
    bool first_pass;
    clock_t internet_timer;
    clock_t log_check; // last time it checked if the log was open.
    CLog * Log;
    int MAX_INTERNET_TIMEOUT;
    int DEBUG_LEVEL;

};

