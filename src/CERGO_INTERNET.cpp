#include "CERGO_INTERNET.h"

CERGO_INTERNET::CERGO_INTERNET(int debug_level)
{
    DEBUG_LEVEL = debug_level;
    MAX_INTERNET_TIMEOUT = 1000;
    first_pass = true;
    internet_outage = true;
    internet_connection = false;
}



int CERGO_INTERNET::internet_availiable()
{
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();

    if(curl)
    {
        curl_easy_setopt(curl,CURLOPT_TIMEOUT,2);
        curl_easy_setopt(curl, CURLOPT_URL, "data.ergotelescope.org");
        while ((res = curl_easy_perform(curl)) != CURLE_OK)
        {
            switch (res)
            {
            case CURLE_COULDNT_CONNECT:
                curl_easy_cleanup(curl);
                return 1;
                break;
            case CURLE_COULDNT_RESOLVE_HOST:
                curl_easy_cleanup(curl);
                return 2;
                break;

            case CURLE_COULDNT_RESOLVE_PROXY:
                curl_easy_cleanup(curl);
                return 3;
                break;
            }
        }
        curl_easy_cleanup(curl);
        return 0;
    }
    curl_easy_cleanup(curl);
    return 5;
}

bool CERGO_INTERNET::send_string(const std::string & data_string)
{
    if(internet_connection)
    {
      CURL * curl;
      CURLcode res;
      curl = curl_easy_init();
      std::string sending_string = "http://ergotelescope.org/postevent3.asp?dta=";
      sending_string.append(data_string);

      if(curl)
      {
          if(DEBUG_LEVEL >= 3)
          {
              Log->add("%s \n",sending_string.c_str());
          }
          curl_easy_setopt(curl,CURLOPT_TIMEOUT,3);
          curl_easy_setopt(curl, CURLOPT_URL, sending_string.c_str());

          /* Perform the request, res will get the return code */
          res = curl_easy_perform(curl);
          /* Check for errors */
          if(res != CURLE_OK)
          {
              curl_easy_cleanup(curl);
              return false;
          }

          /* always cleanup */
          curl_easy_cleanup(curl);
          return true;
      }
    /* always cleanup */
      curl_easy_cleanup(curl);
    }
    return false;

}

void CERGO_INTERNET::reset_internet(clock_t & timer,int MAX_TIME)
{
    int test = 0;
    if (((clock() - timer)/( CLOCKS_PER_SEC))>MAX_TIME)
    {
        if(DEBUG_LEVEL >= 1)
        {
            Log->add("RESTARTING WICD \n\n");
        }
        FILE * f = popen("service wicd restart", "r");
        char Line[100];
        usleep( 2 * 1000 );
        while (fgets(Line, 100, f) != NULL)
        {
            if(DEBUG_LEVEL >= 1)
            {
                Log->add("%s",Line);
            }
            test = strncmp( "[ ok ]" ,Line,6);
            if(test == 0)
            {
                timer = 0;
                break;
            }
            else
            {
                if(DEBUG_LEVEL >= 1)
                {
                    Log->add("Test char diff  = %d \n " , test);
                }
                timer = 900;
            }
        }
        pclose(f);
    }
}

void CERGO_INTERNET::manage_list()
{
        static std::forward_list <std::string> string_list;
        int internet_int= internet_availiable();
        if(internet_int == 0)
        {
          if(internet_outage)
          {
            internet_connection = true;
            Log->add("CONNECTION RESTORED");
            internet_outage = false;
          }
          Log->archive_load(string_list);

          for(int i=0 ; i < 5; i++)
          {
            if(!string_list.empty())
            {
              if(send_string(URLEncode(string_list.front().c_str()))) // calls the function that sends data to the server returns true on success
              {
                  if(DEBUG_LEVEL >= 1)
                  {
                    Log->add("Sent string : %s" , string_list.front().c_str());
                  }
                  string_list.pop_front();// pops the first element
              }
            }
          }
        }
        else
        {
          if(!internet_outage) // if the outage flag is not set
          {
              internet_connection = false;
              if(internet_int == 1)
              {
                Log->add("ERROR : INTERNET COULDNT_CONNECT "); //there is probably no connection to the server
              }
              else if(internet_int == 2)
              {
                Log->add("ERROR :INTERNET COULDNT_RESOLVE_HOST "); //there is probably no connection to the server
              }
              else if(internet_int == 3)
              {
                Log->add("ERROR :INTERNET CURLE_COULDNT_RESOLVE_PROXY"); //there is probably no connection to the server
              }
              else if(internet_int == 4)
              {
                Log->add("ERROR :INTERNET UNKNOWN ERROR 1"); //there is probably no connection to the server
              }
              else if(internet_int == 5)
              {
                Log->add("ERROR :INTERNET UNKNOWN ERROR 2"); //there is probably no connection to the server
              }
              internet_outage = true;
          }
        }
}

std::string CERGO_INTERNET::URLEncode(const char* msg)
{
    const char *hex = "0123456789abcdef";
    std::string encodedMsg = "";

    while (*msg!='\0')
    {
        if( ('a' <= *msg && *msg <= 'z')
                || ('A' <= *msg && *msg <= 'Z')
                || ('0' <= *msg && *msg <= '9') )
        {
            encodedMsg += *msg;
        }
        else
        {
            encodedMsg += '%';
            encodedMsg += hex[*msg >> 4];
            encodedMsg += hex[*msg & 15];
        }
        msg++;
    }
    return encodedMsg;
}

CERGO_INTERNET::~CERGO_INTERNET()
{
    //dtor
}
