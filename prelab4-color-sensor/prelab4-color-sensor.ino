/*
 * Lab 4
 *
 * Created (Date): 3/27/2025
 * Author (Name, netID): Michelle Yang, my394
 * Comment: See PDF submission for more comments (although minor error in placement of certain code in PDF submission)
 */ 

// --- Global Variables ---

// "period" : stores the value of the output wave period (not necessarily in seconds) 
volatile int period = 0; // volatile so the operating system doesn't optimize the code and checks resister changes to trigger interrupt (I think)
// In microsecond ^
// "timer" : stores the value of TIMER1 
volatile int timer = 0;


// --- ISR: an interrupt function that resets and reads the timer value

ISR(PCINT2_vect) // From interrupt vector table
{	
  // resets the timer to zero on a rising edge
  if (PIND & 0b00010000) { // If OUT of the color sensor on a rising edge (high)
    timer = TCNT1; // Store current timer value TCNT1 in variable timer
    
  } else { // and stores the timer value in a variable ("timer") on a falling edge (or vice versa).
    TCNT1 = 0; // Reset timer
  }
}


// --- initColor: initializes the interrupt and timer 

void initColor()
{

  // a.	Initialize your I/O pins
  // DDRD &= 0b11101111; // PD4, OUT --> input (0) to control LED
  DDRB = 0b00000001; // PB0, LED --> output (1)
 
  // b. Initialize my pin change interrupt
  PCICR = 0b00000100; // PCINT20, output of color sensor (act as input to) controls LED

  // c.	Initialize your timer --> normal mode, prescaler N = 1
  TCCR1A = 0b00000000; // WGM11, WGM10 set to 0
  TCCR1B = 0b00000001; // WGM13, WGM12, CS12, CS11 set to 0. CS10 set to 1. 

}


// --- getColor: calculates the period of the sensor output wave and returns it as a variable

int getColor()
{

  // a.	Enable the specific bit for your Pin Change Interrupt
  PCMSK2 = 0b00010000; // Enable pin change interrupt 2 (PCINT 16 to 23)
  // b.	Add a short delay (5 ~ 10 milliseconds)
  _delay_ms(7); // 7 miliseconds

  // c.	Disable the specific bit for your pin change interrupt
  // (to prevent further interrupts until you call getColor again)
  PCMSK2 = 0b00000000; 

  // d.	Return the period in units of time
  period = (timer*2)*(.0625); // TCNT1 captures time from rising edge to falling edge --> 1 full period = 2 * that time
  Serial.println(timer);
  return period;
}


int main(void)
{
  // call initColor function
  initColor();

  sei(); // Enable interrupt

  Serial.begin(9600); // For debugging, initialize serial communication
  while(1) 
  {
    // call getColor function
    getColor(); // This function call enables interrupt --> which updates the global variable period

    // print the "period"
    Serial.println(period);

    // Turn LED on if period is above certain threshold
    if (period > 200) { // For example, if threshold = 250 microseconds --> 4000 ticks 
      PORTB = 0b00000001; // Turn LED on
    } else {
      PORTB = 0b00000000; // Turn LED off
    }
    // Add a short delay (less than a second)
    _delay_ms(500); // Half a second --> monitar display period easier to read
  }

}
