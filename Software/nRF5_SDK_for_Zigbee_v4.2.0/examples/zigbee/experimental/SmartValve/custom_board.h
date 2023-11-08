#include "nrf_gpio.h"

#define PCB_version_3
// Battery measurment input aka ADC input
#define BATTERY_VOLTAGE_PIN               NRF_GPIO_PIN_MAP(0,2)     

// motor driver
#ifdef PCB_version_2
  #define ENABLE_DRIVER_PIN                  NRF_GPIO_PIN_MAP(0,22)
  #define PHASE_DRIVER_PIN                   NRF_GPIO_PIN_MAP(1,0)
  #define NSLEEP_DRIVER_PIN                  NRF_GPIO_PIN_MAP(1,3)
#endif

#ifdef PCB_version_3
  #define ENABLE_DRIVER_PIN                  NRF_GPIO_PIN_MAP(1,1)
  #define PHASE_DRIVER_PIN                   NRF_GPIO_PIN_MAP(1,3)
  #define NSLEEP_DRIVER_PIN                  NRF_GPIO_PIN_MAP(1,0)
#endif

#define IN1_DRIVER_PIN                PHASE_DRIVER_PIN 
#define IN2_DRIVER_PIN                ENABLE_DRIVER_PIN 
// buzzer definition 
#define BUZZER         NRF_GPIO_PIN_MAP(1,10)

// LEDs definitions
#define LEDS_NUMBER    2

#define LED_1          NRF_GPIO_PIN_MAP(1,4)    // LED1
#define LED_2          NRF_GPIO_PIN_MAP(1,6)    // LED2
#define LED_START      LED_1
#define LED_STOP       LED_2

#define LEDS_ACTIVE_STATE 0

#define LEDS_LIST { LED_1, LED_2}

#define LEDS_INV_MASK  LEDS_MASK

#define BSP_LED_0      LED_1
#define BSP_LED_1      LED_2

// buttons definitions
#define BUTTONS_NUMBER 5

#ifdef PCB_version_2
  #define BUTTON_1       NRF_GPIO_PIN_MAP(1,2)    //pairing
  #define BUTTON_2       NRF_GPIO_PIN_MAP(1,10)   //open
  #define BUTTON_3       NRF_GPIO_PIN_MAP(1,13)   //close
  #define BUTTON_4       NRF_GPIO_PIN_MAP(0,3)    //control
  #define BUTTON_PULL    NRF_GPIO_PIN_PULLUP
#endif

#ifdef PCB_version_3
  #define BUTTON_1       NRF_GPIO_PIN_MAP(1,2)    //pairing
  #define BUTTON_2       NRF_GPIO_PIN_MAP(1,13)   //open
  #define BUTTON_3       NRF_GPIO_PIN_MAP(1,15)   //close
  #define BUTTON_4       NRF_GPIO_PIN_MAP(0,3)    //control
  #define BUTTON_5       NRF_GPIO_PIN_MAP(0,5)    //control2

  #define BUTTON_PULL    NRF_GPIO_PIN_NOPULL //NRF_GPIO_PIN_PULLUP //NRF_GPIO_PIN_PULLDOWN
#endif

#define BUTTONS_ACTIVE_STATE 0

#define BUTTONS_LIST { BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4, BUTTON_5 }

#define BUTTON_PAIRING_LIST_NUMBER  0
#define BUTTON_OPEN_LIST_NUMBER     1
#define BUTTON_CLOSE_LIST_NUMBER    2
#define BUTTON_CONTROL_LIST_NUMBER  3
#define BUTTON_CONTROL2_LIST_NUMBER 4

#define BSP_BUTTON_0   BUTTON_1
#define BSP_BUTTON_1   BUTTON_2
#define BSP_BUTTON_2   BUTTON_3
#define BSP_BUTTON_3   BUTTON_4
#define BSP_BUTTON_4   BUTTON_5

#define BUTTON_PAIRING  BUTTON_1
#define BUTTON_OPEN     BUTTON_2
#define BUTTON_CLOSE    BUTTON_3
#define BUTTON_CONTROL  BUTTON_4
#define BUTTON_CONTROL2 BUTTON_5

#define RX_PIN_NUMBER  8
#define TX_PIN_NUMBER  6
#define CTS_PIN_NUMBER 7
#define RTS_PIN_NUMBER 5
#define HWFC           true

/*#define BSP_QSPI_SCK_PIN   19
#define BSP_QSPI_CSN_PIN   17
#define BSP_QSPI_IO0_PIN   20
#define BSP_QSPI_IO1_PIN   21
#define BSP_QSPI_IO2_PIN   22
#define BSP_QSPI_IO3_PIN   23


// serialization APPLICATION board - temp. setup for running serialized MEMU tests
#define SER_APP_RX_PIN              NRF_GPIO_PIN_MAP(1,13)    // UART RX pin number.
#define SER_APP_TX_PIN              NRF_GPIO_PIN_MAP(1,14)    // UART TX pin number.
#define SER_APP_CTS_PIN             NRF_GPIO_PIN_MAP(0,2)     // UART Clear To Send pin number.
#define SER_APP_RTS_PIN             NRF_GPIO_PIN_MAP(1,15)    // UART Request To Send pin number.

#define SER_APP_SPIM0_SCK_PIN       NRF_GPIO_PIN_MAP(0,27)     // SPI clock GPIO pin number.
#define SER_APP_SPIM0_MOSI_PIN      NRF_GPIO_PIN_MAP(0,2)      // SPI Master Out Slave In GPIO pin number
#define SER_APP_SPIM0_MISO_PIN      NRF_GPIO_PIN_MAP(0,26)     // SPI Master In Slave Out GPIO pin number
#define SER_APP_SPIM0_SS_PIN        NRF_GPIO_PIN_MAP(1,13)     // SPI Slave Select GPIO pin number
#define SER_APP_SPIM0_RDY_PIN       NRF_GPIO_PIN_MAP(1,15)     // SPI READY GPIO pin number
#define SER_APP_SPIM0_REQ_PIN       NRF_GPIO_PIN_MAP(1,14)     // SPI REQUEST GPIO pin number

// serialization CONNECTIVITY board
#define SER_CON_RX_PIN              NRF_GPIO_PIN_MAP(1,14)    // UART RX pin number.
#define SER_CON_TX_PIN              NRF_GPIO_PIN_MAP(1,13)    // UART TX pin number.
#define SER_CON_CTS_PIN             NRF_GPIO_PIN_MAP(1,15)    // UART Clear To Send pin number. Not used if HWFC is set to false.
#define SER_CON_RTS_PIN             NRF_GPIO_PIN_MAP(0,2)     // UART Request To Send pin number. Not used if HWFC is set to false.


#define SER_CON_SPIS_SCK_PIN        NRF_GPIO_PIN_MAP(0,27)    // SPI SCK signal.
#define SER_CON_SPIS_MOSI_PIN       NRF_GPIO_PIN_MAP(0,2)     // SPI MOSI signal.
#define SER_CON_SPIS_MISO_PIN       NRF_GPIO_PIN_MAP(0,26)    // SPI MISO signal.
#define SER_CON_SPIS_CSN_PIN        NRF_GPIO_PIN_MAP(1,13)    // SPI CSN signal.
#define SER_CON_SPIS_RDY_PIN        NRF_GPIO_PIN_MAP(1,15)    // SPI READY GPIO pin number.
#define SER_CON_SPIS_REQ_PIN        NRF_GPIO_PIN_MAP(1,14)    // SPI REQUEST GPIO pin number.

#define SER_CONN_CHIP_RESET_PIN     NRF_GPIO_PIN_MAP(1,1)    // Pin used to reset connectivity chip*/



