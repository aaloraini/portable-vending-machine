# Arduino Vending Controller – Firmware

Technical documentation for the **4-slot vending machine** firmware running on **Arduino Mega 2560**.  
This README lives alongside the sketch `Machine_Logic.ino` and covers features, wiring, configuration, and tuning.

---

## Table of Contents
- [1) Overview](#1-overview)
- [2) Features](#2-features)
- [3) Hardware & Wiring](#3-hardware--wiring)
  - [3.1 Bill of Materials](#31-bill-of-materials)
  - [3.2 Power](#32-power)
  - [3.3 Pin Map (Mega 2560)](#33-pin-map-mega-2560)
- [4) Configuration Knobs](#4-configuration-knobs)
  - [4.1 Coin Detection](#41-coin-detection)
  - [4.2 Products & Pricing](#42-products--pricing)
  - [4.3 Motors](#43-motors)
  - [4.4 Step Timing](#44-step-timing)
- [5) System Behavior](#5-system-behavior)
- [6) Important Functions](#6-important-functions)
- [7) LCD UI Details](#7-lcd-ui-details)
- [8) Timing & Concurrency Notes](#8-timing--concurrency-notes)
- [9) Customization Examples](#9-customization-examples)
- [10) Safety & Reliability Tips](#10-safety--reliability-tips)
- [11) Troubleshooting](#11-troubleshooting)
- [12) Build & Dependencies](#12-build--dependencies)

---

## 1) Overview

This sketch runs a 4-slot vending mechanism with:
- **I²C 16×2 LCD** (address `0x27`)
- **4× ULN2003** driver boards + **28BYJ-48** steppers (one per product)
- **Coin acceptor** (open-collector pulse output)
- **4×4 keypad** for product selection

**Credits & funds model**
- Balances are tracked in **sen** (cents) and **credits** (MYR).
- The coin acceptor emits **pulse bursts** per coin; pulses are debounced and grouped to detect denomination.
- Products vend if `balance ≥ price`; price is **deducted before** motor motion.

---

## 2) Features

- Smooth **scrolling marquee** (“INSERT YOUR COIN”) on the LCD bottom line.
- Clear LCD states: *idle (balance), coin accepted, unknown coin, insufficient funds, vending start, vending done*.
- **Non-blocking** coin recognition via **ISR + timed grouping**.
- Per-motor **direction** and **step count** tuning.
- Simple **keypad mapping** to products.

---

## 3) Hardware & Wiring

### 3.1 Bill of Materials
- Arduino **Mega 2560**
- LCD **16×2**, I²C backpack (PCF8574 @ `0x27`)
- Coin acceptor (open-collector pulse output)
- **4× ULN2003** stepper driver boards
- **4× 28BYJ-48** steppers
- **4×4** membrane keypad

### 3.2 Power
- **12 V → buck → regulated 5 V** rail for Mega + LCD + keypad + ULN2003 logic  
- **12 V** for coin acceptor (per its spec)

> Keep **all grounds common** (Mega, ULN2003 boards, coin acceptor, buck).

### 3.3 Pin Map (Mega 2560)

| Subsystem | Pin(s) | Notes |
|---|---|---|
| Coin acceptor (pulse) | **D3** | `INPUT_PULLUP`, external **10 k** pull-up recommended; **INT** on **FALLING** |
| I²C LCD | **SDA/SCL** | Mega: **D20 (SDA)**, **D21 (SCL)** |
| Keypad rows | **D4, D5, D6, D7** | `rowPins` |
| Keypad columns | **D8, D9, D10, D11** | `colPins` |
| Motor 1 ULN2003 IN1..IN4 | **D23, D25, D27, D29** | `M_PINS[0]` |
| Motor 2 ULN2003 IN1..IN4 | **D31, D33, D35, D37** | `M_PINS[1]` |
| Motor 3 ULN2003 IN1..IN4 | **D39, D41, D43, D45** | `M_PINS[2]` |
| Motor 4 ULN2003 IN1..IN4 | **D47, D49, D51, D53** | `M_PINS[3]` |

---

## 4) Configuration Knobs

> All constants below live near the top of `Machine_Logic.ino`. Adjust to match your hardware.

### 4.1 Coin Detection
    const uint8_t  P20 = 7;      // pulses for 20 sen
    const uint8_t  P50 = 10;     // pulses for 50 sen
    const uint8_t  TOLERANCE = 1;            // ±1 pulse
    const unsigned long DEBOUNCE_US   = 30000UL; // 30 ms (ISR debouncing)
    const unsigned long GROUP_TIMEOUT = 250UL;   // ms quiet gap to finalize a coin

**How it works:** the **ISR** counts pulses; the `loop()` **finalizes** a coin if no new pulses arrive for `GROUP_TIMEOUT` ms, then matches the pulse count to a denomination within `TOLERANCE`.

**Calibrate:** Watch Serial logs for *“Unknown coin: X pulses”*. Set `P20`/`P50` (or add more) and adjust `TOLERANCE`.

### 4.2 Products & Pricing
    const char  PRODUCT_KEYS[4]   = { '4', '5', '7', '8' };
    const char* PRODUCT_NAMES[4]  = { "Product 1", "Product 2", "Product 3", "Product 4" };
    int         productPrice[4]   = { 20, 50, 70, 100 }; // sen

Map keypad keys to slots and set **prices in sen**.

### 4.3 Motors
    int8_t  MOTOR_DIR[4]             = { +1, +1, -1, +1 };   // per-slot direction
    unsigned VEND_STEPS_PER_MOTOR[4] = { 3072, 3072, 3072, 3072 };

- **Direction:** flip sign if your spiral turns the wrong way.  
- **Steps per vend:** tune for your dispenser’s pitch. (28BYJ-48 ≈ **4076** half-steps/rev typical; many use **2048** depending on gearbox—**3072** is a safe starting point.)

### 4.4 Step Timing
    const unsigned STEP_DELAY_MS = 1.5; // comment says ms

> `delay()` uses **integer milliseconds**. For sub-ms timing use `delayMicroseconds()` and adapt the logic.

---

## 5) System Behavior

**Idle**
- LCD line 1: `Balance: MYR x.xx`
- LCD line 2: scrolling **INSERT YOUR COIN**
- Keypad is polled

**Coin inserted**
- ISR counts pulses (debounced)
- After `GROUP_TIMEOUT` of quiet, denomination is matched and funds increment
- LCD briefly shows **Coin accepted**

**Vend**
- Press one of `PRODUCT_KEYS` to select slot
- If balance < price → **Insufficient / Balance**
- Else: **deduct**, **vending screen**, **stepper run**, **dispensed / Thank you!**, return to idle

**Diagnostics**
- Press **D** to print funds to Serial

---

## 6) Important Functions

- `coinISR()` — Debounces and counts coin pulses (µs). Updates `lastPulseUs` / `lastPulseMs`.
- `rotateInsertCoin()` — Smooth marquee by slicing a doubled string.
- `fmtMoneyCents()` / `fmtPriceOnly()` — Build `MYR w.xx` strings and price strings.
- **Funds helpers** — `totalFundsCents()`, `setFundsFromTotal()`, `addFundsCents()`, `deductFundsCents()`.
- **Motor control** — `motorHalfStep()`, `motorRotate()`, `motorIdle()` (8-phase half-step; de-energize at end).
- **LCD states** — `showIdleScreen()`, `lcdShowCoinAdded()`, `lcdShowUnknownCoin()`, `lcdShowInsufficient()`, `lcdShowVendStart()`, `lcdShowVendDone()`.

---

## 7) LCD UI Details

- **I²C address:** `0x27` → change in `LiquidCrystal_I2C lcd(0x27, 16, 2)` if needed.
- **Line fitting:** `fit16()` pads/truncates to exactly 16 chars (optionally centered) to avoid stale characters.
- **Marquee speed:** `ROTATE_INTERVAL` (ms) and `ROTATE_STEPS` (chars/tick).
- **Vend done:** top line scrolls product name ~3 s; bottom line says *Thank you!*.

---

## 8) Timing & Concurrency Notes

- **ISR vs loop:** ISR only increments `pulseCount` and timestamps; grouping & denomination matching happen in `loop()` when quiet for `GROUP_TIMEOUT`.
- **Debounce:** `DEBOUNCE_US` rejects spurious edges within 30 ms (adjust to your acceptor).
- **Stepper timing:** `delay(STEP_DELAY_MS)` is per half-step; smaller → faster but less torque (possible stalls). Verify supply and load.

---

## 9) Customization Examples

**Add a new coin (e.g., 10 sen)**

    // 1) Read Serial logs: "Unknown coin: X pulses"
    const uint8_t P10 = /* X from measurement */;

    if (abs(int(pulses) - P10) <= TOLERANCE) {
      addFundsCents(10);
      Serial.println("→ Detected 10 sen");
      lcdShowCoinAdded(10);
    } else if (abs(int(pulses) - P20) <= TOLERANCE) {
      // existing cases...
    }

**Change which keys vend**

    const char  PRODUCT_KEYS[4]  = { '1', '2', '3', 'A' }; // example remap

**Tune vend distance**

    VEND_STEPS_PER_MOTOR[slot] = 2800; // increase/decrease until it dispenses exactly one item

---

## 10) Safety & Reliability Tips

- **Power:** Separate motor power if possible; ULN2003 sinks current—ensure **common ground**.
- **Back-EMF:** ULN2003 has flyback diodes; still route wiring cleanly.
- **Coin line:** Short/shielded cable; strong pull-up; confirm acceptor output is **5 V-safe**.
- **Thermals:** Long stalls overheat steppers—keep step times reasonable and **de-energize after motion** (`motorIdle()` already does this).

---

## 11) Troubleshooting

- **“Unknown coin” often** → tweak `TOLERANCE`, ensure `GROUP_TIMEOUT` covers the full pulse train, reduce electrical noise.
- **Missed pulses** → adjust `DEBOUNCE_US` (too large can suppress valid edges), check wiring/grounding.
- **Weak torque / missed steps** → increase `STEP_DELAY_MS` (slower), verify 5 V under load, check coil order.
- **Wrong vend direction** → flip `MOTOR_DIR[slot]` sign.

---

## 12) Build & Dependencies

- **Arduino IDE** (2.x) or **Arduino CLI**
- **Libraries**
  - `Wire.h` *(core)*
  - `LiquidCrystal_I2C.h`
  - `Keypad.h`

**Target:** Arduino **Mega 2560**


