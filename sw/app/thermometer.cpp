#include "chu_init.h"
#include "gpio_cores.h"
#include "i2c_core.h"
#include "sseg_core.h"
#include "math.h"

// the ADT7420 on the Nexys 4 DDR is hardwired to address 0x4B
const uint8_t ADT7420_ADDR = 0x4B;
const uint8_t REG_TEMP_MSB = 0x00;

// for reading 13-bit Temperature from ADT7420
bool read_temp_raw(I2cCore *i2c, float &temp_c) {
    uint8_t raw_data[2];
    uint8_t reg_addr = REG_TEMP_MSB;

    int ack = i2c->write_transaction(ADT7420_ADDR, &reg_addr, 1, 1);
    ack += i2c->read_transaction(ADT7420_ADDR, raw_data, 2, 0);

    if (ack == 0) {
        uint16_t temp_raw = (uint16_t)((raw_data[0] << 8) | raw_data[1]);
        
        int16_t signed_raw = (int16_t)temp_raw;
        
        // resolution: 128 LSBs per 1 degree Celsius (1/128 = 0.0078125)
        temp_c = (float)signed_raw / 128.0;
        return true;
    }
    return false;
}

// the format for display is " XX.YY U " (Where U is Unit C/F)
void display_temp_sseg(SsegCore *sseg, float temp, bool is_fahrenheit) {
    int val = (int)(temp * 100); 

    int hunds = abs(val % 10);
    val /= 10;
    int tenths = abs(val % 10);
    val /= 10;
    int ones = abs(val % 10);
    val /= 10;
    int tens = abs(val % 10);
    
    uint8_t unit_ptn = is_fahrenheit ? sseg->h2s(15) : sseg->h2s(12);

    sseg->write_1ptn(unit_ptn, 0);
    
    sseg->write_1ptn(sseg->h2s(hunds), 1);
    sseg->write_1ptn(sseg->h2s(tenths), 2);
    sseg->write_1ptn(sseg->h2s(ones), 3);
    
    if (tens == 0 && val == 0) {
        sseg->write_1ptn(0xFF, 4);
    } else {
        sseg->write_1ptn(sseg->h2s(tens), 4);
    }

    for (int i = 5; i < 8; i++) {
        sseg->write_1ptn(0xFF, i);
    }

    sseg->set_dp(0x08); 
}

int main() {
    // 1. instantiate Cores
    GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
    GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));
    SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));
    I2cCore adt7420(get_slot_addr(BRIDGE_BASE, S10_I2C));

    // 2. setup
    adt7420.set_freq(400000);

    float temp_c, temp_display;
    bool is_f;

    while (1) {
        // 3. read temp
        if (read_temp_raw(&adt7420, temp_c)) {
            // 4. check SW0 to determine either C or F
            if (sw.read() & 0x01) {
                is_f = true;
                temp_display = temp_c * 1.8 + 32.0;
            } else {
                is_f = false;
                temp_display = temp_c;
            }
            display_temp_sseg(&sseg, temp_display, is_f);
        } 
        else {
            // show "Err" on the sseg
            sseg.write_1ptn(sseg.h2s(14), 2);
            sseg.write_1ptn(0xFF, 1);
            sseg.write_1ptn(0xFF, 0);
        }
        display_temp_sseg(&sseg, temp_display, is_f);
        sleep_ms(200);
    }
    return 0;
}