
unsigned char partial_buffer[256] ;
unsigned int partial_index ;

unsigned short test_array[] = {  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
                                11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                21, 22, 23, 24, 25, 26, 27, 28, 20, 30,
                                31, 32, 33, 34, 35, 36 } ;

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

int read_serial_message(void * read_buffer, unsigned int read_size) {

    unsigned char temp_ch = 0xFF ;
    while ( Serial.available() ) {
        temp_ch = Serial.read() ;
        partial_buffer[partial_index++] = temp_ch ;
        // break if last character was a 0
        if ( temp_ch == 0 ) {
            break ;
        }
    }

    if ( temp_ch == 0 ) {
        unsigned int ii ;
        unsigned int message_len = partial_index - 1 ;
        unsigned char * buf = (unsigned char *)read_buffer ;
        for ( ii = 0 ; ii < partial_index and ii < read_size ; ii++ ) {
            buf[ii] = partial_buffer[ii] ;
        }
        partial_index = 0 ;
        return message_len ;
    }

    return -1 ;
}

int get_command(void *command) {
    int ret ;
    unsigned char read_buffer[256] ;

    ret = read_serial_message(read_buffer, sizeof(read_buffer)) ;
    if ( ret > 0 ) {
        ret = cobs_decode(read_buffer, ret, command) ;
    }
    return ret ;
}



void setup() {
  Serial.begin(1000000);
  partial_index = 0 ;
  //pinMode(0, OUTPUT) ;
}


void loop() {
  int ii ;
  int ret = 0;
  unsigned char command[256] ;
  unsigned char write_buffer[256] ;
  int write_len ;

  for ( ii = 0 ; ii < 18 ; ii++ ) {
      // execute any commands
      ret = get_command(command) ;

      if ( ret > 0 ) {
          //command[0] = ret ;
          write_len = cobs_encode( command , ret, write_buffer) ;
          write_buffer[write_len++] = 0 ;
          Serial.write(write_buffer, write_len) ;
      }

      //digitalWrite(0, HIGH) ;
      // simulate talking to a servo.
      //delay(100);
      //digitalWrite(0, LOW) ;
      //delay(100);

      //test_array[0] = ret ;
      //test_array[1] += 1 ;

      write_len = cobs_encode(&test_array[ii * 2] , 4, write_buffer) ;
      write_buffer[write_len++] = 0 ;
      Serial.write(write_buffer, write_len) ;

      delay(10);
  }
}
