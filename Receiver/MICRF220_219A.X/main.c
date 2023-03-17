/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using MPLAB(c) Code Configurator

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  MPLAB(c) Code Configurator Plugin - v3.66, MC & Peripherals v1.75
        Device            :  PIC18F25K40
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.05
        MPLAB             :  MPLAB X 5.10
*/

/*
    (c) 2019 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#include "mcc_generated_files/mcc.h"
#include <string.h>
#include <stdio.h>
#include "lcd.h"
#include "receiver.h"
#include "dvr_micrf220_219a.h"
#include "dvr_adc.h"

/***********************************************************************************************************************
 *
 * Function Name: main
 *
 * Purpose: Initialize the process and firmware modules, run the main loop
 *
 * Arguments: None
 *
 * Returns: None
 *
 * Side Effects: N/A
 *
 * Reentrant Code: No
 *
 **********************************************************************************************************************/
void main(void)
{
    rxDataPacket_t rxPacket;                        // Populated by RX_process() if data is ready.
    engData_t      engData;                         // Contains engineering data from RX_getEngData() module.
    uint32_t       txCnt;
    
    SYSTEM_Initialize();                            // Initialize the device
    spi1_master_open(SPI1_CUSTOM0);
    DVR_ADC_init();
    LCD_CHIP_SELECT_SetDigitalOutput();             // Configure the LCD chip select output pin
    LcdInitialization();                            // Initialize the LCD module
    printf(" RF Demo, Rev 4");                       // Print the version of this demo
    LcdGoto(1, 0);                                  // Set the cursor up on the next line
    printf(" MICRF 220/219A");                      // Print the radio being used
    __delay_ms(2000);                               // Display the demo info for 2 seconds.
    RX_init();                                      // Initialize the RX receiver
    INTERRUPT_GlobalInterruptHighEnable();          // Enable the Global Interrupts (used for the bit-slicer)
    INTERRUPT_GlobalInterruptLowEnable();           // Enables the low interrupts (used for the ADC)
    
    while (1)                                       // Start the main loop
    {
        DVR_ADC_processConversion();                // Process ADC data
        if (RX_process(&rxPacket))                  // Check if there is an RF packet received
        {
#if RX_ENG_DATA_ON == 1                    
            RX_getEngData(&engData);                // Get the engineering data
#endif
            LcdClear();                             // Clear the LCD information
            LcdGoto(0, 0);                          // Set cursor up on the 1st line in the 1st position
            if (S1_GetValue())                      // If S1 is pushed, display the engineering data
            {
#if MICRF_ENABLE_RSSI == 1
                printf("RSSI: %d/%d", rxPacket.msgRssi, rxPacket.noiseRssi);    // Display the RSSI values
#endif
                LcdGoto(1, 0);                                          // Set the cursor up on the next line
                (void)memcpy(&txCnt, &rxPacket.data[0], sizeof(txCnt));
                printf("T:%ld R:%ld",txCnt, engData.validPackets);      // Display only the number of packets received
            }
            else
            {
#if RX_ENG_DATA_ON == 1                
                printf("Err: %d, CRC: %d", engData.cntFailure, engData.crcFailures);
                LcdGoto(1, 0);                                          // Set the cursor up on the next line
                printf("P: %d, Buf: %d", engData.protocolFailures, engData.bufferOverflow);
#endif
            }
        }
    }
}

/***********************************************************************************************************************
 *
 * Function Name: putch
 *
 * Purpose: Used by printf to display the data
 *
 * Arguments: char myChar
 *
 * Returns: None
 *
 * Side Effects: The display data is altered
 *
 * Reentrant Code: yes
 *
 **********************************************************************************************************************/
void putch(char myChar)
{
    WriteLcdByte(myChar);
}

/**
 End of File
*/