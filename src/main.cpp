/*

	This code has been tested on raspberry pis but should work on any linux system with a serial port, and gpio pins.
	Written by Simon Tsaoussis and Rewritten in C++ by Matthew Weger
	This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

	GPS configuration : ERGO protocol
	Baud rate : 38400
	Active messages :
		NAV-POSLLH Geodetic Position Solution, PAGE 66 of datasheet
		NAV-VELNED Velocity Solution in NED, PAGE 71 of datasheet
		NAV-STATUS Receiver Navigation Status
		  or
		NAV-SOL Navigation Solution Information



*/



#include <forward_list>
#include <stdio.h>
#include <thread>         // std::thread
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <iomanip>
#include "CLog.h"
#include <thread>         // std::thread
#include "CERGO_SERIAL.h"
#include "CERGO_GPS.h"
#include "CERGO_INTERNET.h"

void print_list(std::deque <uint8_t> ,std::string );


int main(int argc, char *argv[])
{
    int DEBUG_LEVEL = 0;
    if(argc >= 2)
    {
        DEBUG_LEVEL = atoi(argv[1]);
    }

    CLog * Log= new CLog; //inits the log
    CERGO_SERIAL * Serial = new CERGO_SERIAL(DEBUG_LEVEL) ; // inits the Serial class
    CERGO_GPS * GPS = new CERGO_GPS(DEBUG_LEVEL); // inits the GPS CLASS
    CERGO_INTERNET * Internet = new CERGO_INTERNET(DEBUG_LEVEL); // inits the INTERNET class

    Log ->add("\n############################################################ \n\n \t\t ERGO-PIXLE RESTARTED \n\n############################################################ \n ");
    std::deque <uint8_t> test_list;
    int data_int = 0;
    bool internet_light_set = false;

    int LIGHT_TIMER = 50;
    int SLEEP_TIMER = 150;
    enum light_colours_t{RED_RIGHT = 24,YELLOW_MIDDLE= 23,GREEN_LEFT=18};
    enum data_type_t{BAD_DATA,INSUFFICIENT_DATA,TIME_DATA,POS_DATA,FIX_DATA};
    enum debug_level_t{LOW=1,MEDIUM,HIGH};

    //Adds restarted message to log

    Serial->setval_gpio(true,RED_RIGHT);
    Serial->serial_setup(1337);

    std::deque <uint8_t> data_list; // list to store serial data
    std::stringstream test_string;
    std::thread ([&] { Internet->manage_list(); }).detach();

    while(true) // main management loop
    {
        Serial->data_read(data_list); // checks and reads incomming data
        while(!data_list.empty())
        {
            if(DEBUG_LEVEL >= MEDIUM)
            {
                test_list = data_list;
            }
            data_int = GPS->Read_data(data_list);
             if(data_int == FIX_DATA)
            {
                if(DEBUG_LEVEL >= MEDIUM)
                {
                    printf("fix data\n");
                }
            }
            if(data_int == POS_DATA)
            {
                if(DEBUG_LEVEL >= MEDIUM)
                {
                    printf("POS DATA\n");
                }
            }
            else if( data_int == TIME_DATA)//sends the serial data to be parsed
            {
                Serial->setval_gpio(true,GREEN_LEFT);
                GPS->packatize();
                Internet->set_check_archive(true);
                usleep(LIGHT_TIMER);
                Serial->setval_gpio(false,GREEN_LEFT);

                if(DEBUG_LEVEL >= HIGH)
                {
                    print_list(test_list,"Good data!");
                }
            }
            else if(data_int == INSUFFICIENT_DATA)
            {
                if(DEBUG_LEVEL >= HIGH)
                {
                    print_list(test_list,"Not enough data!");
                }
                break;
            }
            else if (data_int == BAD_DATA)
            {
                if(DEBUG_LEVEL >= HIGH)
                {
                    print_list(test_list,"Bad checksum!");
                }
            }
        }

        if(Internet->get_internet_availiable())
        {
            if(!internet_light_set)
            {
                internet_light_set =true;
                Serial->setval_gpio(true,YELLOW_MIDDLE);
            }
        }
        else
        {
            if(internet_light_set)
            {
                internet_light_set= false;
                Serial->setval_gpio(false,YELLOW_MIDDLE);
            }
        }

    }
}

void print_list(std::deque <uint8_t> print_list,std::string type)
{
    printf("%s",type.c_str());
    while(!print_list.empty())
    {
        printf("0x%X ",print_list.front());
        print_list.pop_front();
    }
    printf("\n\n");
}
