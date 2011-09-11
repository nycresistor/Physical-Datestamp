#define F_CPU 16000000UL  // 16 MHz
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <string.h>
#include <stdint.h>

#include "Configuration.hh"

const uint16_t HOMING = 0xffff;

volatile uint16_t currentPosInSteps = 0;
volatile uint16_t targetPosInSteps = 0;
volatile bool isDone = true;
volatile bool isHoming = false;

void goToPosition(uint16_t target) {
  isDone = false;
  targetPosInSteps = target;
}

void home() {
  currentPosInSteps = HOMING;
  targetPosInSteps = 0;
  isDone = false;
  isHoming = true;
}

bool isAtPosition() { return isDone; }

void initStepClock()
{
  // The target speed is 50mm/second.
  // We will need 2,500 steps per second.

  // We'll use timer 0; 1/64 prescaler,
  // interrupt on OCR0A match, OCR0A at 100.

  // WGM0: 010  CS0: 011
  TCCR0A = 0x02;
  TCCR0B = 0x03;
  OCR0A = 150;
  TIMSK0 = 0x02; // turn on interrupt
}

volatile uint16_t numServoPos = 0;

void initServos()
{
  NUMBERS_SERVO.setDirection(true);
  NUMBERS_SERVO.setValue(false);

  // Timer 2 for the servos. 1-2ms long every 20ms.
  // 1/1 prescaler,
  // interrupt on OCR2A match, 
  // OCR2A at 160.

  // WGM0: 010  CS0: 001
  TCCR2A = 0x00;
  TCCR2B = 0x01;
  OCR2A = 160;
  TIMSK2 = 0x02; // turn on interrupt
}

void initStepPins()
{  
  X_STEP_PIN.setDirection(true);
  X_STEP_PIN.setValue(false);
  X_DIR_PIN.setDirection(true);
  X_DIR_PIN.setValue(false);
  X_ENABLE_PIN.setDirection(true);
  X_ENABLE_PIN.setValue(true);
  X_END_PIN.setDirection(false);
}


void initSerial() {
  UBRR0H = UBRR_VALUE >> 8;
  UBRR0L = UBRR_VALUE & 0xff;
  /* set config for uart, explicitly clear TX interrupt flag */
  UCSR0A = UCSR0A_VALUE | _BV(TXC0);
  UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);
  UCSR0C = _BV(UCSZ01)|_BV(UCSZ00);
  /* defaults to 8-bit, no parity, 1 stop bit */
}

void printSerial(char* pTxt) {
  while(*pTxt != '\0') {
    UCSR0A |= _BV(TXC0);
    UDR0 = *pTxt;
    while ((UCSR0A & _BV(TXC0)) == 0);
    pTxt++;
  }
}

void init()
{
  cli();
  PSU_PIN.setDirection(true);
  PSU_PIN.setValue(false);
  initSerial();
  printSerial("Initializing stamper.\n");
  _delay_ms(1200);
  initStepPins();
  initServos();
  initStepClock();
  //set_sleep_mode( SLEEP_MODE_IDLE );
  sei();
}

volatile uint8_t rxOffset = 0;
volatile char rxBuf[255];
volatile bool rxPending = false;
char command[255];

void processCommand() {
  char* p = command;
  if (*p == 'S') { // Stepper position
    p++;
    int val = 0;
    while (*p <= '9' && 
	   *p >= '0') {
      val = val * 10;
      val += (int)(*p - '0');
      p++;
    }
    printSerial("OK\n");
    goToPosition(val);
  }
  else if (*p == 'N') { // Numbers position
    p++;
    uint16_t val = 0;
    while (*p <= '9' && 
	   *p >= '0') {
      val = val * 10;
      val += (int)(*p - '0');
      p++;
    }
    printSerial("OK\n");
    numServoPos = val;
  }
  else if (*p == 'H') { // Home stepper
    home();
  }
  rxPending = false;
}

int main( void )
{
  init();
  home();
  while(!isAtPosition());

  while (1) {
    if (rxPending) {
      processCommand();
    }
  }
  return 0;
}

ISR(USART0_RX_vect)
{
  rxBuf[ rxOffset ] = UDR0;
  if ( rxBuf[ rxOffset ] == '\r' ) {
    UDR0 = '\r';
    rxBuf[ rxOffset ] = '\0';
    for (int i = 0; i < 255; i++) {
      command[i] = rxBuf[i];
      if (command[i] == '\0') break;
    }
    rxOffset = 0;
    rxPending = true;
  } else {
    UDR0 = rxBuf[rxOffset];
    rxOffset++;
    if (rxOffset > 250) { rxOffset = 250; }
  }
}

volatile uint16_t ticks = 0;
const uint16_t TICK_TOP = 1000;

ISR(TIMER2_COMPA_vect)
{
  NUMBERS_SERVO.setValue(ticks<numServoPos);
  ticks++;
  if (ticks > TICK_TOP) {
    ticks = 0;
  }
}

ISR(TIMER0_COMPA_vect)
{
  if (isHoming) {
    if (!X_END_PIN.getValue()) {
      X_ENABLE_PIN.setValue(true);
      currentPosInSteps = targetPosInSteps = 0;
      isHoming = false;
      isDone = true;
      return;
    }
  }
  if (isDone || (currentPosInSteps == targetPosInSteps)) {
    X_ENABLE_PIN.setValue(true);
    isDone = true;
  } else {
    X_ENABLE_PIN.setValue(false);
    isDone = false;
    bool forward = currentPosInSteps < targetPosInSteps;
    if (!forward && !X_END_PIN.getValue()) {
      // stop!
      isDone = true;
      return;
    }
    X_DIR_PIN.setValue(forward);
    currentPosInSteps += forward?1:-1;
    X_STEP_PIN.setValue(true);
    asm volatile("nop\n\t"
		 "nop\n\t"
		 "nop\n\t"
		 "nop\n\t"
		 ::);
    X_STEP_PIN.setValue(false);
  }
}
