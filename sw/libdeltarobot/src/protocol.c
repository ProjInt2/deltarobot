#ifndef PROTOCOL_C_NO_INCLUDE
#include "libdeltarobot/protocol.h"
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
        data->m_sz += bytes;
    }
    
    return ret;
}

#define ST_SEARCH_FRAME 0
#define ST_READ_OPCODE 0
#define ST_READ_SIZE 0
#define ST_READ_DATA 0
#define ST_CHECKSUM 0
#define ST_FINALIZE 0

int dp_recv( struct protocol_data *data, void* buf, int bytes, struct protocol_msg *msg )
{
    int state = ST_SEARCH_FRAME;
    
    for(int i = data->m_idx; i < (data->m_idx + data->m_sz); i++)
    {
        char c = dp_helper_modread(data, i);
        
        switch(state)
        {
            case ST_SEARCH_FRAME:
                if(c == DP_PROTO_FRAME_START)
                break;
                
            case ST_READ_OPCODE:
                
                break;
            
            case ST_READ_SIZE:
                
                break;
            
            case ST_READ_DATA:
                
                break;
            
            case ST_CHECKSUM:
                
                break;
            
            case ST_FINALIZE:
                
                break;
        }
    }
}

int dp_send( struct protocol_msg *msg, send_func_t sendfunc )
{
    
}
