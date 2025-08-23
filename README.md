# Portable Vending Machine

Open-source prototype of a lightweight **coin-operated portable vending machine**.  
This repo bundles the **firmware (Arduino)**, **machine logic**, **electronics**, **CAD layouts**, and the **design book** so others can reproduce, learn from, or extend the project.

---

## Table of Contents
- [Features](#features)
- [Repository Contents](#repository-contents)
- [Quick Start (Firmware)](#quick-start-firmware)
- [Electronics & CAD](#electronics--cad)
- [Bill of Materials](#bill-of-materials)
- [Machine Logic](#machine-logic)
- [How to Reproduce](#how-to-reproduce)
- [Contributing](#contributing)
- [License](#license)
- [Team](#team)

---

## Features
- Coin-based credit system with keypad selection
- 16×2 I2C LCD user interface (status, credit, messages)
- Four stepper-driven dispensing channels (ULN2003 + 28BYJ-48–style)
- Simple, robust flow: **Idle → Coin → Balance → Select → Vend → Thank You**
- Reproducible documentation: **design book**, **schematic**, **layouts**, **BOM**

---

## Repository Contents

| Area | Files |
|---|---|
| **Design Book** | `design book.pdf` |
| **Electronics** | `Electronics Sch.png` |
| **BOM** | `Demo BOM.xlsx` |
| **Machine Logic** | `machine-logic.pdf` |
| **Firmware (Arduino)** | `Machine_Logic.ino` |
| **CAD / Layouts** | `Vending Machine Final Layouts.dwg`, `Vending Machine Layouts.pdf` |
| **Poster** | `Smart Portable Vending Machine Poster.pdf` |


---

## Quick Start (Firmware)

1. **Open** `Machine_Logic.ino` in **Arduino IDE 2.x**.  
2. **Board:** Arduino Mega 2560 (or update to your board).  
3. **Libraries (install via Library Manager):**
   - `LiquidCrystal_I2C` (16×2 LCD over I2C)
   - `Keypad` (4×4 matrix keypad)
   - (Optional) use Arduino built-in `Stepper` or your preferred stepper driver library
4. **Wire** the modules (LCD I2C, keypad rows/cols, coin selector signal, ULN2003 stepper boards, 12V→5V buck).  
5. **Verify** → **Upload**.  
6. Open **Serial Monitor** (if used) and test: insert coin → choose product → observe vending.


---

## Electronics & CAD

- **Schematic:** `Electronics Sch.png`  
- **CAD:**
  - Native drawing: `Vending Machine Final Layouts.dwg`
  - Printable views: `Vending Machine Layouts.pdf`


---

## Bill of Materials

- **BOM file:** `Demo BOM.xlsx`  

Common modules used:
- **Coin selector** (12V) with pulse output
- **Buck converter** (12V→5V) for logic rail
- **4× ULN2003 stepper drivers** + **28BYJ-48 steppers**
- **16×2 LCD (I2C backpack)**
- **4×4 matrix keypad**
- **Arduino Mega 2560**

---

## Machine Logic

- Diagram: `machine-logic.pdf`  
Nominal state flow:
1. **Idle** → wait for coin pulses  
2. **Balance Update** → display credit  
3. **Selection** via keypad  
4. **Check balance** → if sufficient, **deduct** and **vend** (drive selected stepper)  
5. **Thank You** → return to **Idle**

---

## How to Reproduce

1. **Mechanics/CAD**  
   - Review `Vending Machine Layouts.pdf` for dimensions and part placement.  
   - (Optional) export Files for 3D fabrication.
2. **Electronics**  
   - Assemble per `Electronics Sch.png`.  
   - Use `Demo BOM.xlsx` to source parts.  
   - Power: 12V supply → buck to 5V for MCU/LCD/logic; keep coin selector at 12V (signal conditioned to MCU input).
3. **Firmware**  
   - Upload `Machine_Logic.ino`.  
   - Adjust pins, motor step counts, and pulse-per-coin values to match your hardware.
4. **Test**  
   - Insert coins; verify credit increments.  
   - Select each channel; confirm the correct motor runs and stops as expected.

---

## Contributing

Contributions are welcome!  
- Open an **Issue** for bugs/ideas.  
- Use small, focused **PRs** with screenshots, logs, or short clips.  
- Prefer clear commit messages (`feat: …`, `fix: …`, `docs: …`).  
- If you add files, keep to the recommended folder layout above.

---

## License

Released under the **MIT License** (see `LICENSE`).  
If your use case needs a different license, please open an issue to discuss dual-licensing or exceptions.

---

## Team

- **Aloraini Abdulhakim**  
- **Abdelmohsine Smili**  
- **Salma Yasser**  
- **Mohammed Salman**  
- **Muhannadhu Musthafa**  
- **Aiham Samooh**

> Special thanks to everyone who supported testing and feedback.

---

