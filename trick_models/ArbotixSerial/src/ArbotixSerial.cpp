
#include <algorithm>
#include <iostream>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <string.h>

#include "ArbotixSerial/include/ArbotixSerial.hh"

ArbotixSerial::ArbotixSerial( std::string in_port ) :
 port(in_port),
 fh(-1) {}

int ArbotixSerial::initialization() {
    fh = open(port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY ) ;
    if ( fh == -1 ) {
        perror("could not open serial port") ;
        return fh ;
    }

    struct termios tio ;
    tcgetattr(fh, &tio) ;
    cfsetospeed(&tio,B1000000);
    cfsetispeed(&tio,B1000000);
    tcsetattr(fh,TCSANOW,&tio);

    return 0 ;
}

int ArbotixSerial::read_data(void * read_buffer, unsigned int read_size) {
    unsigned char * buf = (unsigned char *)read_buffer ;
    int num_bytes = -1 ;
    int total_bytes = 0 ;
    unsigned char temp_ch = 0xFF ;
    unsigned int read_index = 0 ;

    if ( fh != -1 ) {
        do {
            num_bytes = ::read(fh, &temp_ch, 1) ;
            if ( num_bytes == 0 ) {
                return 0 ;
            } else if ( num_bytes != -1 ) {
                partial_read.push_back(temp_ch) ;
            }
        } while ( num_bytes != -1 and temp_ch != 0 ) ;

        if ( temp_ch == 0 ) {
            unsigned int ii ;
            unsigned int message_len = partial_read.size() - 1;
            //std::cout << "message_len = " << message_len << " message = " ;
            for ( ii = 0 ; ii < message_len and ii < read_size ; ii++ ) {
                //std::cout << (int)partial_read[ii] << " " ;
                buf[ii] = partial_read[ii] ;
            }
            //std::cout << std::endl ;
            partial_read.clear() ;
            return message_len ;
        }
    }
    return -1 ;
}

int ArbotixSerial::write_data(void * write_buffer, unsigned int write_size) {
    int num_bytes = -1 ;
    if ( fh != -1 ) {
        num_bytes = ::write(fh, write_buffer, write_size) ;
    }
    return num_bytes ;
}

int ArbotixSerial::shutdown() {
    if ( fh != -1 ) {
        close(fh) ;
    }
    return 0 ;
}

#define FinishBlock(X) (*code_ptr = (X), code_ptr = dst++, code = 0x01)

int cobs_encode(void * src , unsigned int length, void * dest) {
    const unsigned char *ptr = (const unsigned char *)src ;
    unsigned char *dst = (unsigned char *)dest ;
    const unsigned char *end = ptr + length;
    unsigned char *code_ptr = dst++;
    unsigned char code = 0x01;

    while (ptr < end) {
        if (*ptr == 0) {
            FinishBlock(code);
        } else {
            *dst++ = *ptr;
            code++;
            if (code == 0xFF) {
                FinishBlock(code);
            }
        }
        ptr++;
    }
    FinishBlock(code);
    return (int)(dst - (unsigned char *)dest) ;
}


int cobs_decode(void * src , unsigned int length, void * dest) {
    const unsigned char *ptr = (const unsigned char *)src ;
    unsigned char *dst  = (unsigned char *)dest ;
    const unsigned char *end = ptr + length;

    while (ptr < end) {
        int i, code = *ptr++;
        for (i=1; ptr<end && i<code; i++) {
            *dst++ = *ptr++;
        }
        if (code < 0xFF) {
          *dst++ = 0;
        }
    }
    return (int)(dst - (unsigned char *)dest - 1) ;
}
