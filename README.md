# Zephyr Deployment on macOS (nRF/STM + TFLite-Micro)

This document provides a comprehensive guide for setting up and deploying Zephyr RTOS on a macOS environment, supporting nRF52840, nRF5340, STM boards, and TFLite-Micro.

This guide aligns with the official Zephyr documentation: [Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html).

## 🚀 Prerequisites

### 1. Install Homebrew & Dependencies

Open **Terminal** and execute the following commands sequentially:

1.  Install Homebrew:

    ```bash
    /bin/bash -c "$(curl -fsSL [https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh](https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh))"
    ```

2.  Follow the on-screen instructions, which may involve adding Homebrew to your shell configuration:

    ```bash
    echo >> /Users/kainat.altaf/.zprofile
    echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> /Users/kainat.altaf/.zprofile
    eval "$(/opt/homebrew/bin/brew shellenv)"
    brew help
    ```

3.  Install required packages:

    ```bash
    brew install cmake ninja gperf python3 python-tk ccache qemu dtc libmagic wget openocd nrfutil picocom
    echo 'export PATH="/opt/homebrew/opt/ccache/libexec:$PATH"' >> ~/.bashrc
    source ~/.bashrc
    ```

4.  Update Python path in your shell profile:

    ```bash
    (echo; echo 'export PATH="'$(brew --prefix)'/opt/python/libexec/bin:$PATH"') >> ~/.zprofile
    source ~/.zprofile
    ```

### 2. Get Zephyr and Install Python Dependencies

1.  Create and activate a Python virtual environment:

    ```bash
    python3 -m venv ~/zephyrproject/.venv
    source ~/zephyrproject/.venv/bin/activate
    ```

    **Note:** Activate this virtual environment every time you work with Zephyr.

2.  Install West and initialize the Zephyr project:

    ```bash
    pip install west
    west init ~/zephyrproject
    cd ~/zephyrproject
    west update
    west zephyr-export
    west packages pip --install
    ```

3.  Install the Zephyr SDK:

    ```bash
    cd zephyr
    west sdk install
    ```

4.  Verify board support:

    ```bash
    west boards
    ```

### 3. Install Segger J-Link Software and nRF5x Command-Line Tools

1.  Refer to the official Zephyr documentation for Segger J-Link setup: [Nordic Segger Setup](https://docs.zephyrproject.org/latest/develop/flash_debug/nordic_segger.html).

2.  Install the nRF5x Command-Line Tools:

    ```bash
    brew install --cask nordic-nrf-command-line-tools
    ```

### 4. Run Your Code

1.  Build the blinky sample:

    ```bash
    west build -p always -b <your-board-name> samples/basic/blinky
    # Example: west build -p always -b nrf52840dk/nrf52840 samples/basic/blinky
    ```

2.  Flash the built binary to your board:

    ```bash
    west flash
    ```

For TFLite-Micro examples, refer to the official documentation: [TFLite-Micro Hello World](https://docs.zephyrproject.org/latest/samples/modules/tflite-micro/hello_world/README.html#tflite-hello-world). Alternatively, use the following commands:

    ```bash
    west config manifest.project-filter -- +tflite-micro
    west update
    west build -p always -b nrf52840dk/nrf52840 samples/modules/tflite-micro/hello_world
    west flash
    ```

or for nrf5340.

    ```bash
    west build -p auto -b nrf5340dk/nrf5340/cpuapp samples/modules/tflite-micro/hello_world
    west flash
    ```

### 5. View Terminal Outputs

Open a new terminal and execute the following commands:

1.  **For nRF Boards:**

    ```bash
    ls /dev/tty.usbmodem*
    # Example output: /dev/tty.usbmodem0006838451071
    picocom -b 115200 /dev/tty.usbmodem0006838451071
    ```

2.  **For STM Boards:**

    1.  Install STM32CubeProgrammer: [STM32CubeProgrammer Download](https://www.st.com/en/development-tools/stm32cubeprog.html).

    2.  Verify the installation and update your PATH if necessary:

        ```bash
        which STM32_Programmer_CLI
        # If empty or error, run:
        export PATH=/Applications/STMicroelectronics/STM32Cube/STM32CubeProgrammer/STM32CubeProgrammer.app/Contents/MacOs/bin:$PATH
        source ~/.bashrc
        which STM32_Programmer_CLI
        ```

    3.  Connect to the serial port:

        ```bash
        ls /dev/tty.usbmodem*
        # Example output: /dev/tty.usbmodem11103
        picocom -b 115200 /dev/tty.usbmodem11103
        ```

    4. Build and flash the TFLite-Micro hello world example.

        ```bash
        west build -p auto -b stm32h747i_disco/stm32h747xx/m7 samples/modules/tflite-micro/hello_world
        west flash
        ```
### 6. Necessary modifications to run models with Fully Connected and LSTM layers
Follow the steps mentioned in **modifications.md**.
