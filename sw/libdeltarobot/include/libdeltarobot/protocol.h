#pragma once

/**
 * @file protocol.h
 * @brief Header library for gerenating and parsing deltarobot protocol messages
 * @author "Lucas Camargo" <camargo@lisha.ufsc.br>
 */
#ifdef __cplusplus
extern "C"
{
#endif
    
#define DP_BUFSIZE 512
    
#define DP_RC_SUCCESS           0
#define DP_RC_ERROR_GENERIC     1
#define DP_RC_NO_FRAME          2
#define DP_RC_OVERFLOW          3
    
#define DP_PROTO_FRAME_START '$'
    
    struct protocol_msg
    {
        unsigned char opcode;
        int data_s;
        char data[256];
        int checksum_ok;
    };
    
    struct protocol_data
    {
        char m_buf[DP_BUFSIZE]; // buffer being processed
        int m_idx; // current index
        int m_sz;  // current size  
    };
    
    // byte-writing callback
    typedef void (*send_func_t)(void *buf, int bytes, void *usr);
    
    /** 
     * @brief feed data into the buffer
     * @return DP_RC_SUCCESS
     */
    int dp_process( struct protocol_data *data, void *buf, int bytes );
    
    /**
     * @brief analizes protocol data to look for a message
     * @return DP_RC_SUCCESS or DP_RC_NO_FRAME
     */
    int dp_recv( struct protocol_data *data, void* buf, int bytes, protocol_msg *msg );
        
    /**
     * @brief sends a message using the given send function
     */
    int dp_send( struct protocol_msg *msg, send_func_t sendfunc );
    
    
#ifdef __cplusplus
};
#endif
