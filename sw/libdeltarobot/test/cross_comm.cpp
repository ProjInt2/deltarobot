/**
 * @file cross_comm.cpp
 * @brief Test protocol message sending and receiving
 * @author Lucas Camargo <camargo@lisha.ufsc.br>
 */

#include <iostream>
#include <cstring>

// compile protocol support code together
extern "C"{
#include "../include/libdeltarobot/protocol.h"
#define PROTOCOL_C_NO_INCLUDE
#include "../src/protocol.c"
}

protocol_data data_a, data_b;


int send_to_another(void * data, int bytes, void *dest_p_data)
{
    std::cout << "sending " << bytes << " bytes of data into " << dest_p_data << std::endl;
    
    protocol_data *dest = (protocol_data*) dest_p_data;
    int ret = dp_process(dest, data, bytes);
    
    std::cout << "sent" << std::endl;
    return ret;
}

void dump_message(protocol_msg* message)
{
    std::cout << "MESSAGE DUMP" << std::endl;
    std::cout << "\tOpcode: 0x" << std::hex << ((int)(unsigned char)message->opcode) << "'" << 
    message->opcode << "'" << std::endl;
    std::cout << "\tData size: " << message->data_s << std::endl;
    if(message->data_s)
    {
        std::cout << "\tData bytes: ";
        for(int b = 0; b < message->data_s; b++)
        {
            std::cout << " 0x" << std::hex << ((int)(unsigned char)message->data[b]);
        }
        std::cout << std::endl;
    }
}


void dump_protocol_data(protocol_data* data)
{
    std::cout << "PROTOCOL DATA DUMP" << std::endl;
    std::cout << "\tData size: " << data->m_sz << std::endl;
    std::cout << "\tCurrent index: " << data->m_idx << std::endl;
    std::cout << "\tBuffer: ";
    for(int b = 0; b < sizeof(data->m_buf); b++)
    {
        std::cout << " 0x" << std::hex << ((int)(unsigned char)data->m_buf[b]);
    }
    std::cout << std::endl;
}

int main(int argc, char ** argv)
{    
    bzero(&data_a, sizeof(protocol_data));
    bzero(&data_b, sizeof(protocol_data));
    
    protocol_msg msg;
    int ret;
    
    for(int i = 0; i < 200; i++)
    {

        std::cout << "\n\n\nTEST 1: send and receive a simple message with one data byte\n";

        // send a message to b
        std::cout << "sending ack to b" << std::endl;
        bzero(&msg, sizeof(protocol_msg));
        msg.opcode = DP_OPCODE_ACK;
        msg.data_s = 1;
        msg.data[0] = 'L'; // arbitrary
        ret = dp_send(&msg, &send_to_another, &data_b);

        std::cout << "returned " << ret << std::endl;
        if(ret)
            return -1;

        std::cout << "dumping data in b" << std::endl;
        dump_protocol_data(&data_b);

        // process message in b
        std::cout << "processing message in b" << std::endl;
        ret = dp_recv(&data_b, &msg);

        std::cout << "returned " << ret << std::endl;
        if(ret)
            return ret;

        dump_message(&msg);
        if((msg.opcode != DP_OPCODE_ACK) ||
        (msg.data_s != 1) ||
        (msg.data[0] != 'L'))
        {
            std::cout << "message error!" << std::endl;
            return -2;
        }


        // BEGIN part 2
        std::cout << "\n\n\nTEST 2: send three messages at once, and receive all of them\n";

        ret = dp_send(&msg, &send_to_another, &data_a);
        if(!ret) ret = dp_send(&msg, &send_to_another, &data_a);
        if(!ret) ret = dp_send(&msg, &send_to_another, &data_a);

        std::cout << "returned " << ret << std::endl;
        if(ret)
            return ret;

    #define CHECK_MSG \
        if((msg.opcode != DP_OPCODE_ACK) || \
            (msg.data_s != 1) || \
            (msg.data[0] != 'L')) \
        { \
        ret = -99;  \
        }
        std::cout << "receiving and checking the three messages" << ret << std::endl;

        ret = dp_recv(&data_a, &msg);
        CHECK_MSG
        if(!ret) ret = dp_recv(&data_a, &msg);
        CHECK_MSG
        if(!ret) ret = dp_recv(&data_a, &msg);
        CHECK_MSG

        std::cout << "returned " << ret << std::endl;
        if(ret)
            return ret;

        std::cout << "trying to receive another" << std::endl;
        if(!dp_recv(&data_a, &msg))
        {
            std::cout << "Received! But there shouldn't be anything left in the buffer!" << std::endl;
            return -98;
        }
    }

    std::cout << "all tests passed! :)" << std::endl;

}
