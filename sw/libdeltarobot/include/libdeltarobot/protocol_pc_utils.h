#pragma once

#include "./protocol.h"

#include <string>

namespace deltarobot
{

    const char * protocol_opcode_to_str( unsigned char opcode );

    std::string protocol_msg_to_str( const protocol_msg * msg );

};
