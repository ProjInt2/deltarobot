#define PART_TM4C123GH6PM 1

#include "camargo/common.h"
#include "camargo/led.h"
#include "camargo/pwm.h"

#include "driverlib/pin_map.h"
#include "driverlib/uart.h"

extern "C++"
{
#include "./libdeltarobot/protocol.h"
}

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

#define BAUDRATE  115200
void UART0Handler();

inline void setup()
{
    // INIT PWM
    if(cPWMInit())
        for(;;); // lockup here
    
    // INIT UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), BAUDRATE, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    
    UARTIntRegister( UART0_BASE, UART0Handler );
    UARTIntEnable( UART0_BASE, UART_INT_RX | UART_INT_RT );
    
    UARTCharPutNonBlocking(UART0_BASE, 'I');   
}

static int pwidth = 0;
void loop()
{
    SysCtlDelay( 20000 ); // control delay
    
    
    if(pwidth < 100000 - 1)
        pwidth ++;
    else
        pwidth = 0;
    
    cPWMSet( pwidth, pwidth/2, pwidth/3 ); // set pwm output values 
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

