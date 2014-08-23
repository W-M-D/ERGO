#include "CLog.h"
#include <ctime>
#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
/*
USAGE
create a instance of the class

CLog * Log;


Then to add

use regular expresions to add using format string specifiers
http://www.cplusplus.com/reference/cstdio/printf/
more information on format strings here...

www.cplusplus.com/reference/cstdio/printf/

Log->add(" your string here %d,%f,..."42,42.42 )

*/
CLog::CLog()
{
  system("mkdir -p /etc/ERGO/");
   //ctor
}

void CLog::data_add(std::string & date, std::string & time, std::string & unit_id, std::string & lat,std::string & lon,std::string & alt,std::string & nanoseconds)
{
  std::ofstream data_file;
  data_file.open("/etc/ERGO/ERGO_DATA.csv",std::ios_base::out | std::ios_base::app);
  data_file << date << ',' << time << ',' << unit_id << ',' << lat << ',' << lon << ',' << alt << ',' << nanoseconds << '\n';
  data_file.close();
}

void CLog::archive_save(std::forward_list <std::string> & string_list )
{
  std::ofstream data_file;
  data_file.open("/etc/ERGO/ERGO_ARCHIVE.list",std::ios::out | std::ios::app);
  while(!string_list.empty())
  {
    data_file << string_list.front() << '\n';
    string_list.pop_front();
  }
  data_file.close();
}

void CLog::archive_load(std::forward_list <std::string> &  data_list)
{

      std::ifstream data_in;
      data_in.open("/etc/ERGO/ERGO_ARCHIVE.list");

      if(!is_empty(data_in))
      {
          std::string instring;
          auto string_iterator = data_list.before_begin(); // iterator for string list
          while (!data_in.eof())
          {
          getline(data_in,instring);
          string_iterator = data_list.emplace_after(string_iterator,instring);
          }

          std::ofstream data_clear;
          data_clear.open( "/etc/ERGO/ERGO_ARCHIVE.list", std::ios::out | std::ios::trunc );
          data_clear.close();
      }

    data_in.close();

}

bool CLog::is_empty(std::ifstream& pFile)
{
    return pFile.peek() == std::ifstream::traits_type::eof();
}


void CLog::add(const char * text , ...)
{
    std::ofstream log_file;
    log_file.open ("/etc/ERGO/ERGO_LOG.log", std::ios_base::out | std::ios_base::app); //creates log



    va_list va_alist;  //formats log and places into buffer
    char log_buff[512] ;
    memset(log_buff, 0, sizeof(log_buff));
    va_start ( va_alist, text );
    vsnprintf( log_buff + strlen( log_buff ),511, text, va_alist );
    va_end( va_alist );

    std::string log_text (log_buff);
    log_file << get_time() << " " << log_text << std::endl; // prints log
    log_file.close();
}

std::string CLog::get_time()
{
    std::stringstream time_now;
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    time_now << (now->tm_year + 1900) << '-'
    << (now->tm_mon + 1) << '-'
    <<  now->tm_mday << ' '
    <<  now->tm_hour <<':'
    <<  now->tm_min  <<':'
    <<  now->tm_sec;

    return(time_now.str());
}

CLog::~CLog()
{
    //dtor
}
