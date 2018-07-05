#include <stdio.h>
#include <unistd.h>

#include "../include/libdeltarobot/protocol.h"
#define PROTOCOL_C_NO_INCLUDE
#include "../src/protocol.c"

int fileptr_write(void *buf, int bytes, void *usr)
{
    fwrite(buf, bytes, 1, (FILE*)usr);
    char newline = '\n';
    fwrite(&newline, 1, 1, (FILE*)usr);
    fflush((FILE*)usr);
    return 0;
}


int ugetchar(void)
{
    char c;
    
    return (read(0, &c, 1) == 1) ? (unsigned char) c : EOF;
}

int main()
{
    FILE *serial = fopen("/dev/ttyACM0", "rw");
    
    if(!serial)
    {
        perror("/dev/ttyACM0");
        return -1;
    }
    
    char c;
    while ( (c = ugetchar()) != EOF )
    {
        printf("got char '%c'\n", c);
        
        struct protocol_msg msg;
        msg.opcode = c;
        msg.data_s = 0;
        
        if(dp_send(&msg, &fileptr_write, serial))
            printf("\ndp_send error\n");
    }
    
    return 0;
}

