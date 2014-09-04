#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include "CERGO_SERIAL.h"
static const char* portName = "/dev/ttyAMA0";

CERGO_SERIAL::CERGO_SERIAL(int debug_level, int V_TIME)
{
    DEBUG_LEVEL = debug_level;
    if(DEBUG_LEVEL >= 1)
    {
          Log->add("serial port %d \n\n",tty_fd);
    }    v_time = V_TIME;
    while(!serial_init(9600))
    {
        sleep(60);
    }
    export_gpio(24);//LEFT
    export_gpio(23);//MIDDLE
    export_gpio(18);//RIGHT CLOSEST TO GEIGER COUNTER
}

bool CERGO_SERIAL::serial_init(int baud)
{
        if(tty_fd == 3)
        {
          close(tty_fd);
          usleep( 1 * 1000 );
          if(DEBUG_LEVEL >= 3)
          {
                      Log->add(" reset TTY_FD = %d",tty_fd);
          }
        }
        else
        {
          if(DEBUG_LEVEL >= 3)
          {
                      Log->add("TTY_FD = %d",tty_fd);
          }
        }
        tcgetattr(tty_fd, &tio);
        tcflush(tty_fd, TCIOFLUSH);

        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // make the reads non-blocking
        memset(&tio,0,sizeof(tio));
        tio.c_iflag=0;
        tio.c_oflag=0;
        tio.c_cflag=CREAD|CLOCAL;           // 8n1, see termios.h for more information        tio.c_cflag &= ~CSIZE;
        tio.c_cflag |= CS8;
        tio.c_cflag &= ~PARENB;// no parity
        tio.c_cflag &= ~CSTOPB;// one stop bit

        tio.c_lflag=0;
        tio.c_cc[VMIN]=5;
        tio.c_cc[VTIME]=v_time;

        tty_fd=open(portName, O_RDWR |O_NONBLOCK);
        if (baud == 9600)
        {
        cfsetospeed(&tio,B9600);            // 115200 baud
        cfsetispeed(&tio,B9600);            // 115200 baud
        usleep( 3 * 1000 );
        }

        if (baud == 38400)
        {
        cfsetospeed(&tio,B38400);            // 115200 baud
        cfsetispeed(&tio,B38400);            // 115200 baud
        usleep( 3 * 1000 );
        }


        tcsetattr(tty_fd,TCSANOW,&tio);
        tcflush(tty_fd, TCIOFLUSH);
        fcntl(tty_fd, F_SETFL, O_NONBLOCK);
        usleep( 1 * 1000 );


    return true;
}


int CERGO_SERIAL::data_read (std::deque <uint8_t> & data_list)
{
    struct pollfd fds[1];
    fds[0].fd = tty_fd;
    fds[0].events = POLLIN ;
    int pollrc = poll( fds, 1,10);
    if (pollrc < 0)
    {
        perror("poll");
    }
    else if( pollrc > 0)
    {
        if( fds[0].revents )
        {
          if( POLLIN)
          {
            uint8_t buff[1024];
            memset(buff, 0, sizeof(buff));
            ssize_t rc = read(tty_fd, &buff, sizeof(buff) );
            if (rc > 0)
            {
                for(unsigned int x = 0;x < (rc/sizeof(char)) ;x++)
                {
                        data_list.emplace_back(buff[x]);
                }
                return rc;
            }
        }
        }
    }
    return -1;
}


int CERGO_SERIAL::export_gpio(int gpionum)
{
	std::string export_str = "/sys/class/gpio/export";
	std::ofstream exportgpio(export_str.c_str()); // Open "export" file. Convert C++ string to C string. Required for all Linux pathnames
	if (exportgpio < 0){
		return -1;
	}

	exportgpio << gpionum ; //write GPIO number to export
    exportgpio.close(); //close export file
    setdir_gpio(gpionum);
    return 0;
}


int CERGO_SERIAL::unexport_gpio(int gpionum)
{
  setval_gpio(0,gpionum);
	std::string unexport_str = "/sys/class/gpio/unexport";
	std::ofstream unexportgpio(unexport_str.c_str()); //Open unexport file
	if (unexportgpio < 0){
		return -1;
	}

	unexportgpio << gpionum ; //write GPIO number to unexport
    unexportgpio.close(); //close unexport file
    return 0;
}

int CERGO_SERIAL::setdir_gpio(int gpionum)
{
  std::string direction = "out";
  std::stringstream setdir_str;
  setdir_str << "/sys/class/gpio/gpio" << gpionum << "/direction";
	std::ofstream setdirgpio(setdir_str.str().c_str()); // open direction file for gpio
		if (setdirgpio < 0)
    {
			return -1;
		}

		setdirgpio << direction; //write direction to direction file
		setdirgpio.close(); // close direction file
	    return 0;
}

int CERGO_SERIAL::setval_gpio(int val,int gpionum)
{

	std::stringstream setval_str;
	setval_str << "/sys/class/gpio/gpio" << gpionum << "/value";
	std::ofstream setvalgpio(setval_str.str()); // open value file for gpio
		if (setvalgpio < 0){
			return -1;
		}

		setvalgpio << val ;//write value to value file
		setvalgpio.close();// close value file
	    return 0;
}

int CERGO_SERIAL::getval_gpio(int gpionum)
{
  std::string val;
	std::stringstream getval_str;
	getval_str << "/sys/class/gpio/gpio" << gpionum << "/value";
	std::ifstream getvalgpio(getval_str.str());// open value file for gpio
	if (getvalgpio < 0){
		return -1;
			}

	getvalgpio >> val ;  //read gpio value
	getvalgpio.close(); //close the value file

	if(val != "0")
    return 1;
	else
		return 0;

    return -1;
}




void CERGO_SERIAL::serial_setup(int ID)
{
    if (ID == 1337)
    {

        int setNav499[] = {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x00, 0x96, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8B, 0x54};
        int setNav49[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xC8, 0x00, 0x01, 0x00, 0x01, 0x00, 0xDE, 0x6A};
        int setNav5[] = {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x27, 0xCE };
        int setNav2[] =  {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x0D, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x20, 0x25};
        int setNav[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x13, 0xBE };



       sendUBX(setNav499,(sizeof(setNav499)/sizeof(int)));
       Log->add("Sucess:setNav499 %s" ,getUBX_ACK(setNav499) ? "true" : "false");
        serial_init(38400);

        sendUBX(setNav49,(sizeof(setNav49)/sizeof(int)));
        Log->add("Sucess:setNav49 %s" ,getUBX_ACK(setNav49) ? "true" : "false");

        sendUBX(setNav5,(sizeof(setNav5)/sizeof(int)));
        Log->add("Sucess:setNav5 %s" ,getUBX_ACK(setNav5) ? "true" : "false");


        sendUBX(setNav2,(sizeof(setNav2)/sizeof(int)));
        Log->add("Sucess:setNav2 %s" ,getUBX_ACK(setNav2) ? "true" : "false");


        sendUBX(setNav,(sizeof(setNav)/sizeof(int)));
        Log->add("Sucess:setNav %s" ,getUBX_ACK(setNav) ? "true" : "false");


    }

    if (ID == 247)
    {
        int setNav[] = {0xB5, 0x62, 0x06, 0x03, 0x1C, 0x00, 0x06, 0x03, 0x10, 0x18, 0x14, 0x05, 0x00, 0x3C, 0x3C, 0x14, 0xE8, 0x03, 0x00, 0x00, 0x00, 0x17, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x0F, 0x00, 0x00, 0x00, 0x91, 0x54};
        sendUBX(setNav,sizeof(setNav));

        Log->add("Sucess: %s" ,getUBX_ACK(setNav) ? "true" : "false");
    }
}

void CERGO_SERIAL::sendUBX(int *MSG,size_t len)
{
     lseek(tty_fd, 0, SEEK_SET);
     if(DEBUG_LEVEL >= 3)
     {
            Log->add("\n\nSENDING BYTES:\n");
     }
     for(unsigned int i =0; i < len; i++)
     {
         uint8_t c = MSG[i];
         if(POLLOUT)
         {
          write(tty_fd,&c,1);
          if(DEBUG_LEVEL >= 3)
          {
                Log->add("0x%X ",MSG[i]);
          }
         }
        else
        {
          i--;
        }
     }
      if(DEBUG_LEVEL >= 3)
      {
          Log->add("\n\n\n\n");
      }
}

bool CERGO_SERIAL::getUBX_ACK(int *MSG)
{
    int ackByteID = 0;
    int ackPacket[10];
    clock_t start_clock = clock();
   std::deque <uint8_t> data_list; // list to store serial data
    if(DEBUG_LEVEL >= 2)
    {
        Log->add(" * Reading ACK response: ");
    }
    // Construct the expected ACK packet
    ackPacket[0] = 0xB5;	// header CPOCMSG[i]MSG[i]MSG[i]C
    ackPacket[1] = 0x62;	// header
    ackPacket[2] = 0x05;	// class
    ackPacket[3] = 0x01;	// id
    ackPacket[4] = 0x02;	// length
    ackPacket[5] = 0x00;
    ackPacket[6] = MSG[2];	// ACK class
    ackPacket[7] = MSG[3];	// ACK id
    ackPacket[8] = 0;		// CK_A
    ackPacket[9] = 0;		// CK_B

    // Calculate the checksums
    for (int i=2; i<8; i++)
    {
        ackPacket[8] = ackPacket[8] + ackPacket[i];
        ackPacket[9] = ackPacket[9] + ackPacket[8];
    }

    while (true)
    {
        data_read(data_list);
        // Test for success
        if (ackByteID > 9)
        {
            // All packets in order!
            if(DEBUG_LEVEL >= 2)
          {
            Log->add(" (SUCCESS!)");
          }
            return true;
        }

        // Make sure data is available to read
        if (((clock() - start_clock)/( CLOCKS_PER_SEC / 1000 )) > 30)
        {
            if(DEBUG_LEVEL >= 2)
            {
                Log->add(" (FAILED!)");
            }
            return false;
        }

        // Check that bytes arrive in sequence as per expected ACK packet
        while(!data_list.empty())
        {


        if(ackByteID > 9)
        {
            break;
        }
        if (data_list.front() == ackPacket[ackByteID])
        {
            ackByteID++;
        }
        else
        {
            ackByteID = 0;	// Reset and look again, invalid order
        }

        if(DEBUG_LEVEL >= 3)
        {
              Log->add("0x%X ",data_list.front());
        }

        data_list.pop_front();
        }

    }


    }




CERGO_SERIAL::~CERGO_SERIAL()
{
    unexport_gpio(18);
    unexport_gpio(23);
    unexport_gpio(24);
    Log->add("closing %d",tty_fd);
    close(tty_fd);
}

