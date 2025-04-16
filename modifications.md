# Changes to run TFLITE-MICRO Model

This document outlines the necessary modifications to integrate and run TF-LITE-MICRO models with LSTM/FC capabilities within the Zephyr RTOS environment and to expand M7 flash memory for STM32H747I-DISCO(optional). While this repository includes these modifications, users who installed Zephyr via the README.md must apply these changes manually.

## 1. Enabling LSTM/FC Support in CMSIS-NN

To enable LSTM/FC functionality, the `CMakeLists.txt` file of the CMSIS-NN module needs to be modified.

**Location:** `zephyrproject/zephyr/modules/cmsis-nn/CMakeLists.txt`

**Modifications:**

-   **Update `CONFIG_CMSIS_NN_LSTM`:**

    Replace the original conditional block with the following:

    ```cmake
    if(CONFIG_CMSIS_NN_LSTM)
      file(GLOB SRC_8 "${CMSIS_NN_DIR}/Source/LSTMFunctions/*_s8.c")
      file(GLOB SRC_16 "${CMSIS_NN_DIR}/Source/LSTMFunctions/*_s16.c")
      zephyr_library_sources(${SRC_8} ${SRC_16})
    endif()
    ```

    
-   **Update `CONFIG_CMSIS_NN_FULLYCONNECTED`:**

    Replace the original conditional block with the following:

    ```cmake
    if(CONFIG_CMSIS_NN_FULLYCONNECTED)
      file(GLOB SRC_S4 "${CMSIS_NN_DIR}/Source/FullyConnectedFunctions/*_s4.c")
      file(GLOB SRC_S8 "${CMSIS_NN_DIR}/Source/FullyConnectedFunctions/*_s8.c")
      file(GLOB SRC_S16 "${CMSIS_NN_DIR}/Source/FullyConnectedFunctions/*_s16*.c")
      file(GLOB SRC_S64 "${CMSIS_NN_DIR}/Source/FullyConnectedFunctions/*_s64*.c")
      zephyr_library_sources(${SRC_S4} ${SRC_S8} ${SRC_S16} ${SRC_S64})
    endif()
    ```


## 2. Expanding M7 Flash Memory on STM32H747I-DISCO (Optional)

This section details how to expand the M7 core's flash memory from 1024KB to 1984KB on the STM32H747I-DISCO board. This step is only needed if you need to run a model of size more than 1 MB. 

**Location:** `zephyrproject/zephyr/dts/arm/st/h7/stm32h747Xi_m7.dtsi`

**Modifications:**

To expand the flash memory of M7 and shrink M4, replace the original content with the following:

```
/*
 * Copyright (c) 2019 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <mem.h>
#include <st/h7/stm32h747.dtsi>

/ {
    cpus {
        /delete-node/ cpu@1;
    };

    soc {
        flash-controller@52002000 {
            flash0: flash@8000000 {
                reg = <0x08000000 DT_SIZE_K(1984)>;
            };
            flash1: flash@8100000 {
                reg = <0x08100000 DT_SIZE_K(64)>;
                compatible = "st,stm32-nv-flash", "soc-nv-flash";
                write-block-size = < 0x20 >;
                erase-block-size = < 0x20000 >;
                max-erase-time = < 0xfa0 >;
            };
        };

        mailbox@58026400 {
            interrupts = <125 0>;
        };
    };
};
```
Or n the terminal, run the following:
```
STM32_Programmer_CLI -c port=SWD \        
  -w ./build/zephyr/zephyr.bin 0x08000000 \
  -rdu \
  -ob BOOT_CM4_ADD0=0x81f ```
