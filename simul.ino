#include "Arduino.h"

const int pin_green = 4;
const int pin_yellow = 5;
const int pin_red = 6;

const int pin_nosuce = 12;
const int pin_lazers = 13;

enum Plotter {
  Idle,
  Printing,
  Cooldown,
  Finished,
};

enum Health {
  Ok,
  ErrNosucejs,
  ErrNosucejsEmergency,
  Working,
};


struct state_t {
  Plotter plotter = Plotter::Idle;
  unsigned long laser_off_at_ms = 0;
  bool nosuce = false;
  bool lazers = false;
};


void leds(int r, int y, int g) {
  digitalWrite(pin_red, r ? HIGH : LOW); 
  digitalWrite(pin_yellow, y ? HIGH : LOW); 
  digitalWrite(pin_green, g ? HIGH : LOW); 
}

void blink_hello () {
  leds(1, 1, 1);
  delay(500);
}

void get_state (state_t& s) {
  // todo: debounce

  s.lazers = ! digitalRead(pin_lazers);

  unsigned t = micros() / 1000;

  if (s.plotter == Plotter::Printing && ! s.lazers) {
    s.plotter = Plotter::Cooldown;
    s.laser_off_at_ms = t + 5000;
  }
  if (s.lazers) {
    s.plotter = Plotter::Printing;
  }

  if (s.plotter == Plotter::Cooldown && t > s.laser_off_at_ms) {
    s.plotter = Plotter::Finished;
  }

  s.nosuce = ! digitalRead(pin_nosuce);

  if ( ! s.nosuce && s.plotter == Plotter::Finished) {
    // nosūce izslēgta, atgriežamies atkal sākuma pozīcijā: kad tiks ieslēgta, atkal nekas nespīdēs
    s.plotter = Plotter::Idle;
  }
}

state_t n;

void setup () {

  pinMode(pin_green, OUTPUT);
  pinMode(pin_red, OUTPUT);
  pinMode(pin_yellow, OUTPUT);

  pinMode(pin_nosuce, INPUT_PULLUP);
  pinMode(pin_lazers, INPUT_PULLUP);

  blink_hello();

  Serial.begin(115200);

}

void loop () {

  get_state(n);

  Health h = Health::Ok;

  if ( ! n.nosuce) {

    h = ErrNosucejs;

    if (n.plotter != Plotter::Idle && n.plotter != Plotter::Finished) {
      h = ErrNosucejsEmergency;
    }
  } else {
    if (n.plotter != Plotter::Idle && n.plotter != Plotter::Finished) {
      h = Health::Working;
    }
  }

  switch (h) {
  case Health::Ok:
    if (n.plotter == Plotter::Finished) {
      leds(0, 0, 1);
    } else {
      leds(0, 0, 0);
    }
    break;
  case Health::ErrNosucejs:
    leds(0, 1, 0);
    break;
  case Health::ErrNosucejsEmergency:
    if ((micros() / 500000) % 2 == 1) {
      leds(0, 1, 0);
    } else {
      leds(1, 1, 0);
    }
    break;
  case Health::Working:
    leds(1, 0, 0);
    break;
  }

}
