#include "CERGO_INTERNET.h"

CERGO_INTERNET::CERGO_INTERNET(int debug_level)
{
    DEBUG_LEVEL = debug_level;
    MAX_INTERNET_TIMEOUT = 1000;
    first_pass = true;
    check_archive = false;
}

bool CERGO_INTERNET::internet_availiable()
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();

    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://ergotelescope.org/postevent3.asp?dta=");
        if ((res = curl_easy_perform(curl)) != CURLE_OK)
        {
            switch (res)
            {
            case CURLE_COULDNT_CONNECT:
                curl_easy_cleanup(curl);
                return false;

            case CURLE_COULDNT_RESOLVE_HOST:
                curl_easy_cleanup(curl);
                return false;

            case CURLE_COULDNT_RESOLVE_PROXY:
                curl_easy_cleanup(curl);
                return false;

            default:
                curl_easy_cleanup(curl);
                return false;

            }
        }
        curl_easy_cleanup(curl);
        return true;
    }
    curl_easy_cleanup(curl);
    return false;
}

/*bool CERGO_INTERNET::internet_availiable()
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();

    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://ergotelescope.org/postevent3.asp?dta=");
        if ((res = curl_easy_perform(curl)) != CURLE_OK)
        {
            switch (res)
            {
            case CURLE_COULDNT_CONNECT:
                curl_easy_cleanup(curl);
                return false;

            case CURLE_COULDNT_RESOLVE_HOST:
                curl_easy_cleanup(curl);
                return false;

            case CURLE_COULDNT_RESOLVE_PROXY:
                curl_easy_cleanup(curl);
                return false;

            default:
                curl_easy_cleanup(curl);
                return false;

            }
        }
        curl_easy_cleanup(curl);
        return false;
    }
    curl_easy_cleanup(curl);
    return false;
}*/

bool CERGO_INTERNET::send_string(const std::string & data_string)
{
    CURL * curl;
    CURLcode res;
    curl = curl_easy_init();
    std::string sending_string = "http://ergotelescope.org/postevent3.asp?dta=";;
    sending_string.append(data_string);

    if(curl)
    {
        if(DEBUG_LEVEL >= 3)
        {
            Log->add("%s \n",sending_string.c_str());
        }
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

void CERGO_INTERNET::manage_list(std::string event_string)
{
        static std::forward_list <std::string> string_list;

        string_list.emplace_after(string_list.before_begin(),event_string);
        for(int i = 0; i < 20;i++)// only send 20 strings at a time to prevent hanging
        {
          if(!string_list.empty()) // checks if the string list has any strings in it
          {
              if(internet_availiable())
              {
                if(first_pass == true)// if this is the first time running this function
                {
                      Log->archive_load(string_list);// check the archive there might of been a crash etc....
                      first_pass = false;
                }
                if (check_archive)// if the archive flag has been set
                {
                    Log->archive_load(string_list);// check the archive
                    Log->add("Internet connection restored");//this probably means that we have server connection again
                    check_archive = false; // reset the flag
                }

                if(send_string(URLEncode(string_list.front().c_str()))) // calls the function that sends data to the server returns true on success
                {
                    string_list.pop_front();// pops the first element
                }
              }
              else
              {
                if(!check_archive) // if the archive flag is not set
                {
                    Log->add("ERROR :NO CONNECTION TO SERVER ARCHIVING STRINGS "); //there is probably no connection to the server
                    check_archive = true; // set the archive flag
                }
                Log->archive_save(string_list); // archive the strings
                break;
              }
          }
          else
          {
            break;
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
