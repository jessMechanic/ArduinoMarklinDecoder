#include <HardwareSerial.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define motorPin1 3
#define motorPin2 4

volatile bool startSample = false;
volatile uint32_t binary = 0;
volatile uint8_t sample_size = 0;

volatile bool sample_done = false;
volatile bool ignore = false;

const uint8_t decoder_adress = 26;
int train_speed = 0;
bool direction = true;
bool light = false;
bool function1 = false;
bool function2 = false;
bool function3 = false;
bool function4 = false;

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setup_timer0()
{
  TCCR0A = 0; // set entire TCCR0A register to 0
  TCCR0B = 0; // same for TCCR0B
  TCNT0 = 0;  // initialize counter value to 0
  // set compare match register for 2000 Hz increments
  OCR0A = 255; // = 16000000 / (64 * 2000) - 1 (must be <256)
  // turn on CTC mode
  TCCR0B |= (1 << WGM01);
  // Set CS02, CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (0 << CS02) | (1 << CS01) | (1 << CS00);
}

void setup_timer1()
{
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1 = 0;  // initialize counter value to 0
  // set compare match register for 100 Hz increments
  OCR1A = 19999; // = 16000000 / (8 * 100) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12, CS11 and CS10 bits for 8 prescaler
  TCCR1B |= (0 << CS12) | (1 << CS11) | (0 << CS10);
}
void reset_timer()
{
  TCNT0 = 0;
}
void start_timer()
{
  reset_timer();           // initialize counter value to 0
  TIMSK0 |= (1 << OCIE0A); // enable timer compare interrupt
}

void stop_timer()
{
  TIMSK0 &= ~(1 << OCIE0A); // disable timer compare interrupt
}

void setup_int0_falling_edge()
{
  EICRA = 0;
  EIMSK = 0;
  EICRA |= (1 << ISC01); // set INT0 to trigger on falling edge
  EIMSK |= (1 << INT0);  // enable INT0 interrupt
}

ISR(TIMER0_COMPA_vect)
{
  if (sample_size < 1)
  {
    return;
  }
  sample_done = true;

  stop_timer();
}

ISR(INT0_vect)
{

  start_timer();
  reset_timer();
  _delay_us(100);
  binary = binary << 1;
  if (!(PIND & (1 << PIND2)))
  {
    binary |= 1;
  }
  sample_size++;
}

void set_speed()
{

  OCR1A = (int)map(train_speed, 0, 15, 0, 9999);
  if (direction)
  {
    TCCR1A |= (1 << COM1A0) | (0 << COM1A1);
    TCCR1A &= ~((1 << COM1B0) | (0 << COM1B1));
  }

  else
  {
    TCCR1A |= (1 << COM1B0) | (0 << COM1B1);
    TCCR1A &= ~((1 << COM1A0) | (0 << COM1A1));
  }
}
void handle_speed(uint8_t DCBA, uint8_t EFGH)
{
  int train_speed_loc = DCBA == 0 ? DCBA : DCBA - 1;

  if (EFGH == 0b1010 || EFGH == 0b1011)
    direction = false;
  else
    direction = true;

  if (train_speed_loc != train_speed)
  {
    if (abs(train_speed_loc - train_speed) > 5 && train_speed_loc != 0)
    {
      return;
    }
    train_speed = train_speed_loc;
    set_speed();
  }
}
void handle_functions(uint8_t EFGH)
{
  // different functions
  //  110 function 1  // doesnt work for some reason
  //  001 function 2
  //  011 function 3
  //  111 function 4
  if (((EFGH & 0b1110) == 0b1110))
  {
    // function4
    if ((EFGH & 0b0001) != function1)
    {
      function1 = EFGH & 0b0001;
    }
  }
  else if (((EFGH & 0b1110) == 0b0010))
  {
    // function2
    if ((EFGH & 0b0001) != function2)
    {
      function2 = EFGH & 0b0001;
      PORTB &= ~(1 << PORTB4);
      _delay_ms(100);
      PORTB |= (1 << PORTB4);
    }
  }
  else if (((EFGH & 0b1110) == 0b0110))
  {
    // function3
    if ((EFGH & 0b0001) != function3)
    {
      function3 = EFGH & 0b0001;
    }
  }
  else if (((EFGH & 0b1110) == 0b0110))
  {
    // function3
    if ((EFGH & 0b0001) != function1)
    {
      function1 = EFGH & 0b0001;
    }
  }
}

void handle_binary()
{
  uint8_t firstDigit = binary >> 16 & 0b11;
  uint8_t secondDigit = binary >> 14 & 0b11;
  uint8_t thirdDigit = binary >> 12 & 0b11;
  uint8_t fourthDigit = binary >> 10 & 0b11;

  uint8_t adress = fourthDigit * 27 + thirdDigit * 9 + secondDigit * 3 + firstDigit;
  if (adress != decoder_adress)
  {
    return;
  }
  if (light != ((binary >> 9) & 0b1))
  {
    PORTB ^= (1 << PORTB5);
    light = (binary >> 9) & 0b1;
  }
  uint8_t EFGH = ((binary >> 3) & 0b1000) | ((binary >> 2) & 0b100) | ((binary >> 1) & 0b10) | ((binary & 0b1));
  uint8_t DCBA = ((binary >> 7) & 0b1) | (((binary >> 5) & 0b1) << 1) | (((binary >> 3) & 0b1) << 2) | (((binary >> 1) & 0b1) << 3);

  if (EFGH == 0b1010 || EFGH == 0b1011 || EFGH == 0b0101 || EFGH == 0b0100)
  {
    handle_speed(DCBA, EFGH);
  }
  else
  {
    handle_functions(EFGH);
  }
}
int main(void)
{
  Serial.begin(9600);
  DDRD &= ~(1 << DDD2);   // set pin2 as input
  PORTD |= (1 << PORTD2); // enable pull-up on pin2

  DDRB |= (1 << DDB2) | (1 << DDB1) | (1 << DDB5) | (1 << DDB4); // set pin 10 and 9 as output
  PORTB |= (1 << PORTB4);

  // 9 and 10 for motor
  // 8 for light

  PORTB = 0;              // set pin 8, 9 and 10 to low
  PORTB |= (1 << PORTB4); // set pin miso to high
  setup_timer0();
  setup_timer1();
  setup_int0_falling_edge();
  start_timer();
  sei(); // enable global interrupts
  while (1)
  {
    if (sample_done)
    {
      sample_done = false;
      if (sample_size == 18)
      {
        handle_binary();
        binary = 0;
      }
      sample_size = 0;
    }
  }
  return 0;
}