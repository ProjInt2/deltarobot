#define PART_TM4C1230C3PM 1

#include "camargo/common.h"
#include "camargo/led.h"

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

inline void setup()
{
  
}

inline void loop()
{
  
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
