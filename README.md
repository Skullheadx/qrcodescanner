# QR Code Toolkit: Generator & Scanner Interface

## 🚀 Overview

Welcome to the QR Code Toolkit! 

1.  **A C++ QR Code Generator**: Crafted from scratch, this tool takes numeric data and transforms it into a Version 1 QR code, saving it as a PBM (Portable BitMap) image. See it in action in [`qrgenerator.cpp`](qrgenerator.cpp).
2.  **STM32 Camera Interface**: Code designed for an STM32 microcontroller to interface with a camera module (like the OV7670). This lays the groundwork for a potential QR code scanning application. Check out the setup in [`main.cpp`](main.cpp).

## ✨ QR Code Generator (`qrgenerator.cpp`)

This command-line program generates Version 1 QR codes, specifically for numeric input data with 'Medium' error correction.

### Core Features:

- **Numeric Data Encoding**: Converts your input numbers (e.g., "123456789") into the compact binary format required for QR codes.
- **Robust Error Correction**: Implements Reed-Solomon error correction. For Version 1-M, it generates 10 error correction codewords for 16 data codewords, allowing the QR code to be read even if partially damaged.
- **Standard QR Structure Generation**:
  - **Finder Patterns**: The iconic squares at three corners of the QR code.
  - **Timing Patterns**: Alternating black and white modules that help decoders determine module coordinates.
  - **Data & ECC Placement**: Meticulously places data and error correction codewords throughout the QR grid according to official specifications.
- **Smart Masking**: Applies all 8 standard mask patterns to the QR grid. It then evaluates them (using logic in [`evaluate_symbol`](qrgenerator.cpp)) to select the pattern that ensures the best readability.
- **Format Information**: Encodes crucial details like the error correction level ('M') and the chosen mask pattern ID into the QR code. This information is also BCH encoded for its own protection.
- **PBM Image Output**: Saves the final QR code as a simple, uncompressed `.pbm` image file, viewable with most image viewers. See [`create_image`](qrgenerator.cpp). An example output is [`6969696969.pbm`](6969696969.pbm).

### How It Works: A Peek Under the Hood ⚙️

The generator follows a precise sequence to build your QR code:

1.  **Input & Initialization**:
    - Takes a numeric string as input (e.g., a 9-digit ID from [`id_generator`](qrgenerator.cpp)).
    - Prepares the data with a mode indicator (`0001` for numeric) and a character count indicator.
2.  **Bitstream Assembly**:
    - Converts the data to a binary stream.
    - Adds a terminator sequence (`0000`).
    - Pads the stream to ensure it's a multiple of 8 bits.
    - Adds specific padding codewords ([`pad_codeword1`](qrgenerator.cpp), [`pad_codeword2`](qrgenerator.cpp)) to reach the 16 data codewords required for Version 1-M.
3.  **Error Correction Magic**:
    - The data codewords are treated as coefficients of a data polynomial.
    - A generator polynomial is used with Galois Field (GF(256)) arithmetic (leveraging [`log_table`](qrgenerator.cpp) and [`antilog_table`](qrgenerator.cpp)) to compute Reed-Solomon error correction codewords.
4.  **Building the Matrix (21x21 for Version 1)**:
    - The combined data and error correction codewords are strategically placed within the QR code grid.
    - Essential structural elements like finder patterns and timing patterns are drawn onto the grid.
5.  **Optimizing for Readability (Masking)**:
    - The grid is tested against 8 different mask patterns. The "best" pattern (minimizing penalties) is chosen.
    - Format information (error correction level + mask pattern ID) is generated, BCH encoded, and embedded in designated areas of the QR code.
    - A mandatory dark module is added.
6.  **Saving Your QR Code**:
    - The final binary grid is converted into a PBM image by the [`create_image`](qrgenerator.cpp) function.

### Usage

To generate a QR code:

1.  Compile [`qrgenerator.cpp`](qrgenerator.cpp) using a C++ compiler (e.g., g++).
2.  Run the compiled executable.
    - It will typically generate a QR code for a randomly generated 9-digit number (thanks to [`id_generator`](qrgenerator.cpp)) or a hardcoded one if you modify the source.
    - The output will be a `.pbm` file named after the input data (e.g., `123456789.pbm`).

## 📸 Camera Interface / QR Scanner Foundation (`main.cpp`)

The [`main.cpp`](main.cpp) file shifts focus to hardware, specifically setting up an STM32 microcontroller. 

- **Hardware Configuration**:
  - **GPIO Setup**: Configures General Purpose Input/Output pins for:
    - A parallel camera interface (D0-D7 data lines, HREF, VSYNC, DCLK).
    - I2C communication (SDA, SCL), often used to configure camera sensors like the OV7670.
  - **Peripheral Initialization**: Sets up I2C and DMA ([`DMA_Init`](main.cpp)) controllers on the STM32.
- **Intended Purpose**: To capture images using a camera module connected to the STM32, process these images, and decode QR codes. The [`camera.cpp`](camera.cpp) file might contain further logic for this.

This component is distinct from the generator and would run on the embedded STM32 platform.


## 🔮 Future Directions & Potential Enhancements

This project has a solid foundation with many exciting possibilities:

- **Full Scanner Implementation**: Develop the STM32-based QR scanner to capture and decode QR codes in real-time.
- **Generator Upgrades**:
  - Support for higher QR code versions (larger grids, more data).
  - Implement other error correction levels (L, Q, H).
  - Add encoding for Alphanumeric, Byte, and Kanji data types.
- **Integration**: Potentially link the generator and scanner components, perhaps for testing or a closed-loop system.

---

