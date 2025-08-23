# Portable Vending Machine 

An open-source prototype of a lightweight, **coin-operated portable vending machine**. This repository contains everything needed to reproduce the project: the **firmware (Arduino)**, **machine logic**, **electronics schematics**, **CAD layouts**, and a comprehensive **design book**.

---

## 📖 Table of Contents

- [Features](#-features)
- [Repository Contents](#-repository-contents)
- [Quick Start (Firmware)](#-quick-start-firmware)
- [Electronics & CAD](#-electronics--cad)
- [Bill of Materials](#-bill-of-materials)
- [Machine Logic](#-machine-logic)
- [How to Reproduce](#-how-to-reproduce)
- [Contributing](#-contributing)
- [License](#-license)
- [Team](#-team)

---

## ✨ Features

- **Coin-Based Credit System** with a 4x4 matrix keypad for product selection.
- **User Interface** via a 16×2 I2C LCD (displays status, credit, and messages).
- **Dispensing Mechanism** powered by four stepper motors (ULN2003 drivers + 28BYJ-48 style).
- **Robust Logic Flow**: `Idle` → `Coin Inserted` → `Balance Update` → `Selection` → `Vend` → `Thank You`.
- **Complete Documentation**: Includes a design book, schematics, CAD layouts, and a Bill of Materials (BOM) for easy reproduction.

---

## 📁 Repository Contents

| Category | File(s) | Description |
| :--- | :--- | :--- |
| **Design Book** | `design book.pdf` | Comprehensive project documentation. |
| **Electronics** | `Electronics Sch.png` | Wiring schematic. |
| **BOM** | `Demo BOM.xlsx` | Bill of Materials for sourcing parts. |
| **Machine Logic** | `machine-logic.pdf` | State flow diagram. |
| **Firmware** | `Machine_Logic.ino` | Main Arduino sketch. |
| **CAD / Layouts** | `Vending Machine Final Layouts.dwg`<br>`Vending Machine Layouts.pdf` | Native CAD file and printable PDF. |
| **Poster** | `Smart Portable Vending Machine Poster.pdf` | Project summary poster. |

---

## 🚀 Quick Start (Firmware)

To get the firmware running on your hardware:

1.  **Open** the `Machine_Logic.ino` file in the **Arduino IDE (2.x recommended)**.
2.  **Select Board:** Choose *Arduino Mega 2560* (or modify the code for your specific board).
3.  **Install Libraries** (via Sketch > Include Library > Manage Libraries):
    - `LiquidCrystal_I2C` by Frank de Brabander (for the 16×2 LCD).
    - `Keypad` by Mark Stanley, Alexander Brevig (for the 4×4 matrix keypad).
    - The built-in `Stepper` library is used for motor control.
4.  **Wire the modules** according to the schematic (`Electronics Sch.png`). Key connections:
    - LCD via I2C (SDA, SCL)
    - Keypad rows and columns
    - Coin selector signal pin
    - ULN2003 stepper driver boards
    - 12V→5V buck converter for power
5.  **Compile** and **Upload** the sketch to your board.
6.  **Test:** Open the Serial Monitor (if debugging is enabled), insert a coin (or simulate a pulse), select a product, and observe the vending sequence.

---

## 🔌 Electronics & CAD

-   **Schematic:** Refer to `Electronics Sch.png` for the complete circuit diagram.
-   **CAD Files:**
    -   `Vending Machine Final Layouts.dwg` - Native AutoCAD file for editing.
    -   `Vending Machine Layouts.pdf` - Printable PDF for fabrication.

---

## 📋 Bill of Materials

The complete parts list is available in: `Demo BOM.xlsx`

**Key Components:**
-   Arduino Mega 2560
-   16×2 LCD with I2C backpack
-   4×4 Matrix Keypad
-   Coin Selector (12V with pulse output)
-   Buck Converter (12V to 5V)
-   4x ULN2003 Stepper Driver Boards
-   4x 28BYJ-48 Stepper Motors
-   12V Power Supply

---

## ⚙️ Machine Logic

The operational workflow is detailed in: `machine-logic.pdf`

**Nominal State Flow:**
1.  **Idle:** Waits for a coin pulse.
2.  **Balance Update:** Credit is added and displayed on the LCD.
3.  **Selection:** User inputs a product code via the keypad.
4.  **Vend:** If credit is sufficient, the cost is deducted, and the corresponding stepper motor is activated.
5.  **Thank You:** A confirmation message is displayed before returning to the **Idle** state.

---

## 🛠️ How to Reproduce

Follow these steps to build your own unit:

1.  **Mechanical Assembly:**
    -   Review the `Vending Machine Layouts.pdf` for dimensions and assembly instructions.
    -   Fabricate parts using a laser cutter or 3D printer as indicated.
2.  **Electronics Assembly:**
    -   Wire all components according to the `Electronics Sch.png` schematic.
    -   Use the `Demo BOM.xlsx` to source all necessary parts.
    -   **Power Note:** The system uses a 12V supply. A buck converter steps this down to 5V for the MCU and logic, while the coin selector runs directly on 12V (its signal is conditioned for the MCU).
3.  **Software Setup:**
    -   Upload the `Machine_Logic.ino` sketch to your Arduino.
    -   Calibrate the code for your specific hardware: adjust pin definitions, motor step counts, and pulses-per-coin value.
4.  **Testing & Calibration:**
    -   Test coin insertion and verify the credit updates correctly.
    -   Test each product channel to ensure the correct motor runs for the intended duration.

---

## 🤝 Contributing

We welcome contributions! Please help us improve this project.

-   **Open an Issue** to report bugs, suggest new features, or discuss ideas.
-   **Submit a Pull Request (PR)** for direct contributions:
    -   Keep PRs small and focused on a single change.
    -   Include details like screenshots, logs, or short video clips if applicable.
    -   Use clear commit messages (e.g., `feat: add servo support`, `fix: correct keypad mapping`).

---

## 📄 License

This project is open source and available under the **MIT License**. See the `LICENSE` file for full details. For inquiries regarding alternative licensing, please open an issue to discuss.

---

## 👥 Team

This project was created by:
-   **Aloraini Abdulhakim**
-   **Muhannadhu Musthafa**
-   **Abdelmohsine Smili**
-   **Salma Yasser**
-   **Mohammed Salman**
-   **Aiham Samooh**

> **Special Thanks** to our advisor:
> -   **Ts. Wan Zailah binti Wan Said**
>
> ...and to everyone who provided valuable feedback and support during testing.
