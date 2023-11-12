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

// cik sekundes ilgs ir kūldauns pēc lāzera beigām, lai uzskatītu, ka printeris ir beidzis darbu.
const unsigned Cooldown_s = 5;


struct state_t {
  Plotter plotter = Plotter::Idle;
  unsigned long t_cooldown_end = 0;
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

  unsigned long t = millis() / 1000; // operējam ar sekundēm

  if (s.plotter == Plotter::Printing && ! s.lazers) {
    s.plotter = Plotter::Cooldown;
    s.t_cooldown_end = t + Cooldown_s;
  }
  if (s.lazers) {
    s.plotter = Plotter::Printing;
  }

  // millis() overflowo pēc 50 dienām: ja plotera indikators būs ieslēgts 50
  // dienas pēc kārtas, tad 50. dienā tas var palaist garām vienu
  // Cooldown => Finished pāreju un palikt nebeidzamā cooldownā līdz nākamajai
  // drukai.
  //
  if (s.plotter == Plotter::Cooldown && t > s.t_cooldown_end) {
    s.plotter = Plotter::Finished;
  }

  s.nosuce = ! digitalRead(pin_nosuce);

  if ( ! s.nosuce && s.plotter == Plotter::Finished) {
    // nosūce izslēgta, atgriežamies atkal sākuma pozīcijā, resetojam printeri uz Idle
    // (mazliet atšķiras indikācijas, atkarībā no tā, vai printeris ir Idle, vai Finished —
    // idle nav nekā jāindicē, Finished tiek spīdināts)
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
      leds(1, 0, 0);
    } else {
      leds(1, 1, 0);
    }
    break;
  case Health::Working:
    leds(1, 0, 0);
    break;
  }

}
