# Chip-8 Emulator

[Intro](Images\Captura de pantalla 2025-05-29 185816.png)

This is a Chip-8 emulator built with C++, SDL3, and ImGui, providing a graphical interface for playing Chip-8 ROMs and debugging their execution.

## Features

*   **Chip-8 Emulation Core**: Accurate emulation of the Chip-8 instruction set.
*   **Graphical User Interface (GUI)**: Powered by ImGui, offering an interactive and user-friendly experience.
*   **ROM Loading**: Easily load Chip-8 ROMs using an integrated file dialog.
*   **Debugging Tools**:
    *   **Register View**: Monitor the state of all V registers, Program Counter (PC), and Index Register (I).
    *   **Timer Display**: View the current values of the Delay Timer and Sound Timer.
    *   **Current Instruction**: See the currently executing instruction in hexadecimal.
    *   **Memory Map**: Inspect a segment of RAM around the Index Register (I).
    *   **Stack View**: Observe the call stack.
    *   **Disassembly View**: (Available in Debug Mode) See the disassembled instructions, with the current PC highlighted. Allows live editing of memory if the emulator is stopped.
*   **Execution Control**:
    *   **Run/Stop**: Toggle continuous execution of the ROM.
    *   **Step (->)**: Execute one instruction at a time when the emulator is stopped.
    *   **Reset**: Restart the currently loaded ROM.
*   **Configurable Quirks**: Adjust specific Chip-8 behaviors to ensure compatibility with various ROMs.
*   **Audio Support**: Emulates the Chip-8's characteristic beep sound.
*   **Keyboard Input**: Map your physical keyboard to the Chip-8's hexadecimal keypad.

## Prerequisites

To build and run this project, you will need:

*   **C++ Compiler**: A C++11 compatible compiler (e.g., GCC, Clang, MSVC).
*   **CMake**: For managing the build process.
*   **SDL3 Development Libraries**: Simple DirectMedia Layer 3.
*   **ImGui**: Dear ImGui (included as a vendor library).
*   **ImGuiFileDialog**: A file dialog for ImGui (included as a vendor library).

## Building the Project

Follow these steps to build the emulator:

1.  **Clone the repository**:
    ```bash
    git clone <repository_url>
    cd <repository_name>
    ```
    (Replace `<repository_url>` and `<repository_name>` with your actual repository details.)

2.  **Create a build directory and configure CMake**:
    ```bash
    mkdir build
    cd build
    cmake ..
    ```

3.  **Build the project**:
    ```bash
    cmake --build .
    ```
    This will compile the source code and create the executable.

## How to Use

1.  **Run the Emulator**:
    Navigate to your build directory and run the generated executable (e.g., `./Chip8Emulator` on Linux/macOS, `Chip8Emulator.exe` on Windows).

2.  **Load a ROM**:
    *   In the emulator window, go to the menu bar at the top.
    *   Click on `Files` -> `Choose File`.
    *   A file dialog will appear. Navigate to your Chip-8 ROMs (typically `.ch8` files) and select one. The emulator will automatically reset and load the selected ROM.

3.  **Emulator Controls**:
    *   **Run / Stop**: Click the `Run` button to start continuous emulation. It will change to `Stop` to pause.
    *   **Step (->)**: When the emulator is `Stop`ped, click `->` to execute the next instruction.
    *   **Reset**: Click `Reset` to reload the current ROM and reset the emulator state.

4.  **Keyboard Input**:
    The emulator maps standard keyboard keys to the Chip-8's hexadecimal keypad. The mapping is displayed in the "Registro" window. For example:
    *   `1`, `2`, `3`, `4` map to Chip-8 `1`, `2`, `3`, `C`
    *   `Q`, `W`, `E`, `R` map to Chip-8 `4`, `5`, `6`, `D`
    *   `A`, `S`, `D`, `F` map to Chip-8 `7`, `8`, `9`, `E`
    *   `Z`, `X`, `C`, `V` map to Chip-8 `A`, `0`, `B`, `F`

5.  **Debug Mode**:
    The `debug_mode` flag in `AppConfig` (currently set to `true` in `main.cpp`) enables the "Asm" window, which shows the disassembly and allows memory editing when the emulator is stopped.


[OpCodes](Images\Captura de pantalla 2026-03-24 181118.png)


## Chip-8 Quirks

Chip-8 emulation can vary slightly between different implementations, leading to "quirks" that affect how certain instructions behave. This emulator allows you to toggle these quirks:

*   **SHIFT [0x8XY6 and 0x8XY7]**:
    *   **Enabled**: The `8XY6` (shift right) and `8XY7` (shift left) instructions affect `VX` and store the least/most significant bit in `VF`.
    *   **Disabled**: These instructions affect `VY` and store the least/most significant bit in `VF`.
*   **JUMP [BNNN]**:
    *   **Enabled**: The `BNNN` instruction jumps to `NNN + V0`.
    *   **Disabled**: The `BNNN` instruction jumps to `NNN + VX` (where X is the first nibble of the instruction).
*   **MEM QUIRK [Modify I]**:
    *   **Enabled**: The `FX55` (store registers) and `FX65` (read registers) instructions increment the `I` register while writing/reading.
    *   **Disabled**: The `I` register remains unchanged after these operations.

To change a quirk:
1.  Go to the menu bar and click `Quirks`.
2.  Click the button next to the quirk you want to toggle. The text color will indicate its state (Green for Enabled, Red for Disabled).
3.  **Note**: Changing a quirk will automatically reset and reload the currently selected ROM to apply the new behavior.

## Important Notes

*   **Velocity Setting**: The `velocity` variable in `AppConfig` controls the number of instructions executed per GUI frame. A higher velocity makes the emulator run faster but might affect the accuracy of the debug display, as it updates less frequently relative to instruction execution.
*   **Audio**: The emulator initializes SDL audio to play the Chip-8 beep. Ensure your system's audio is configured correctly.
*   **Error Handling**: Basic error handling is in place for SDL and Chip-8 initialization. Check the console output for any error messages.

---
