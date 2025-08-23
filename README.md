# Portable Vending Machine

Open-source prototype for a lightweight, coin‑operated portable vending machine with shared CAD, electronics, firmware, and documentation.



## Repo Structure
```
- ``
- `LICENSE`
- `README.md`
- **docs/**
  - **design-book/**
    - `docs/design-book/.gitkeep`
    - `docs/design-book/design book.pdf`
- **firmware/**
  - **arduino/**
    - **Machine_Logic/**
      - `firmware/arduino/Machine_Logic/.gitkeep`
      - `firmware/arduino/Machine_Logic/Machine_Logic.ino`
      - `firmware/arduino/Machine_Logic/README.md`
- **hardware/**
  - **cad/**
    - `hardware/cad/.gitkeep`
    - `hardware/cad/Vending Machine Final Layouts.dwg`
    - `hardware/cad/Vending Machine Layouts.pdf`
  - **electronics/**
    - `hardware/electronics/.gitkeep`
    - `hardware/electronics/Demo BOM.xlsx`
    - `hardware/electronics/Electronics Sch.png`
- **logic/**
  - `logic/.gitkeep`
  - `logic/machine-logic.pdf`
- **media/**
  - `media/.gitkeep`
  - `media/Smart Portable Vending Machine Poster.pdf`
```

## What’s Inside
### Firmware (Arduino)
- Open `firmware/arduino/Machine_Logic/Machine_Logic.ino` in **Arduino IDE 2.x**.
- Select board and port (update below).
- Install libraries listed in `firmware/README.md` (or below).
- **Verify** → **Upload**.


### Electronics
Electronics docs: (add schematic/PCB/Gerbers if available).

### CAD / Mechanics
CAD: (add STEP/STL/DXF exports if available).

### Machine Logic
- **Machine Logic**: `firmware/arduino/Machine_Logic/README.md`, `logic/machine-logic.pdf`

### Docs & Media
- **Design Book**: `docs/design-book/design book.pdf`
- **Poster**: `media/Smart Portable Vending Machine Poster.pdf`

---

## Getting Started

### Hardware
- Assemble according to the CAD and electronics docs.
- Refer to the **BOM** for parts and quantities.

### Firmware
- Open `firmware/arduino/Machine_Logic/Machine_Logic.ino` in **Arduino IDE 2.x**.
- Select board and port (update below).
- Install libraries listed in `firmware/README.md` (or below).
- **Verify** → **Upload**.


> **Board:** (e.g., Arduino Mega/Uno/Nano)  
> **Required libraries:** (list here, e.g., `LiquidCrystal_I2C`, `Keypad` …)  
> **Pin mapping:** Document pins in `firmware/README.md` or below.

### Reproducibility
- CAD: open STEP files in FreeCAD/Fusion/SolidWorks. Use DXF/PDF drawings for fabrication.
- Electronics: use `schematic.pdf`, order parts from `bom.csv`, and (optionally) fab PCB using `gerbers`.
- Logic: see machine-logic diagram; a Mermaid version can be added for easy edits.

---

## Contributing
We welcome issues and pull requests. Please:
- Keep PRs focused and add build/bench photos or logs when relevant.
- Follow a clear commit style (e.g., `feat:`, `fix:`, `docs:`).

If you’re unsure where to start, open an issue with your idea.

## License
MIT — see `LICENSE`.

## Acknowledgments
- Team: 
Aloraini Abdulhakim
Abdelmohsine Smili  
Salma Yasser Abdelsamie Fetouh
Mohammed Salman Jalil 
Muhannadhu Musthafa 
Aiham Samooh Nizaru 
<img width="468" height="133" alt="image" src="https://github.com/user-attachments/assets/f330600f-24c8-4cb0-a4e6-69066fa02d7e" />

- Libraries & tools: Arduino, AutoCAD
