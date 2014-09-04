#ifndef CERGO_SERIAL_H
#define CERGO_SERIAL_H
#define SERIAL 3
#define GPIO_MODE_PATH "/sys/devices/virtual/misc/gpio/mode/"
#define GPIO_FILENAME "gpio"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include "CLog.h"
#include <forward_list>
#include <deque>
#include <sys/poll.h>




class CERGO_SERIAL
{
public:

    CERGO_SERIAL(int,int);
    bool  serial_init(int);
    void serial_setup(int);
    bool getUBX_ACK(int *);

    void sendUBX(int *,size_t);

    int data_read (std::deque <uint8_t> &);
    void setPinMode(int pinID, int mode);
    void writeFile(int fileID, int value);

    int export_gpio(int);
    int unexport_gpio(int );
    int setdir_gpio(int gpionum);
    int setval_gpio(int val,int gpionum);
    int getval_gpio(int gpionum);



    virtual ~CERGO_SERIAL();
protected:

private:
    int v_time;
    int tty_fd;
    struct termios tio;
    uint8_t data_in;
    int DEBUG_LEVEL;

    // struct termios port_options;

    CLog * Log;




};

#endif // CERGO_SERIAL_H


