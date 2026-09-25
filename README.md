# stm32-freertos-fw
FreeRTOS-based sensor processing and UART telemetry firmware for STM32 Nucleo boards.

---

# Phase 1 Documentation: Hardware Setup, Firmware & Telemetry Bridge

## 1. Project Goal

The primary objective of this project is to build an end-to-end **Embedded Gateway** that connects a real-time microcontroller (**STM32F446RE**) running FreeRTOS with an **Embedded Linux Gateway (Raspberry Pi 3)** running Yocto Linux.

In Phase 1, the goal was to set up the toolchains, cross-compilation pipeline, low-level hardware drivers, linker scripts, and flash procedure to establish a stable physical UART serial telemetry bridge between the STM32 and the Raspberry Pi.

---

## 2. Hardware Setup

### Hardware Components Used

* **Target Microcontroller:** STM32F446RE Nucleo-64 Board (ARM Cortex-M4 @ 16 MHz HSI default, up to 180 MHz)
* **Embedded Gateway:** Raspberry Pi 3 (Running custom Yocto/Linux rootfs with OpenOCD and `sensor-gateway-app`)
* **Development Workstation:** Ubuntu Linux Host VM (Cross-compilation, build system, NFS server)
* **Interface / Debugger:** On-board ST-LINK/V2-1 via USB cable

### Physical & Interconnect Architecture

1. **ST-LINK USB Connection:** The STM32 Nucleo board connects via USB to the Raspberry Pi.
2. **Virtual COM Port (VCP):** The ST-LINK on-board debugger bridges the STM32's **USART2 (PA2 = TX, PA3 = RX)** directly to the Pi’s USB port, enumerating as `/dev/ttyACM0` on the Raspberry Pi.
3. **NFS Shared File System:** The Ubuntu Host VM exports a rootfs directory over NFS (`/srv/nfs/rpi-rootfs/root/`), allowing instant binary file sharing with the Pi without manually copying over network protocols each time.

---

## 3. How We Did It (Step-by-Step Resolution)

1. **Bare-Metal & FreeRTOS Startup Configuration:**
* Resolved CMSIS header missing definitions by directly mapping the Coprocessor Access Control Register (`SCB_CPACR` at `0xE000ED88`) to enable the Cortex-M4 Floating Point Unit (FPU).
* Mapped the hardware interrupt vector table (`startup_stm32f446xx.s`) to standard system handlers (`SysTick_Handler`, `Reset_Handler`).


2. **Linker Script & Toolchain Fixes:**
* Fixed GCC/Newlib `_sbrk` dynamic heap allocation errors by defining explicit `end` and `_end` memory symbols aligned at 8-byte boundaries in `STM32F446RETx_FLASH.ld`.
* Implemented minimal POSIX system stubs (`_read`, `_write`, `_lseek`, `_close`, `_fstat`, `_isatty`, `_kill`, `_getpid`) to resolve Newlib non-hosted C library linker warnings.


3. **Direct Hardware UART Engine:**
* Configured `RCC` registers (`RCC_AHB1ENR` for GPIOA clock, `RCC_APB1ENR` for USART2 clock).
* Set `PA2` pin mode to Alternate Function 7 (`AF7`).
* Programmed `USART2_BRR` baud rate register for **115200 baud** (at 16 MHz HSI clock).
* Enabled hardware transmit routines (`USART2_CR1`) to broadcast data frames across `/dev/ttyACM0`.


4. **Flashing & Binary Verification:**
* Identified and fixed an issue where OpenOCD was flashing stale `.bin` files instead of newly generated `.elf` binaries.
* Flashed `.elf` binaries directly so OpenOCD automatically handled memory sector mapping to Flash address `0x08000000`.



---

## 4. What Is Needed vs. What Is NOT Needed

### What IS Needed

* **ARM Cross Toolchain:** `arm-none-eabi-gcc` on the Host VM to cross-compile Cortex-M4 binaries.
* **OpenOCD on Gateway (Raspberry Pi):** To flash the STM32 directly over ST-LINK SWD using `interface/stlink.cfg` and `target/stm32f4x.cfg`.
* **Linker Script (`.ld`):** Memory map declaring Flash (`0x08000000`), RAM (`0x20000000`), vector tables, and heap symbols (`end`).
* **System Call Stubs:** Bare-metal overrides for POSIX functions required by standard C libraries.
* **ELF File Format:** Preserves symbol table information and flash offset section addresses for OpenOCD.

### What IS NOT Needed

* **Heavy CMSIS Header Packages (For Basic Telemetry):** Registers can be manipulated directly via memory-mapped addresses (`0x4002...` / `0x4000...`) when full CMSIS dependencies are absent.
* **Manual Offset Flashing for ELF Files:** You do **not** need to pass raw hex memory addresses (like `0x08000000`) in OpenOCD when flashing `.elf` files, as section addresses are embedded inside the ELF structure.
* **GNU Coreutils on Embedded BusyBox:** Commands like `ls --time-style` do not exist in BusyBox; standard POSIX commands (`ls -l`, `stat`) should be used instead.

---

## 5. Useful Commands Reference

### On Ubuntu Host VM (Compilation & NFS Transfer)

* **Build Firmware:**
```bash
cd ~/Desktop/YoctoProject/embedded-gateway/stm32-freertos-fw/build
make clean
make -j$(nproc)

```


* **Copy Binary to NFS Share:**
```bash
cp stm32-freertos-fw.elf /srv/nfs/rpi-rootfs/root/

```


* **Verify ELF File Entry Point & Headers:**
```bash
arm-none-eabi-readelf -h stm32-freertos-fw.elf

```



---

### On Raspberry Pi (Flashing & Testing)

* **Check File Modification Time on BusyBox:**
```bash
ls -l /root/stm32-freertos-fw.elf
# or
stat /root/stm32-freertos-fw.elf

```


* **Flash Firmware to STM32 Target:**
```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program /root/stm32-freertos-fw.elf verify reset exit"

```


* **Read Serial Telemetry Directly:**
```bash
cat /dev/ttyACM0

```


* **Launch Gateway App:**
```bash
sensor-gateway-app

```
