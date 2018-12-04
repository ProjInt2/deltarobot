#define PART_TM4C123GH6PM 1

#include "libcamargo/common.h"
#include "libcamargo/led.h"
#include "libcamargo/pwm_mock.h"

#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "./libdeltarobot/protocol.h"
#define PROTOCOL_C_NO_INCLUDE
#include "./libdeltarobot-src/protocol.c"

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

float getnan()
{
    union
    {
        int the_int;
        float the_float;
    } nanunion;
    
    nanunion.the_int = 0x7F800001;
    return nanunion.the_float;
}

void my_memcpy(void* dst, void* src, int bytes)
{
    unsigned char * const dstBytes = (unsigned char *) dst;
    unsigned char * const srcBytes = (unsigned char *) src;
    for(int i = 0; i < bytes; i++)
        dstBytes[i] = srcBytes[i];    
}

int is_nan(float arg)
{
    if(arg != arg)  // certainly a nan
        return 1;
    
    return arg == getnan();
}

int pwm_alpha_to_period( float alpha )
{
    return (PWM_PERIOD/2) + (alpha*(PWM_PERIOD/2));
}


// GLOBALS
struct protocol_data pdata;
int curr_status;
unsigned int curr_led;

int pwmEnable;
float pwmValues[3];  // -1 .. +1
float currPos[3];
float goalPos[3];

#define POSITION_TOLERANCE 0.126f // mm

void setup_vars()
{
    // initialize pdata struct
    pdata.m_idx = 0;
    pdata.m_sz = 0;
    
    curr_status = DP_STATUS_IDLE;
    curr_led = LED_G;
    
    pwmEnable = 0;
    
    for(int i = 0; i < 3; i++)
    {
        pwmValues[i] = 0.0f;
        currPos[i] = getnan();
    }
}

int is_position_known()
{
    return (!is_nan(currPos[0])) && (!is_nan(currPos[1])) && (!is_nan(currPos[2])); 
}


// UART DEFINES
#define BAUDRATE  115200
void UART0Handler();
int uart_send(void *buf, int bytes, void *usr);

void setup_uart()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), BAUDRATE, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    
    UARTIntRegister( UART0_BASE, UART0Handler );
    UARTIntEnable( UART0_BASE, UART_INT_RX | UART_INT_TX );
}

int uart_send_func(void *data, int size, void* usr)
{
    (void) usr;
    unsigned char *dataBytes = (unsigned char *) data;
    for(int i = 0; i < size; i++)
        UARTCharPut(UART0_BASE, dataBytes[i]);
    return 0;
}


// STOPS DEFINES

#define STOP_A             GPIO_PIN_0
#define STOP_B             GPIO_PIN_1
#define STOP_C             GPIO_PIN_2
#define STOPS_ALL             (STOP_A | STOP_B | STOP_C | GPIO_PIN_3)

int32_t stops_status = 0;

// returns true if something changed
int read_stops()
{
    int32_t prev = stops_status;
    int32_t stops_status_port = GPIOPinRead(GPIO_PORTE_BASE, STOPS_ALL);


    stops_status =      (stops_status_port & STOP_A? (1 << 0) : 0) |
                        (stops_status_port & STOP_B? (1 << 1) : 0) |
                        (stops_status_port & STOP_C? (1 << 2) : 0);

    return prev ^ stops_status;
}

void setup_stops()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIODirModeSet(GPIO_PORTE_BASE, STOPS_ALL, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTE_BASE, STOPS_ALL, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    read_stops();
}


// ENCODER DEFINES

#define ENC_A             GPIO_PIN_3
#define ENC_B             GPIO_PIN_3
#define ENC_C             GPIO_PIN_2
#define ENC_ALL           (ENC_A | ENC_B | ENC_C)

int32_t encoders_status = 0;

// returns true if something changed
int read_encoders()
{
    int32_t prev = encoders_status;
    int32_t encoders_status_portd = GPIOPinRead(GPIO_PORTD_BASE, ENC_A | ENC_C);
    int32_t encoders_status_portb = GPIOPinRead(GPIO_PORTB_BASE, ENC_B);

    encoders_status =   (encoders_status_portd & ENC_A? (1 << 0) : 0) |
                        (encoders_status_portb & ENC_B? (1 << 1) : 0) |
                        (encoders_status_portd & ENC_C? (1 << 2) : 0);

    return prev ^ encoders_status; // if something changed
}

void setup_encoders()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, ENC_A | ENC_C);
    GPIODirModeSet(GPIO_PORTD_BASE, ENC_A | ENC_C, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTD_BASE, ENC_A | ENC_C, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);


    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, ENC_B);
    GPIODirModeSet(GPIO_PORTB_BASE, ENC_B, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTB_BASE, ENC_B, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    read_encoders();
}



// FUNCTIONS

void send_ack(unsigned int opcode)
{   
    struct protocol_msg msg;
    msg.opcode = DP_OPCODE_ACK;
    msg.data_s = 1;
    msg.data[0] = opcode;
    dp_send(&msg, &uart_send_func, 0);
}

void send_nack(unsigned int opcode)
{   
    struct protocol_msg msg;
    msg.opcode = DP_OPCODE_NACK;
    msg.data_s = 1;
    msg.data[0] = opcode;
    dp_send(&msg, &uart_send_func, 0);
}

void send_status()
{
    struct protocol_msg_status_report rep;
    rep.DA = currPos[0];
    rep.DB = currPos[1];
    rep.DC = currPos[2];
    rep.STAT = ((~stops_status) << 2) | (curr_status & 0x03);
    
    struct protocol_msg msg;
    msg.opcode = DP_OPCODE_STATUS_REPL;
    msg.data_s = sizeof(rep);
    my_memcpy(msg.data, &rep, sizeof(rep));
    
    dp_send(&msg, &uart_send_func, 0);
}

void process_messages()
{
    while(UARTCharsAvail(UART0_BASE)) 
    {
        uint8_t c = UARTCharGet(UART0_BASE);
        
        dp_process(&pdata, &c, 1);
    }
    
    struct protocol_msg pmsg;
    struct pos_s_t { float DA, DB, DC; } *pos;
    if(!dp_recv(&pdata, &pmsg))
    {
        // received a message
        
        switch(pmsg.opcode)
        {
            case DP_OPCODE_ACK:
                cLedSet(LED_B | LED_G);
                break;
            case DP_OPCODE_NACK:
                cLedSet(LED_R);
                break;
            case DP_OPCODE_HOMING:
                {
                    // host wants us to go home
                    if(curr_status != DP_STATUS_ERROR)
                    {
                        curr_status = DP_STATUS_HOMING;
                        curr_led = LED_G | LED_B;
                        send_ack(DP_OPCODE_HOMING);
                    }
                    else send_nack(DP_OPCODE_HOMING);
                    
                    send_status();
                }
                break;
            case DP_OPCODE_STATUS:
                send_status();
                break;

            case DP_OPCODE_MOVE:
                {
                    if(!is_position_known())
                    {
                        send_nack(DP_OPCODE_MOVE);
                        break;
                    }   
                    
                    // host wants us to go home
                    if(curr_status != DP_STATUS_ERROR && pmsg.data_s == sizeof(struct pos_s_t))
                    {
                        pos = (struct pos_s_t*) pmsg.data;
                        if(is_nan(pos->DA) || is_nan(pos->DB) || is_nan(pos->DC) )
                        {
                            send_nack(DP_OPCODE_MOVE);
                        }
                        else
                        {
                            goalPos[0] = pos->DA;
                            goalPos[1] = pos->DB;
                            goalPos[2] = pos->DC;
                            curr_status = DP_STATUS_MOVING;
                            send_ack(DP_OPCODE_MOVE);
                        }
                    }
                    else send_nack(DP_OPCODE_MOVE);
                    
                    send_status();
                }
                break;
            case DP_OPCODE_CANCEL:
                curr_led = (LED_R | LED_G);
                if(curr_status != DP_STATUS_ERROR)
                {
                    curr_status = DP_STATUS_IDLE;
                    send_ack(DP_OPCODE_CANCEL);
                }
                else send_nack(DP_OPCODE_CANCEL);
                send_status();
                break;
            default:
                cLedSet(0); // this might not go through if not a valid opcode
                break;
        }
    }
}


void move_step()
{
    for( int i = 0; i < 3; i++)
    {
        float diff = goalPos[i] - currPos[i];
        float diffAbs = diff < 0? -diff : diff;

        if(diffAbs > POSITION_TOLERANCE)
        {
            // difference in position, use pwm
            pwmValues[i] = diff > 0? 1.0f : -1.0f;

            if(pwmValues[i] > 0)
            {
                // going forward
                pwmEnable |= (1 << i);
            }
            else
            {
                // going back
                if(stops_status & (1 << i))
                    pwmEnable |= (1 << i);
                else
                    pwmEnable &= ~(1 << i); // disable going back when stop is triggered
            }
        }
        else
        {
            pwmEnable &= ~(1 << i);
            pwmValues[i] = 0.0f;
        }
    }

    if(!(pwmEnable & 0x07))
    {
        curr_status = DP_STATUS_IDLE;
        send_status();
    }
}

void update_pwm()
{
    switch(curr_status)
    {
        case DP_STATUS_HOMING:
            if(!stops_status)
            {
                pwmEnable = 0x00;
                curr_status = DP_STATUS_IDLE;
                currPos[0] = currPos[1] = currPos[2] = 0.0f;
                send_status();
            }
            else
            {
                pwmEnable = stops_status; // bit in stops_status is zero if pressed
                pwmValues[0] = -1.0f;
                pwmValues[1] = -1.0f;
                pwmValues[2] = -1.0f;
            }
            break;
            send_status();

        case DP_STATUS_MOVING:
            move_step();
            break;

        default:
            pwmEnable = 0;
            break;
            
    }

    cPWMSet( pwmEnable, pwm_alpha_to_period(pwmValues[0]),
             pwm_alpha_to_period(pwmValues[1]), pwm_alpha_to_period(pwmValues[2]) );

    // home in, for testing
    //cPWMSet( stops_status, 0, 0, 0 ); // set pwm output values

    // advance, for testing
    //cPWMSet( 0xff, PWM_PERIOD, PWM_PERIOD, PWM_PERIOD ); // set pwm output values
}


// MAIN - SETUP

inline void setup()
{
    setup_vars();
    
    // INIT PWM
    if(cPWMInit())
        curr_status = DP_STATUS_ERROR;

    setup_uart();    
    setup_stops();
    setup_encoders();
}


// MAIN - LOOP

void loop()
{
    SysCtlDelay( 20000 ); // control delay
    
    read_stops();
    
    process_messages();
    update_pwm();   

    // heartbeat
    static int counter = 0;
    counter ++;
    cLedSet( (counter/10)%2? curr_led : 0 );
    

    /* SIMULATE DISTANCE
    if((!(counter % 500)) && (curr_status == DP_STATUS_HOMING || curr_status == DP_STATUS_MOVING))
    {
        if(pwmEnable & 1)
            if(pwmValues[0] < 0)
                currPos[0] -= 2.5f;
            if(pwmValues[0] > 0)
                currPos[0] += 2.5f;

        if(!(counter % 1000))
        {
            for( int i = 1; i < 3; i++)
            {
                if(pwmEnable & (1 << i))
                    if(pwmValues[i] < 0)
                        currPos[i] -= 2.5f;
                    if(pwmValues[i] > 0)
                        currPos[i] += 2.5f;
            }
        }


        send_status();
    }*/

    static uint32_t prev_enc_status = 0;
    static int enc_ticks[3] = {0,0,0};
    static uint32_t enc_changes = 0;

    if(curr_status == DP_STATUS_MOVING)
    {

        for(int i = 0; i < 3; i++)
        {
            enc_ticks[i] ++;
        }

        if(read_encoders())  // changes happened within the encoders
        {
            uint32_t enc_changes = prev_enc_status ^ encoders_status;

            for(int i = 0; i < 3; i++)
            {
                if(enc_changes & (1 << i))
                {
                    // encoder changed state, see if enough time to consider a genuine change and not just noise
                    if(enc_ticks[i] > 45)
                    {
                        if(pwmEnable & (1 << i))
                        {
                            // currPos[i] = enc_ticks[i];  -- for testing

                            if(pwmValues[i] < 0)
                                currPos[i] -= 2.5f;
                            if(pwmValues[i] > 0)
                                currPos[i] += 2.5f;
                        }

                        send_status();
                    }
                    enc_ticks[i] = 0; // reset counter on change
                }
            }
        }

        curr_led = ((encoders_status&1?LED_R:0)|(encoders_status&1?LED_G:0)|(encoders_status&1?LED_B:0));
    }
    else
    {
        enc_ticks[0] = enc_ticks[1] = enc_ticks[2] = 0;
    }

    prev_enc_status = encoders_status;

}

int main(void)
{

    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    cLedInit();
    cLedSet(LED_R);

    setup();

    cLedSet(LED_G);
    while(1)
    {
        loop();
    }
}

void UART0Handler()
{
    
    uint32_t status = UARTIntStatus(UART0_BASE, true); 
    
    UARTIntClear( UART0_BASE, status );
    
    if(status & UART_INT_RX)
    {
        // do nothing here too
        
    }
    
    if(status & UART_INT_TX)
    {
        // do nothing here
    }
}

int uart_send(void *buf, int bytes, void *usr)
{
    char *charbuf = (char*) buf;
    for(int i = 0; i < bytes; i++)
    {
        UARTCharPut( UART0_BASE, charbuf[i] );       
    }
    
    return 0;
}


