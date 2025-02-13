// EE478 Lab2
// Measurement Unit

/* Compile options:  -ml (Large code model) */

// PIC18F25K22 Configuration Bit Settings
// CONFIG1H
//pragma config FOSC = ECHP
#pragma config FOSC = INTIO7   // Oscillator Selection bits (EC oscillator (high power, >16 MHz))
#pragma config PLLCFG = ON    // 4X PLL Enable (Oscillator used directly)
#pragma config PRICLKEN = ON    // Primary clock enable bit (Primary clock enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switch-over bit (Oscillator Switch-over mode disabled)

// CONFIG2L
#pragma config PWRTEN = OFF
// Power-up Timer Enable bit (Power up timer disabled)
#pragma config BOREN = OFF  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 190       // Brown Out Reset Voltage bits (VBOR set to 1.90 V nominal)

// CONFIG2H
#pragma config WDTEN = OFF      // Watchdog Timer Enable bits (Watch dog timer is always disabled. SWDTEN has no effect.)
#pragma config WDTPS = 32768    // Watchdog Timer Post-scale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC1  // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF      // PORTB A/D Enable bit (PORTB<5:0> pins are configured as analog input channels on Reset)
#pragma config CCP3MX = PORTB5  // P3A/CCP3 Mux bit (P3A/CCP3 input/output is multiplexed with RB5)
#pragma config HFOFST = ON      // HFINTOSC Fast Start-up (HFINTOSC output and ready status are not delayed by the oscillator stable status)
#pragma config T3CMX = PORTC0   // Timer3 Clock input mux bit (T3CKI is on RC0)
#pragma config P2BMX = PORTB5   // ECCP2 B output mux bit (P2B is on RB5)
#pragma config MCLRE = EXTMCLR  // MCLR Pin Enable bit (MCLR pin enabled, RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled if MCLRE is also 1)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection Block 0 (Block 0 (000800-001FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection Block 1 (Block 1 (002000-003FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection Block 2 (Block 2 (004000-005FFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection Block 3 (Block 3 (006000-007FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection Block 0 (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection Block 1 (Block 1 (002000-003FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection Block 2 (Block 2 (004000-005FFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection Block 3 (Block 3 (006000-007FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection Block 0 (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection Block 1 (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection Block 2 (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection Block 3 (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

// ISR to reset other parts
#include "routines.h"

// allocate memory for sensor data
sensor_data_struct sensor_data;
sensor_data_struct* sensor_data_ptr;
sensor_result_struct sensor_result;
sensor_result_struct* sensor_result_ptr;

// global variables for scheduler
unsigned int curr_channel;
unsigned int system_status;
unsigned int sampling_flag;
unsigned int error;

// global variables for UART
volatile char RX_buffer[50];
volatile int remaining_buffer_size;
volatile unsigned int RX_buffer_full;
volatile unsigned char byte_command;
unsigned int report_type;
unsigned int target_ID;
char* RX_buffer_ptr;

// global variables misc sensors
unsigned int freqCounter;
bmp085_t bmp; // calibration struct for the pressure sensor


// global variables for I2C
char I2C_TX_buffer[8];
char* I2C_TX_buffer_ptr;
char I2C_RX_buffer[40];
char* I2C_RX_buffer_ptr;

unsigned int report_flag;

void system_init(void);

void main(void)
{
   //count = 0;
    //system init
   system_init();	
   // redirect output stream to _H_USER
   // initialize peripherals

   
   // initialize global variables   
   done = 0;
   curr_channel = 0;
   RX_buffer_ptr = &(RX_buffer[0]);
   
   report_flag = 0;
   sampling_flag = 0;
   byte_command = '0';

   sensor_data_ptr->accel_X = 0;
   sensor_data_ptr->accel_Y = 0;
   sensor_data_ptr->accel_Z = 0;

   sensor_data_ptr->yaw = 0;
   sensor_data_ptr->pitch = 0;
   sensor_data_ptr->roll = 0;


   I2C_TX_buffer_ptr = &(I2C_TX_buffer[0]);
   I2C_RX_buffer_ptr = &(I2C_RX_buffer[0]);

   sensor_data_ptr = &sensor_data;
   sensor_data_ptr->angular_velocity_X = 0;
   sensor_data_ptr->angular_velocity_Y = 0;
   sensor_data_ptr->angular_velocity_Z = 0;

   sensor_result_ptr = &sensor_result;
   sensor_result_ptr->altitude = 0;
   sensor_result_ptr->pitch = 0;
   sensor_result_ptr->roll =0;
   sensor_result_ptr->yaw =0;
   
   error = NO_ERROR;

   // start measurements
   start_scheduler();
}

// power up initialization 
void system_init(void) 
{   
   // system clk set-up
   OSCCON  =    0b11111000;
   OSCCON2 =    0b00000000;
   //low-frequency internal osc sourced from LFINTOSC
   OSCTUNEbits.INTSRC = 0;
   //disable PLL
   OSCTUNEbits.PLLEN = 1;
   // IO ports initialization
   // see document for exact assignments
   TRISA =     0b11000111;
   TRISB =     0xFF;
   TRISC =     0b11011000;
   ANSELA =    0b00000000;
   ANSELB =    0x00;
   ANSELC =    0x00;
   stdout = _H_USER;
   WPUB   =    0x00;
   IOCB   =    0x00;
   PORTAbits.RA5 = 1;
   

    //I2C for sensor card to flight control pic
    TRISBbits.RB2 = 1;
    TRISBbits.RB1 = 1;
    ANSELBbits.ANSB1 = 0; // digital output
    ANSELBbits.ANSB2 = 0;

   //uart_init();
   timer_init();
   I2C_slave_init();
   I2C_master_init();
   interrupt_init();
   sensors_init();

}

void bus_reset(void)
{
    TRISB = 0x00;
    PORTB = 0x55;
}

unsigned int bytes2int(unsigned char upper, unsigned char lower)
{
    unsigned int result;
    result = upper;
    result = result << 8 | lower;
    return result;
}


void sensors_init(void)
{
    //bmp085_init(&bmp);
    gyro_init();
    magneto_init();
    acceler_init();
}




 