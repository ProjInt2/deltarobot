#pragma once

/**
 * @file protocol.h
 * @brief Library for gerenating and parsing deltarobot protocol messages.
 *          Implemented in pure C to be usable both in the PC and the µC
 * @author "Lucas Camargo" <camargo@lisha.ufsc.br>
 */

#ifdef __cplusplus
extern "C"
{
#endif
    
#define DP_BUFSIZE 256
    
#define DP_RC_SUCCESS           0
#define DP_RC_ERROR_GENERIC     1
#define DP_RC_NO_FRAME          2
#define DP_RC_OVERFLOW          3
#define DP_RC_GARBAGE           4
    
#define DP_PROTO_FRAME_START '$'
#define DP_PROTO_FRAME_END   ':'
#define DP_PROTO_ALL_OPCODES "ANHSMChsme"
    
#define DP_OPCODE_ACK   'A'
#define DP_OPCODE_NACK  'N'
    
#define DP_OPCODE_HOMING  'H'
#define DP_OPCODE_STATUS  'S'
#define DP_OPCODE_MOVE  'M'
#define DP_OPCODE_CANCEL  'C'
    
#define DP_OPCODE_HOMING_REPL  'h'
#define DP_OPCODE_STATUS_REPL  's'
#define DP_OPCODE_MOVE_REPL  'm'
#define DP_OPCODE_GENERAL_ERR  'e'
    
    
#ifndef DP_DATA_MAX_SIZE
#define DP_DATA_MAX_SIZE 32
#endif
    
    struct protocol_msg
    {
        unsigned char opcode;
        int data_s;
        char data[DP_DATA_MAX_SIZE];
        int checksum_ok;
    };
    
    struct protocol_data
    {
        char m_buf[DP_BUFSIZE]; // buffer being processed
        int m_idx; // current index
        int m_sz;  // current size  
    };
    
    // byte-writing callback
    typedef int (*send_func_t)(void *buf, int bytes, void *usr);
    
    /** 
     * @brief feed data into the buffer
     * @return DP_RC_SUCCESS
     */
    int dp_process( struct protocol_data *data, void *buf, int bytes );
    
    /**
     * @brief analyzes protocol data to look for a message
     * @return DP_RC_SUCCESS or DP_RC_NO_FRAME
     */
    int dp_recv( struct protocol_data *data, struct protocol_msg *msg );
        
    /**
     * @brief sends a message using the given send function
     */
    int dp_send( struct protocol_msg *msg, send_func_t sendfunc, void *usr );
    
    
    
//// CONVENIENCE STRUCTURES
    
    struct protocol_msg_status_report
    {
        float DA;
        float DB;
        float DC;
        unsigned char STAT;
    }; // __attribute__((packed)); not needed
    
    struct protocol_msg_position
    {
        float DA;
        float DB;
        float DC;
    }; // __attribute__((packed)); not needed
    
#define DP_STATUS_IDLE   0
#define DP_STATUS_HOMING 1
#define DP_STATUS_MOVING 2
#define DP_STATUS_ERROR  3
    
#ifdef __cplusplus
};
#endif
