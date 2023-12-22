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

#define FOSC 16000000  //!< Fréquence du microcontrôleur.
#define BAUD 9600      //!< Taux de bauds pour la communication série.
#define MYUBRR FOSC / 16 / BAUD - 1 //!< Valeur de l'UBRR pour le calcul du taux de bauds.

int flag = 0; //!< Drapeau indiquant la réception de données sur USART0.
int flag3 = 0; //!< Drapeau indiquant la réception de données sur USART3.

/*! \brief Fonction principale.
 */
int main(void) {
  USART_Init(MYUBRR);
  sei(); // Activation des interruptions globales.

  while (1) {
    if (flag3 == 1) {
      USART_Transmit3(UDR3); // Transmission sur USART3
      flag3 = 0;
    }
    _delay_ms(1);

    if (flag == 1) {
      USART_Transmit(UDR0); // Transmission sur USART0
      flag = 0;
    }
    _delay_ms(1);
  }
}

/*! \brief Interruption de réception sur USART3.
 */
ISR(USART3_RX_vect) {
  flag3 = 1; // Indique la réception de données sur USART3.
}

/*! \brief Interruption de réception sur USART0.
 */
ISR(USART_RX_vect) {
  flag = 1; // Indique la réception de données sur USART0.
}

/*! \brief Fonction d'initialisation de la communication série.
 *  \param ubrr Valeur de l'UBRR pour le taux de bauds.
 */
void USART_Init(unsigned int ubrr) {
  /* Configuration du taux de bauds pour USART0 */
  UBRR0H = (unsigned char)(ubrr >> 8);
  UBRR0L = (unsigned char)ubrr;

  /* Configuration du taux de bauds pour USART3 */
  UBRR3H = (unsigned char)(ubrr >> 8);
  UBRR3L = (unsigned char)ubrr;

  // Activation du récepteur, du transmetteur et de l'interruption de réception
  UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
  UCSR3B = (1 << RXEN3) | (1 << TXEN3) | (1 << RXCIE3);

  // Configuration du format de trame: 8 bits de données, 1 bit d'arrêt pour USART0
  UCSR0C = (3 << UCSZ00);

  // Configuration du format de trame: 8 bits de données, 1 bit d'arrêt pour USART3
  UCSR3C = (3 << UCSZ30);
}

/*! \brief Fonction de transmission sur USART0.
 *  \param data Octet à transmettre.
 */
void USART_Transmit(unsigned char data) {
  /* Attente du tampon de transmission vide */
  while (!(UCSR0A & (1 << UDRE0)))
    ;
  /* Placement des données dans le tampon, puis transmission des données */
  UDR0 = data;
}

/*! \brief Fonction de transmission sur USART3.
 *  \param data Octet à transmettre.
 */
void USART_Transmit3(unsigned char data) {
  /* Attente du tampon de transmission vide */
  while (!(UCSR3A & (1 << UDRE3)))
    ;
  /* Placement des données dans le tampon, puis transmission des données */
  UDR3 = data;
}

/*! \brief Fonction de réception sur USART0.
 *  \return Octet reçu.
 */
unsigned char USART_Receive(void) {
  /* Attente de la réception de données */
  while (!(UCSR0A & (1 << RXC0)))
    ;
  /* Récupération et retour des données reçues depuis le tampon */
  return UDR0;
}

/*! \brief Fonction de réception sur USART3.
 *  \return Octet reçu.
 */
unsigned char USART_Receive3(void) {
  /* Attente de la réception de données */
  while (!(UCSR3A & (1 << RXC3)))
    ;
  /* Récupération et retour des données reçues depuis le tampon */
  return UDR3;
}
