#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "Pin.hh"

// PSU pin
#define PSU_PIN                 Pin(PortD,6)

// Serial setup params
#define UBRR_VALUE 25
#define UCSR0A_VALUE 0

// The X stepper step pin (active on rising edge)
#define X_STEP_PIN              Pin(PortD,7)
// The X direction pin (forward on logic high)
#define X_DIR_PIN               Pin(PortC,2)
// The X stepper enable pin (active low)
#define X_ENABLE_PIN            Pin(PortC,3)
// The X maximum endstop pin (active high)
#define X_END_PIN               Pin(PortC,5)

// Servo for the number wheel
#define NUMBERS_SERVO           Pin(PortC,4)

// Servo for the stamper
#define STAMPER_SERVO           Pin(PortC,7)

// Steps per mm
#define STEPS_PER_MM 50

#endif // CONFIGURATION_H
