/*! \file *********************************************************************
 *
 * \brief
 *      Transmission d'un code ASCII à l'aide d'une liaison série.
 *
 * \par Note: Basé sur la datasheet ATmega328P.
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

#define FOSC 16000000  //!< Fréquence du microcontrôleur.
#define BAUD 9600      //!< Taux de bauds pour la communication série.
#define MYUBRR FOSC / 16 / BAUD - 1 //!< Valeur de l'UBRR pour le calcul du taux de bauds.

#define FRAMING_ERROR (1 << FE0)
#define PARITY_ERROR (1 << UPE0)
#define DATA_OVERRUN (1 << DOR0)

#define RX_BUFFER_SIZE0 8 //!< Taille du buffer de réception.

int flag = 0; //!< Drapeau pour indiquer l'état du programme.
volatile char donnee; //!< Variable pour stocker les données reçues.

// Variables du buffer de réception.
char rx_buffer0[RX_BUFFER_SIZE0];
volatile uint8_t rx_wr_index0 = 0;
volatile uint8_t rx_rd_index0 = 0;
volatile uint8_t rx_counter0 = 0;
volatile uint8_t rx_buffer_overflow0 = 0;

/*! \brief Fonction principale.
 */
int main(void) {
  USART_Init(MYUBRR);
  USART_Transmit(USART_Receive() + 1);
  sei(); // Activation des interruptions globales.
  while (1) {
    _delay_ms(1);
  }
}

/*! \brief Fonction d'initialisation de la communication série.
 *  \param ubrr Valeur de l'UBRR pour le taux de bauds.
 */
void USART_Init(unsigned int ubrr) {
  /* Configuration du taux de bauds */
  UBRR0H = (unsigned char)(ubrr >> 8);
  UBRR0L = (unsigned char)ubrr;

  /* Activation du récepteur, du transmetteur et de l'interruption de réception */
  UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);

  /* Configuration du format de trame: 8 bits de données, 2 bits d'arrêt */
  UCSR0C = (1 << USBS0) | (3 << UCSZ00);
}

/*! \brief Fonction de transmission de la communication série.
 *  \param data Octet à transmettre.
 */
void USART_Transmit(unsigned char data) {
  /* Attente du tampon de transmission vide */
  while (!(UCSR0A & (1 << UDRE0)))
    ;
  /* Placement des données dans le tampon, puis transmission des données */
  UDR0 = data;
}

/*! \brief Fonction de réception de la communication série.
 *  \return Octet reçu.
 */
unsigned char USART_Receive(void) {
  /* Attente de la réception de données */
  while (!(UCSR0A & (1 << RXC0)))
    ;
  char status = UCSR0A;
  if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN)) == 0) {
    /* Récupération et retour des données reçues depuis le tampon */
    return UDR0;
  } else {
    UDR0; // Ignorer l'octet erroné
    USART_putsln("error");
    return 0; // Ou une valeur qui indique une erreur, selon le contexte
  }
}

/*! \brief Interruption de réception USART.
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

/*! \brief Fonction de transmission d'une chaîne de caractères.
 *  \param str Chaîne de caractères à transmettre.
 */
void USART_puts(unsigned char *str) {
  do {
    USART_Transmit(*str);
  } while (*++str != 0);
}

/*! \brief Fonction de saut de ligne.
 *  \param str Chaîne de caractères à transmettre suivie d'un saut de ligne.
 */
void USART_putsln(unsigned char *str) {
  USART_puts(str);
  USART_Transmit(10); // Nouvelle ligne (Line Feed)
  USART_Transmit(13); // Retour chariot (Carriage Return)
}

/*! \brief Fonction pour ajouter un octet au buffer de réception.
 *  \param data Octet à ajouter au buffer.
 */
void getBuffer(unsigned char data) {
  rx_buffer0[rx_wr_index0++] = data;
  if (rx_wr_index0 == RX_BUFFER_SIZE0)
    rx_wr_index0 = 0;
  if (++rx_counter0 == RX_BUFFER_SIZE0) {
    rx_counter0 = 0;
    rx_buffer_overflow0 = 1;
  }
}
