#ifndef PROTOCOL_C_NO_INCLUDE
#include "libdeltarobot/protocol.h"
#endif

#if __unix__
#ifndef PROTOCOL_C_NO_DEBUG
#define PROTOCOL_C_DEBUG_PRINT
#endif
#endif

#ifdef PROTOCOL_C_DEBUG_PRINT
#include <stdio.h>
#endif

//BEGIN helper functions -------------------------------------------------------

inline static void dp_helper_drop_bytes( struct protocol_data *data, int bytes )
{
    if(!bytes)
        return;
    
    if(bytes > DP_BUFSIZE)
        bytes = DP_BUFSIZE;
    
    data->m_idx += bytes;
    data->m_idx %= DP_BUFSIZE;
    data->m_sz -= bytes;
}

// get "end" index
inline static int dp_helper_end( struct protocol_data *data) 
{
    return ( data->m_idx + data->m_sz ) % DP_BUFSIZE;
}

// write byte in buffer with guaranteed wrap-around
inline static void dp_helper_modwrite( struct protocol_data *data, int idx, char val ) 
{
    while(idx < 0)
        idx += DP_BUFSIZE;
    data->m_buf[idx % DP_BUFSIZE] = val;
}

// read byte from buffer with guaranteed wrap-around
inline static char dp_helper_modread( struct protocol_data *data, int idx) 
{
    while(idx < 0)
        idx += DP_BUFSIZE;
    return data->m_buf[idx % DP_BUFSIZE];
}

inline static unsigned char dp_helper_checksum(struct protocol_msg *msg )
{
    unsigned int modsum = DP_PROTO_FRAME_START;
    
    modsum = (modsum + msg->opcode) % 256;
    modsum = (modsum + msg->data_s) % 256;
    
    for(int i = 0; i < msg->data_s; i++)
    {
        modsum = (modsum + msg->data[i]) % 256;
    }
    
    // checksum byte itself considered zero in checksum calculation
    
    modsum = (modsum + DP_PROTO_FRAME_END) % 256;
    return modsum;
}

inline static int dp_helper_opcode_valid( unsigned char opcode )
{
    static const char *opcodes = DP_PROTO_ALL_OPCODES;
    const char *tmp = opcodes;


    while(*tmp)
    {
        if(((unsigned char)(*tmp)) == opcode)
            return 1;
        tmp ++;
    }
    
    return 0;
}

//END helper functions ---------------------------------------------------------



int dp_process( struct protocol_data *data, void *buf, int bytes )
{
    int ret = DP_RC_SUCCESS;
    
    if(bytes > DP_BUFSIZE)
    {
        bytes = DP_BUFSIZE;
        ret = DP_RC_OVERFLOW;
    }
    
    if((data->m_sz+bytes) > DP_BUFSIZE)
    {
        dp_helper_drop_bytes(data, data->m_sz+bytes - DP_BUFSIZE );
        ret = DP_RC_OVERFLOW;
    }
    
    for (int i = 0; i < bytes; i ++)
    {
        dp_helper_modwrite( data, dp_helper_end(data) + i, ((char*)buf)[i]);
    }
    data->m_sz += bytes;
    
    return ret;
}

#define ST_SEARCH_FRAME 0
#define ST_READ_OPCODE 1
#define ST_READ_SIZE 2
#define ST_READ_DATA 3
#define ST_CHECKSUM 4
#define ST_FINALIZE 5

int dp_recv( struct protocol_data *data, struct protocol_msg *msg )
{
    int ret = DP_RC_NO_FRAME;
    msg->opcode = -1;
    
    int state = ST_SEARCH_FRAME;
    int data_counter = 0;
    unsigned char cs;
    
    for(int i = data->m_idx; i < (data->m_idx + data->m_sz); i++)
    {
        char c = dp_helper_modread(data, i);
        
        #ifdef PROTOCOL_C_DEBUG_PRINT
        printf("PROTOCOL RECV char is 0x%02x\n", (int)(unsigned char)c);
        #endif
        
        switch(state)
        {
            case ST_SEARCH_FRAME:
                if(c == DP_PROTO_FRAME_START)
                {
                    #ifdef PROTOCOL_C_DEBUG_PRINT
                    printf("PROTOCOL RECV found frame start\n");
                    #endif
                    state = ST_READ_OPCODE;
                }
                else 
                {
                    ret = DP_RC_GARBAGE;
                    #ifdef PROTOCOL_C_DEBUG_PRINT
                    printf("PROTOCOL RECV garbage\n");
                    #endif
                }
                break;
                
            case ST_READ_OPCODE:
                if(dp_helper_opcode_valid(*(unsigned char*)&c))
                {
                    msg->opcode = c;
                    state = ST_READ_SIZE;
                    
                    #ifdef PROTOCOL_C_DEBUG_PRINT
                    printf("PROTOCOL RECV found opcode\n");
                    #endif
                }
                else
                {
                    state = ST_SEARCH_FRAME;
                    
                    #ifdef PROTOCOL_C_DEBUG_PRINT
                    printf("PROTOCOL RECV invalid opcode, restart\n");
                    #endif
                }
                break;
            
            case ST_READ_SIZE:
                msg->data_s = (unsigned char) c;
                data_counter = 0;
                
                state = msg->data_s? ST_READ_DATA : ST_CHECKSUM;
                #ifdef PROTOCOL_C_DEBUG_PRINT
                printf("PROTOCOL RECV read size\n");
                #endif
                
                break;
            
            case ST_READ_DATA:
                if(data_counter < sizeof(msg->data))
                    msg->data[data_counter] = c;
                data_counter++;
                if(data_counter == msg->data_s)
                {
                    
                    #ifdef PROTOCOL_C_DEBUG_PRINT
                    printf("PROTOCOL RECV read data finish\n");
                    #endif
                    state = ST_CHECKSUM;
                }
                break;
            
            case ST_CHECKSUM:
                cs = dp_helper_checksum(msg);
                if(cs == (unsigned char) c)
                {
                    // checksum match
                    msg->checksum_ok = 1;
                    state = ST_FINALIZE;
                    
                    #ifdef PROTOCOL_C_DEBUG_PRINT
                    printf("PROTOCOL RECV checksum ok: %d\n", (int)cs);
                    #endif
                }
                else
                {
                    // checksim mismatch
                    msg->checksum_ok = 0;
                    state = ST_SEARCH_FRAME;
                    
                    #ifdef PROTOCOL_C_DEBUG_PRINT
                    printf("PROTOCOL RECV checksum failure: %d, expected %d\n", (int)cs,  (int)(unsigned char)c);
                    #endif
                }
                break;
            
            case ST_FINALIZE:
                if(c == DP_PROTO_FRAME_END)
                {
                    int endval = (data->m_idx + data->m_sz);
                    dp_helper_drop_bytes(data, i - data->m_idx + 1);
                    i = endval; // force for structure to end, because break 
                                // would be understood as a case directive
                    ret = DP_RC_SUCCESS;
                    
                    #ifdef PROTOCOL_C_DEBUG_PRINT
                    printf("PROTOCOL RECV frame over, success\n");
                    #endif
                    
                }
                else
                {
                    state = ST_SEARCH_FRAME;
                    
                    
                    #ifdef PROTOCOL_C_DEBUG_PRINT
                    printf("PROTOCOL RECV frame end missing\n");
                    #endif
                }
                break;
        }
    }
    
    return ret;
}

int dp_send( struct protocol_msg *msg, send_func_t sendfunc, void* usr )
{
    // cool, we are going to be sending a message
    char buf[1 + 1 + 1 + 32 + 1 + 1];
    //      st  op  sz  data  ck  end
    int sz = 0;
    
    buf[0] = DP_PROTO_FRAME_START; sz ++;
    buf[1] = (unsigned char) msg->opcode; sz ++;
    buf[2] = (unsigned char) msg->data_s; sz ++;
    
    for(int i = 0; i < msg->data_s; i++)
    {
        buf[3+i] = msg->data[i]; sz++;
    }
    
    buf[sz] = dp_helper_checksum(msg); sz++;
    buf[sz] = DP_PROTO_FRAME_END; sz++;
    
    return sendfunc(buf, sz, usr);
}
