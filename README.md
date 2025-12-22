# README — CSE322 Door Lock System

## Project

**CSE322 — Door Lock System**
A course project implementing an electronic door-lock system. The repository contains firmware/software for two ECUs (Control and HMI) and supporting project documentation (project description and final report). ([GitHub][1])

---

# Table of contents

1. [Overview](#overview)
2. [Repository structure](#repository-structure)
3. [Features](#features)
4. [Requirements](#requirements)
5. [Build & run (guidance)](#build--run-guidance)
6. [Documentation & reports](#documentation--reports)
7. [Contributing](#contributing)
8. [Contact](#contact)

---

# Overview

This repository contains the implementation and documentation of a door-lock system developed as a course project. The design is split into two main ECUs: the Control_ECU (handles lock logic and actuators) and the HMI_ECU (handles user input / display). The repository primarily contains C source code and associated project files. ([GitHub][1])

---

# Repository structure

A summary of the top-level items (as visible in the repository):

* `Control_ECU/` — Firmware and source for the control ECU (lock controller). ([GitHub][2])
* `HMI_ECU/` — Firmware and source for the human–machine interface ECU (keypad/display). ([GitHub][3])
* `CSE211-CSE322 Project Description.pdf` — Course project description and requirements. ([GitHub][4])
* `Team25 - Door Lock System Report.pdf` — Final project report (design, components, testing, results). ([GitHub][5])
* `.vscode/`, `.gitignore` and other project metadata. ([GitHub][1])

Languages detected for the repository: **C** (primary), with some Batchfile / PowerShell helper scripts. ([GitHub][1])

---

# Features (high-level)

* Separated architecture with Control and HMI ECUs. ([GitHub][2])
* Project documentation including project specification and full team report. ([GitHub][4])

> Note: For exact feature details (e.g., supported commands, protocols between ECUs, pinouts), consult the team report PDF in the repo. ([GitHub][5])

---

# Requirements

The project appears to be embedded firmware written in C. Typical requirements for building and running the code will include:

* ARM Cortex-M toolchain or equivalent (e.g., GCC Arm Embedded toolchain, Keil uVision, TI Code Composer) depending on the target board.
* Appropriate programmer / debugger (JTAG/SWD) for flashing the microcontroller.
* Hardware components described in the project report (actuators, keypad, displays, wiring). See the report for the exact BOM and wiring. ([GitHub][5])

---

# Build & run — guidance

Because the repository does not include a top-level README, exact build instructions are not present in the repo view. Use the following as a starting checklist and adapt to the specific MCU and IDE used in the project:

1. **Clone the repository**

```bash
git clone https://github.com/ferasahmed285/CSE322-Door-Lock-System.git
cd CSE322-Door-Lock-System
```

2. **Inspect each ECU folder**

* Open `Control_ECU/` and `HMI_ECU/` in your IDE of choice or inspect makefiles/project files to see the required toolchain. ([GitHub][2])

3. **Install toolchain**

* If the project targets an ARM microcontroller, install an ARM GCC toolchain or vendor IDE (Keil/TI/Atollic/PlatformIO as appropriate).

4. **Build**

* Use the provided project files or makefiles in the ECU folders. If no makefile is present, create one or use your IDE to compile the C sources.

5. **Flash**

* Connect the development board (or target hardware) and flash the built binary using your debugger/programmer.

6. **Test**

* Follow the test procedures and scenarios in the final report to validate system behavior. ([GitHub][5])

---

# Documentation & reports

Two PDFs are present in the repo and are the authoritative source for design, test scenarios, and BOM:

* **CSE211-CSE322 Project Description.pdf** — project brief and grading requirements. ([GitHub][4])
* **Team25 - Door Lock System Report.pdf** — the team’s final report covering architecture, component lists, schematics, test results, and usage instructions. ([GitHub][5])

Refer to those documents for exact wiring diagrams, pin assignments, protocol between ECUs, and test cases.

---

# Contributing

This repository appears to be a course submission. If you want to contribute or update the README:

1. Fork the repository.
2. Create a branch `docs/readme` or similar.
3. Update this README with exact build commands and target board details (e.g., MCU model, IDE, flash utility).
4. Create a pull request describing your changes.

---

# Contact / Authors

Repository contributors (as shown): `ferasahmed285 (Ferass Ahmed)` and `mohamed242006`. ([GitHub][1])

---

[1]: https://github.com/ferasahmed285/CSE322-Door-Lock-System "GitHub - ferasahmed285/CSE322-Door-Lock-System"
[2]: https://github.com/ferasahmed285/CSE322-Door-Lock-System/tree/main/Control_ECU "CSE322-Door-Lock-System/Control_ECU at main · ferasahmed285/CSE322-Door-Lock-System · GitHub"
[3]: https://github.com/ferasahmed285/CSE322-Door-Lock-System/tree/main/HMI_ECU "CSE322-Door-Lock-System/HMI_ECU at main · ferasahmed285/CSE322-Door-Lock-System · GitHub"
[4]: https://github.com/ferasahmed285/CSE322-Door-Lock-System/blob/main/CSE211-CSE322%20Project%20Description.pdf "CSE322-Door-Lock-System/CSE211-CSE322 Project Description.pdf at main · ferasahmed285/CSE322-Door-Lock-System · GitHub"
[5]: https://github.com/ferasahmed285/CSE322-Door-Lock-System/blob/main/Team25%20-%20Door%20Lock%20System%20Report.pdf "CSE322-Door-Lock-System/Team25 - Door Lock System Report.pdf at main · ferasahmed285/CSE322-Door-Lock-System · GitHub"
