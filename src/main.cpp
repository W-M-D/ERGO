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



int main(int argc, char *argv[])
{
    int DEBUG_LEVEL = 0;
    if(argc >= 2)
    {
        DEBUG_LEVEL = atoi(argv[1]);
    }

    CLog * Log= new CLog; //inits the log
    CERGO_SERIAL Serial(DEBUG_LEVEL) ; // inits the Serial class
    CERGO_GPS GPS(DEBUG_LEVEL) ; // inits the GPS CLASS
    CERGO_INTERNET * Internet = new CERGO_INTERNET(DEBUG_LEVEL); // inits the INTERNET class

    std::deque <uint8_t> test_list;
    int counter = 0;
    int data_int = 0;
    bool internet_light_set = false;

    //Adds restarted message to log
    Log ->add("\n############################################################ \n\n \t\t ERGO-PIXLE RESTARTED \n\n############################################################ \n ");

    Serial.setval_gpio(1,24);
    Serial.serial_setup(1337);
    std::deque <uint8_t> data_list; // list to store serial data
    std::stringstream test_string;
    std::thread ([&] { Internet->manage_list(); }).detach();

    while(true) // main management loop
    {
        usleep(200);
        counter = Serial.data_read(data_list); // checks for incomming data
        while(!data_list.empty())
        {
            if(DEBUG_LEVEL >= 2)
            {
                test_list = data_list;
            }
            data_int =GPS.Read_data(data_list);
             if(data_int == 5)
            {
                if(DEBUG_LEVEL >=2)
                {
                    printf("fix data\n");
                }
            }
            if(data_int == 4)
            {
                if(DEBUG_LEVEL >=2)
                {
                    printf("POS DATA\n");
                }
            }
            else if( data_int == 3)//sends the serial data to be parsed
            {
                Serial.setval_gpio(1,18);
                GPS.packatize();
                Internet->set_check_archive(true);
                Serial.setval_gpio(0,18);
                if(DEBUG_LEVEL >=3)
                {
                    printf("Good data!");
                    while(!test_list.empty())
                    {
                        printf("0x%X ",test_list.front());
                        test_list.pop_front();
                    }
                    printf("\n\n");
                }
            }
            else if(data_int == 2)
            {
                if(DEBUG_LEVEL >=3)
                {
                    printf("Not enough data!");
                    while(!test_list.empty())
                    {
                        printf("0x%X ",test_list.front());
                        test_list.pop_front();
                    }
                    printf("\n\n");
                }
                break;
            }
            else if (data_int == 0)
            {
                if(DEBUG_LEVEL >=3)
                {
                    printf("Bad checksum!");
                    while(!test_list.empty())
                    {
                        printf("0x%X ",test_list.front());
                        test_list.pop_front();
                    }
                    printf("\n\n");
                }
            }
        }

        if(Internet->get_internet_availiable() )
        {
            if(!internet_light_set)
            {
                internet_light_set =true;
                Serial.setval_gpio(1,23);
            }
        }
        else
        {
            if(internet_light_set)
            {
                internet_light_set= false;
                Serial.setval_gpio(0,23);
            }
        }

    }
}

