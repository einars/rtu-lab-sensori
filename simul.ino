#include "Arduino.h"

const int out_red = 5;
const int out_yellow = 6;
const int out_green = 7;

const int in_nosuce = A0;
const int in_plotter = A1;

const int test_nosuce_inv = 16;
const int test_plotter_inv = 17;

const int debug_nosuce = 10;
const int debug_plotter = 9;

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
const unsigned Cooldown_s     digitalWrite(out_red, r ? HIGH : LOW);

    digitalWrite(out_yellow, y ? HIGH : LOW);

    digitalWrite(out_green, g ? HIGH : LOW);
}
= 5;

struct state_t {
    Plotter plotter = Plotter::Idle;
    unsigned long t_cooldown_end = 0;
    bool nosuce_on = false;
    bool plotter_on = false;
};

void leds(int r, int y, int g) {
    digitalWrite(out_red, r ? HIGH : LOW);

    digitalWrite(out_yellow, y ? HIGH : LOW);

    digitalWrite(out_green, g ? HIGH : LOW);
}

void blink_hello () {
    leds(1, 1, 1);
    delay(500);
}

void get_state (state_t& s) {
    // todo: debounce

    s.plotter_on = ! digitalRead(test_plotter_inv);

    unsigned long t = millis() / 1000; // operējam ar sekundēm

    if (s.plotter == Plotter::Printing && ! s.plotter_on) {
        s.plotter = Plotter::Cooldown;
        s.t_cooldown_end = t + Cooldown_s;
    }
    if (s.plotter_on) {
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

    s.nosuce_on = ! digitalRead(test_nosuce_inv);

    if ( ! s.nosuce_on && s.plotter == Plotter::Finished) {
        // nosūce izslēgta, atgriežamies atkal sākuma pozīcijā, resetojam printeri uz Idle
        // (mazliet atšķiras indikācijas, atkarībā no tā, vai printeris ir Idle, vai Finished —
        // idle nav nekā jāindicē, Finished tiek spīdināts)
        s.plotter = Plotter::Idle;
    }
}

state_t n;

void setup () {

    // indikatori + optopāri
    pinMode(out_red, OUTPUT);
    pinMode(out_yellow, OUTPUT);
    pinMode(out_green, OUTPUT);

    pinMode(in_nosuce, INPUT);
    pinMode(in_plotter, INPUT);

    pinMode(test_nosuce_inv, INPUT_PULLUP);
    pinMode(test_plotter_inv, INPUT_PULLUP);

    pinMode(debug_plotter, OUTPUT);
    pinMode(debug_nosuce, OUTPUT);


    leds(0, 0, 0);

    // blink_hello();

    Serial.begin(115200);

}

void loop_test () {
    // leds(digitalRead(test_nosuce_inv), digitalRead(test_plotter_inv), millis() % 1000 > 200);
    // digitalWrite(debug_nosuce,  ! digitalRead(test_nosuce_inv));
    // digitalWrite(debug_plotter, ! digitalRead(test_plotter_inv));

    const int d = 1000;
    leds(1, 1, 0);
    delay(d);
    leds(0, 1, 1);
    delay(d);
    leds(1, 0, 1);
    delay(d);
}

void loop_sim () {

    get_state(n);

    Health h = Health::Ok;

    // debugi
    digitalWrite(debug_nosuce, n.nosuce_on);
    digitalWrite(debug_plotter, n.plotter_on);

    if ( ! n.nosuce_on) {

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
        if ((millis() % 1000) < 200) {
            leds(1, 0, 0);
        } else {
            leds(1, 1, 0);
        }
        break;
    case Health::Working:
        leds(1, 0, 0);
        break;
    }

    Serial.print("NOS: ");
    Serial.print(analogRead(in_nosuce));

    Serial.print(" LAZ: ");
    Serial.print(analogRead(in_plotter));
    Serial.print("\n");

    delay(100);


}

void loop () {
    // loop_sim();
    loop_test();
}
