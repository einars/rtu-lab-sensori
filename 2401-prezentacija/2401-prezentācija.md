---
marp: true
theme: olive
paginate: true
---

# Neinvazīva sistēma lāzergriezēja monitorēšanai

Einārs Lielmanis, 2024

https://github.com/einars/rtu-lab-sensori

---
# Arhitektūra

![white](img/arch-basic.png)

---


![bg](img/plate.jpg)

---
# Shēma

https://github.com/einars/rtu-lab-sensori/tree/main/kicad/ploterim

![whitesm](img/schematic.png)

---

# Arhitektūra ar testiem

![white](img/arch-full.png)

---

# Testi

![white](img/plate-indik.png)

---

# Barošana

- USB
- 24V 

  ![](img/24v.png)

- abus kopā nevajadzētu

---

# Ievade

  ![white](img/plate-in.png)

---

# Ievade

  ![white](img/schematic-in.png)

---

# Izvade

![white](img/plate-out.png)

---

# Kods

## Ploteris

Steitmašīna: Idle → Printing → Cooldown → Finished (→ Printing)

Cooldown periods definēts kodā.

## Nosūcējs

Steitmašīna On → Off (→ On)

Raugoties uz šiem stāvokļiem, algoritms izdomā, cik veselīgu luksoforu attēlot.

---

```C
    Health h = Health::Ok;

    if ( ! n.nosuce_on) {
        h = Health::ErrNosucejs;
        if (n.plotter != Plotter::Idle && n.plotter != Plotter::Finished)
            h = Health::ErrNosucejsEmergency;
    } else
        if (n.plotter != Plotter::Idle && n.plotter != Plotter::Finished)
            h = Health::Working;

    switch (h)
    case Health::Ok:
        if (n.plotter == Plotter::Finished) leds(0, 0, 1);
        else leds(0, 0, 0);

    case Health::ErrNosucejs:
        leds(0, 1, 0);

    case Health::ErrNosucejsEmergency:
        leds(1, 1, 0);

    case Health::Working:
        leds(1, 0, 0);
```

---

# Praktiski

- Testi ļāva darboties bez pašas iekārtas

- analogie lasītāji praksē šobrīd nestrādā: kaut kur, lodējot, kļūme un, pieslēdzot vienu, abos nolasās miskaste

---

# Challenges

- smd ir lieliski, through-hole — ķēpa
- maketplates savienojumi ir elle

# Next

- Uzmaketēt īstu platīti
- Izveidot kastīti
- Iedzīvināt dzīvē
