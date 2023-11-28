/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *      Transmition d'un code ASCII à l'aide d'une liaison série
 *
 * \par Note: A partir de la Datasheet ATmega328P
 *
 * \par Documentation: https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
 *
 * \author
 *      Github: https://github.com/kaanel
 *      Email: romainmarteaud@gmail.com
 *
 * $Name: Liaison série $
 * $Revision: 1.0 $
 * $RCSfile: main.c,v $
 * $Date: 2023/11/27 11:03 $  \n
 ******************************************************************************/
//#include <avr/io.h>
#define FOSC 16000000  // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC / 16 / BAUD - 1

#define FRAMING_ERROR (1 << FE0)
#define PARITY_ERROR (1 << UPE0)
#define DATA_OVERRUN (1 << DOR0)

#define RX_BUFFER_SIZE0 8

int flag = 0;
volatile char donnee;

/*! \brief Fonction Main 
 */
int main(void) {
  USART_Init(MYUBRR);
  USART_Transmit(USART_Receive() + 1);
  sei();
  while (1) {
    _delay_ms(1);
  }
}

/*! \brief Fonction Initialisation de la communication
 */
void USART_Init(unsigned int ubrr) {
  /*Set baud rate */
  UBRR0H = (unsigned char)(ubrr >> 8);
  UBRR0L = (unsigned char)ubrr;
  //Enable receiver and transmitter * /
  UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
  /* Set frame format: 8data, 2stop bit */
  UCSR0C = (1 << USBS0) | (3 << UCSZ00);
}

/*! \brief Fonction Transmission de la communication
 */
void USART_Transmit(unsigned char data) {
  /* Wait for empty transmit buffer */
  while (!(UCSR0A & (1 << UDRE0)))
    ;
  /* Put data into buffer, sends the data */
  UDR0 = data;
}

/*! \brief Fonction Reception de la communication
 */
unsigned char USART_Receive(void) {
  /* Wait for data to be received */
  while (!(UCSR0A & (1 << RXC0)))
    ;
  char status = UCSR0A;
  if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN)) == 0) {
    /* Get and return received data from buffer */
    return UDR0;
  } else {
    UDR0;
    USART_putsln("error");
  }
}

/*! \brief https://www.nongnu.org/avr-libc/user-manual/group__avr__interrupts.html
 */
ISR(USART_RX_vect) {
  char status = UCSR0A;
  char byteReceive = UDR0;
  if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN)) == 0) {
    USART_Transmit(byteReceive);
  } else {
    USART_putsln("error");
  }
}

/*! \brief Fonction transmission d'une chaine de caractère
 */
void USART_puts(unsigned char *str) {
  do {
    USART_Transmit(*str);
  } while (*++str != 0);
}

/*! \brief Fonction sauter une ligne
 */
void USART_putsln(unsigned char *str) {
  USART_puts(str);
  USART_Transmit(10);
  USART_Transmit(13);
}

void getBuffer(){
  char rx_buffer0[RX_BUFFER_SIZE0];
  rx_buffer0[rx_wr_index0++]=data;
 if (rx_wr_index0 == RX_BUFFER_SIZE0) rx_wr_index0=0;
 if (++rx_counter0 == RX_BUFFER_SIZE0)
 {
 rx_counter0=0;
 rx_buffer_overflow0=1;
 }
}
