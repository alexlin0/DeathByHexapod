
/**
PURPOSE:
    (Arbotix Serial interface.)
LIBRARY DEPENDENCY:
    ((../src/ArbotixSerial.cpp))
*/

#ifndef _ARBOTIXSERIAL_HH_
#define _ARBOTIXSERIAL_HH_

#include <string>
#include <vector>

class ArbotixSerial {
    public:
        ArbotixSerial( std::string in_port = "/dev/ttyUSB0" ) ;
        virtual int initialization() ;
        virtual int read_data(void * read_buffer, unsigned int read_size) ;
        virtual int write_data(void * write_buffer, unsigned int write_size) ;
        virtual int shutdown() ;

        std::string port ;

    protected:
        int fh ;
        std::vector< unsigned char > partial_read ;
} ;

int cobs_encode(void * src , unsigned int length, void * dest) ;
int cobs_decode(void * src , unsigned int length, void * dest) ;
#endif
