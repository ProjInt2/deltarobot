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
    
#define DP_BUFSIZE 128
    
#define DP_RC_SUCCESS           0
#define DP_RC_ERROR_GENERIC     1
#define DP_RC_NO_FRAME          2
#define DP_RC_OVERFLOW          3
#define DP_RC_GARBAGE           4
    
#define DP_PROTO_FRAME_START '$'
#define DP_PROTO_FRAME_END   ':'
#define DP_PROTO_ALL_OPCODES "ANHSMChsme"
    
#define DP_OPCODE_ACK   ((unsigned char)'A')
#define DP_OPCODE_NACK  ((unsigned char)'N')
    
#define DP_OPCODE_HOMING  ((unsigned char)'H')
#define DP_OPCODE_STATUS  ((unsigned char)'S')
#define DP_OPCODE_MOVE  ((unsigned char)'M')
#define DP_OPCODE_CANCEL  ((unsigned char)'C')
    
#define DP_OPCODE_HOMING_REPL  ((unsigned char)'h')
#define DP_OPCODE_STATUS_REPL  ((unsigned char)'s')
#define DP_OPCODE_MOVE_REPL  ((unsigned char)'m')
#define DP_OPCODE_GENERAL_ERR  ((unsigned char)'e')
    
    
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
    
// used in the fw
#define DP_STATUS_IDLE   0
#define DP_STATUS_HOMING 1
#define DP_STATUS_MOVING 2
#define DP_STATUS_ERROR  3

// for the stat field on the status message
#define DP_BIT_STAT_STATUS_BITS 0x03
#define DP_BIT_STAT_STATUS_IDLE 0x00
#define DP_BIT_STAT_STATUS_HOMING 0x01
#define DP_BIT_STAT_STATUS_MOVING 0x02
#define DP_BIT_STAT_STATUS_ERROR  0x03
#define DP_BIT_STAT_STOP_A_BIT  (1<<2)
#define DP_BIT_STAT_STOP_B_BIT  (1<<3)
#define DP_BIT_STAT_STOP_C_BIT  (1<<4)
#define DP_BIT_STAT_STOP_PRESSED 1
#define DP_BIT_STAT_STOP_CLEAR   0

#ifdef __cplusplus
};
#endif
