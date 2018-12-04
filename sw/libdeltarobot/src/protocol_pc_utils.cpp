
#include "libdeltarobot/protocol_pc_utils.h"
#include <sstream>

namespace deltarobot {

    inline static int dp_helper_opcode_valid( char opcode )
    {
        static const char *opcodes = DP_PROTO_ALL_OPCODES;

        while(*opcodes)
        {
            if((*opcodes) == opcode)
                return 1;
            opcodes ++;
        }

        return 0;
    }

    const char * protocol_opcode_to_str(unsigned char opcode)
    {

        switch (opcode)
        {
            case DP_OPCODE_ACK:
                return "ACK";
            case DP_OPCODE_NACK:
                return "NACK";
            case DP_OPCODE_HOMING:
                return "HOMING!";
            case DP_OPCODE_STATUS:
                return "STATUS?";
            case DP_OPCODE_MOVE:
                return "MOVE!";
            case DP_OPCODE_CANCEL:
                return "CANCEL!";
            case DP_OPCODE_HOMING_REPL:
                return "HOMING_REPLY";
            case DP_OPCODE_STATUS_REPL:
                return "STATUS_REPLY";
            case DP_OPCODE_MOVE_REPL:
                return "MOVE_REPLY";
            case DP_OPCODE_GENERAL_ERR:
                return "GENERAL_ERROR";
            default:
                return "UNKNOWN_OPCODE";
        }
    }

    const char * protocol_status_to_str(int statusbits )
    {

        switch (statusbits)
        {
            case DP_BIT_STAT_STATUS_IDLE:
                return "IDLE";
            case DP_BIT_STAT_STATUS_HOMING:
                return "HOMING";
            case DP_BIT_STAT_STATUS_MOVING:
                return "MOVING";
            case DP_BIT_STAT_STATUS_ERROR:
                return "ERROR";
            default:
                return "UNKNOWN_STATUS";
        }
    }


    std::string protocol_msg_to_str( const protocol_msg * msg )
    {
        std::stringstream ss;
        ss << protocol_opcode_to_str(msg->opcode);

        protocol_msg_position *pos;
        protocol_msg_status_report *status;

        switch( msg->opcode )
        {

            case DP_OPCODE_ACK:
                ss << ": (" <<  protocol_opcode_to_str(msg->data[0]) << ")";
                break;


            case DP_OPCODE_STATUS_REPL:
                status = (protocol_msg_status_report*) msg->data;
                ss << ": ";
                ss << protocol_status_to_str( status->STAT & DP_BIT_STAT_STATUS_BITS );
                ss << "  DA=" << status->DA;
                ss << "  DB=" << status->DB;
                ss << "  DC=" << status->DC;
                if(status->STAT & DP_BIT_STAT_STOP_A_BIT)
                    ss << " STP_A";
                if(status->STAT & DP_BIT_STAT_STOP_B_BIT)
                    ss << " STP_B";
                if(status->STAT & DP_BIT_STAT_STOP_C_BIT)
                    ss << " STP_C";
                break;

            case DP_OPCODE_MOVE:
                pos = (protocol_msg_position*) msg->data;
                ss << ":";
                ss << "  DA=" << pos->DA;
                ss << "  DB=" << pos->DB;
                ss << "  DC=" << pos->DC;
                break;
        }

        return ss.str();
    }


}
