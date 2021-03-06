/*----------------------------------------------------------------------------------
 * File:          Lab8_D1.c
 *
 * Function:      Echo a received character, using polling.
 *
 * Description:   This program echos the character received from UART back to UART.
 *                Toggle LED1 with every received character.
 *                Baud rate: low-frequency (UCOS16=0);
 *                1048576/115200 = ~9.1 (0x0009|0x01)
 *
 * Clocks:        ACLK = LFXT1 = 32768Hz, MCLK = SMCLK = default DCO
 *
 * Board:         MSP-EXP430F5529
 *
 * Instructions: Set the following parameters in putty
 * Port: COMx
 * Baud rate: 115200
 * Data bits: 8
 * Parity: None
 * Stop bits: 1
 * Flow Control: None
 *
 * Note:       If you are using Adafruit USBtoTTL cable, look for COM port
 *             in the Windows Device Manager with the following text:
 *             Silicon Labs CP210x USB to UART Bridge (COM<x>).
 *             Connecting Adafruit USB to TTL:
 *              GND - black wire - connect to the GND pin (on the board or BoosterPack)
 *              Vcc - red wire - leave disconnected
 *              Rx    white wire (receive into USB, connect on TxD of the board P3.3)
 *              Tx -  green wire (transmit from USB, connect to RxD of the board P3.4)
 *        MSP430F5529
 *     -----------------
 * /|\ |            XIN|-
 *  |  |               | 32kHz
 *  |--|RST        XOUT|-
 *     |               |
 *     |   P3.3/UCA0TXD|------------>
 *     |               | 115200 - 8N1
 *     |   P3.4/UCA0RXD|<------------
 *     |           P1.0|----> LED1
 *
 * Input:     None (Type characters in putty/MobaXterm/hyperterminal)
 * Output:    Character echoed at UART
 * Author:    A. Milenkovic, milenkovic@computer.org
 * Date:      October 2018, modified August 2020
 *--------------------------------------------------------------------------------*/
#include <msp430.h>

void UART_setup(void) {

    P3SEL |= BIT3 + BIT4;   // Set USCI_A0 RXD/TXD to receive/transmit data
    UCA0CTL1 |= UCSWRST;    // Set software reset during initialization
    UCA0CTL0 = 0;           // USCI_A0 control register
    UCA0CTL1 |= UCSSEL_2;   // Clock source SMCLK

    UCA0BR0 = 0x09;         // 1048576 Hz  / 115200 lower byte
    UCA0BR1 = 0x00;         // upper byte
    UCA0MCTL |= UCBRS0;     // Modulation (UCBRS0=0x01, UCOS16=0)

    UCA0CTL1 &= ~UCSWRST;   // Clear software reset to initialize USCI state machine
}

void main(void) {
    WDTCTL = WDTPW + WDTHOLD;       // Stop WDT
    P1DIR |= BIT0;                  // Set P1.0 to be output
    UART_setup();                   // Initialize UART

    while (1) {
       while(!(UCA0IFG&UCRXIFG));   // Wait for a new character
       // New character is here in UCA0RXBUF
       while(!(UCA0IFG&UCTXIFG));   // Wait until TXBUF is free
       UCA0TXBUF = UCA0RXBUF;       // TXBUF <= RXBUF (echo)
       P1OUT ^= BIT0;               // Toggle LED1
    }
}
