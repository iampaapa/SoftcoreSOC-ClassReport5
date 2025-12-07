# Class Report 5: Thermometer

This repo contains the source code and instructions to replicate the **Digital Thermometer Experiment** (Exp 17.8.1).

**Demo Video:** [https://www.youtube.com/watch?v=bncg9qyTfBg](https://www.youtube.com/watch?v=bncg9qyTfBg)

## Directory Structure

  * **`hw/`**: Contains SystemVerilog source code (`hdl/`) and constraints (`constraints/`) for the FPGA.
  * **`sw/`**: Contains the C++ drivers (`drivers/`) and the main application code (`app/`) for the MicroBlaze processor.

-----

## Build Instructions

### 1: Hardware Build (Vivado)

1.  Open **Vivado** and create a new **RTL Project**.
2.  **Add Sources:**
      * Add the entire `hw/hdl` directory.
      * Add the constraint file `hw/constraints/Nexys4_DDR_chu.xdc`.
3.  **Configure MicroBlaze MCS IP:**
      * Open the **IP Catalog** and search for "MicroBlaze MCS".
      * Double-click to customize:
          * **Component Name:** `cpu`.
          * **Memory Size:** `128KB`.
          * **Input Clock:** `100` MHz.
          * **Interface:** Check **Enable I/O Bus**.
      * Click **Generate**.
4.  **Set Top Module:**
      * Find and set `sys/top/mcs_top_sampler.sv` as Top.
5.  **Generate Hardware:**
      * Click **Generate Bitstream**.
      * Once complete, go to **File -\> Export -\> Export Hardware**.
      * Check **Include Bitstream** and save the `.xsa` file.

### 2: Software Build (Vitis)

1.  Open **Vitis** and select a workspace.
2.  **Create Platform:**
      * **File -\> New -\> Platform Project**.
      * Create a platform from the `.xsa` file exported in 1.
      * Build the platform project.
3.  **Create Application:**
      * **File -\> New -\> Application Project**.
      * Select the platform created above.
4.  **Import Source Code:**
      * Right-click the `src` folder in your application project -\> **Import** -\> **File System**.
      * Import all files from this repo's `sw/drivers/` folder.
      * Import `sw/app/thermometer.cpp`.
5.  **Build Application:**
      * Build the project (Hammer icon).

### 3: Final Integration (Vivado)

To prevent memory mapping errors (which was happening when I was using Vitis directly), I embedded the software directly into the FPGA bitstream.

1.  Come back to **Vivado**.
2.  **Add the ELF:**
      * **Add Sources** -\> **Add or create design sources**.
      * Navigate to your Vitis workspace and select the generated `.elf` file (e.g., `thermometer_app.elf`).
3.  **Associate ELF:**
      * Go to **Tools -\> Associate ELF Files**.
      * Find the processor instance (`system_i/cpu`).
      * Click the file field under **Design Sources** and select your `.elf` file.
      * Ensure it is applied to the **Implementation** state.
4.  **Re-Generate Bitstream:**
      * Click **Generate Bitstream** again. Vivado will now merge the C++ program into the Block RAM initialization vectors.
5.  **Program Device:**
      * Open **Hardware Manager**, connect to the board, and program with the final `.bit` file.

-----

## Running the Project
1.  Connect the Nexys 4 DDR board via USB and ensure it is powered ON.
2.  **Check the 7-Segment Display:**
      * It should display the current ambient temperature in Celsius (e.g., 24.50C).
3.  **Toggle Units:**
      * Flip Switch 0 (SW0) UP.
      * The display should change to Fahrenheit (e.g., 76.10F).

If you place a hotter or colder object (e.g., your finger or some ice) on the ADT7420 sensor (labeled IC5, near the DDR memory), the temperature reading will rise  or fall on the display.

-----

### **Running the Project using the Pre-built Binaries**

To actually run the project without rebuilding the entire hardware and software stack, you can use the pre-compiled artifacts located in the `bin/` directory.

**Files Included:**
* `mcs_top_sampler.bit`: The complete FPGA bitstream with the C++ application already embedded.
* `project_thermometer.xsa`: The hardware specification file (for Vitis).
* `thermometer_app.elf`: The compiled software executable.

**Steps to use:**
1.  Connect your **Nexys 4 DDR** board via USB and turn it ON.
2.  Open **Vivado**.
3.  Click **Open Hardware Manager** under **Tasks**
4.  Click **Open Target** -> **Auto Connect**.
5.  Click **Program Device**.
6.  Browse to and select `bin/mcs_top_sampler.bit`.
7.  Click **Program**.

**Expected Results:**
* The 7-segment display should show the current temperature (e.g., `24.12 C`).
* Flipping **Switch 0 (SW0)** toggles the unit between Celsius (`C`) and Fahrenheit (`F`).
* Touching the **ADT7420** sensor (IC5) causes the temperature reading to rise.
