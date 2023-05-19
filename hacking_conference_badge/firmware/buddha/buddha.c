
/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ws2812.pio.h"

// Define key maps for audio
#define AUDIO_EN 1
#define AUDIO_PIN 0
volatile int period = 1000;
#define SYS_CLOCK_KHZ 133000

// Define key maps for knobs
#define KNOB_1 29
#define KNOB_1_ADC 3
#define KNOB_2 28
#define KNOB_2_ADC 2
#define KNOB_3 27
#define KNOB_3_ADC 1
#define KNOB_4 26
#define KNOB_4_ADC 0

//Define LED I2C info
#define LED_I2C_SDA 20
#define LED_I2C_SCL 21
#define LED_DRIVER_ADDR 0x78 //01111000 //since AD connected to GND
//const int I2C_addr = 0x78;

// Define key maps for key matrix
int rows[] = {8, 11};
#define rowCount sizeof(rows)/sizeof(rows[0])
int cols[] = {5, 6, 3, 12};
#define  colCount sizeof(cols)/sizeof(cols[0])
int keys[rowCount][colCount];

// Read state of keys simply (Do better: https://www.dairequinlan.com/2021/01/the-keyboard-part-4-firmware/)
void scan_keys() {
  for(int c = 0; c < colCount; c++) {
    gpio_set_dir(cols[c], GPIO_OUT);
    gpio_put(cols[c], 1);

    for(int r = 0; r < rowCount; r++) {
      keys[r][c] = gpio_get(rows[r]);    
      printf("Key %d, %d state : %d.\n",r, c, keys[r][c]); 
    }
    gpio_set_dir(cols[c], GPIO_IN);
  }
}


//WORKING
void read_knobs() {
  adc_select_input(KNOB_1_ADC);
  uint16_t knob1 = adc_read();
  adc_select_input(KNOB_2_ADC);
  uint16_t knob2 = adc_read();
  adc_select_input(KNOB_3_ADC);
  uint16_t knob3 = adc_read();
  adc_select_input(KNOB_4_ADC);
  uint16_t knob4 = adc_read();
  printf("Knob 1:%d, Knob 2:%d, Knob 3:%d, Knob 4:%d\n", knob1, knob2, knob3, knob4);
}

// Waveform generator
void on_pwm_wrap() {
  const static bool sawtooth = true;
  static uint32_t i = 0;
  pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN));
  i++;
  uint16_t level = sawtooth
    ? (i % period) * (3016/(period-1))       // sawtooth
    : (i % period) >= (period/2) ? 65535 : 0; // square
  pwm_set_gpio_level(AUDIO_PIN, level);
}

// Create a terrible piano with keys. Do better: https://github.com/blakelivingston/DuinoTune/tree/master/src/tinytune
void keys_to_freq() {
  int count = 0;
  adc_select_input(0);
  uint16_t knob1 = adc_read();
  for(int c = 0; c < colCount; c++) {
    for(int r = 0; r < rowCount; r++) {
     if (keys[r][c] == 1){
      count = count + (knob1 >> 9) * r + 10 * c;
     }
    }
  }
  period = 200 - count;
  if (keys[1][3] == 1) {
    gpio_put(AUDIO_EN, 1);
  } else {
    gpio_put(AUDIO_EN, 0);
  }
}

// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx
bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

void setup_audio(){
    // Set up Audio
    gpio_init(AUDIO_EN);
    gpio_set_dir(AUDIO_EN, GPIO_OUT);
    set_sys_clock_khz(SYS_CLOCK_KHZ, true);
    
    int audio_pin_slice = pwm_gpio_to_slice_num(AUDIO_PIN);

    gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(AUDIO_PIN);

    pwm_clear_irq(slice_num);
    pwm_set_irq_enabled(slice_num, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, on_pwm_wrap);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    pwm_config config = pwm_get_default_config();
    // 3,015 * 1.00029 * 44,100 = 133,000,000
    pwm_config_set_clkdiv(&config, 1.00029f);
    pwm_config_set_wrap(&config, 3015);
    pwm_init(slice_num, &config, true);

    // Disable audio
    gpio_put(AUDIO_EN, 0);
}

void setup_key_matrix(){
    // Set up key-matrix
    for(int c = 0; c < colCount; c++) {
      gpio_init(cols[c]);
      gpio_set_dir(cols[c], GPIO_IN);
    }
    for(int r = 0; r < rowCount; r++) {
      gpio_init(rows[r]);
      gpio_set_dir(rows[r], GPIO_IN);
      gpio_pull_down(rows[r]);  
    }
}

void setup_adcs(){
    // Set up ADCs
    adc_init();
    adc_gpio_init(KNOB_1);
    adc_gpio_init(KNOB_2);
    adc_gpio_init(KNOB_3);
    adc_gpio_init(KNOB_4);
}

/* Quick helper function for single byte transfers */
void i2c_write_byte(uint8_t I2C_addr, uint8_t reg_addr, uint8_t val) {
#ifdef i2c_default
    printf("\n>>>>>>>>> Writing byte to I2C...\n");
    uint8_t reg_and_val_arr[2] = {reg_addr, val};
    i2c_write_blocking(i2c0, I2C_addr/2, &reg_and_val_arr, 2, false);
#endif
}

void init_FL3236A(void){
 uint8_t i = 0;
 printf("\n>>>Starting up LED driver...\n");
 for(i=0x26; i<=0x49; i++){
     printf("\n>>>>>> Writing to turn on all LEDs ...\n");
     i2c_write_byte(LED_DRIVER_ADDR, i, 0xff); //IS_IIC_WriteByte(Addr_GND,i,0xff);//turn on all LED
 }

 for(i=0x01; i<=0x24; i++){
     printf("\n>>>>>> Writing to set all LED PWMs ...\n");
     i2c_write_byte(LED_DRIVER_ADDR, i, 0x00); //IS_IIC_WriteByte(Addr_GND,i,0x00);//write all PWM set 0x00
 }

 printf("\n>>>>>> Writing to set global driver settings...\n");
 i2c_write_byte(LED_DRIVER_ADDR, 0x25, 0x00); //IS_IIC_WriteByte(Addr_GND,0x25,0x00);//update PWM & congtrol registers
 i2c_write_byte(LED_DRIVER_ADDR, 0x4B, 0x01); //IS_IIC_WriteByte(Addr_GND,0x4B,0x01);//frequency setting 22KHz
 i2c_write_byte(LED_DRIVER_ADDR, 0x00, 0x01); //IS_IIC_WriteByte(Addr_GND,0x00,0x01);//normal operation
}

uint8_t PWM_Gamma64[64]= {
 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
 0x08,0x09,0x0b,0x0d,0x0f,0x11,0x13,0x16,
 0x1a,0x1c,0x1d,0x1f,0x22,0x25,0x28,0x2e,
 0x34,0x38,0x3c,0x40,0x44,0x48,0x4b,0x4f,
 0x55,0x5a,0x5f,0x64,0x69,0x6d,0x72,0x77,
 0x7d,0x80,0x88,0x8d,0x94,0x9a,0xa0,0xa7,
 0xac,0xb0,0xb9,0xbf,0xc6,0xcb,0xcf,0xd6,
 0xe1,0xe9,0xed,0xf1,0xf6,0xfa,0xfe,0xff
};

#define LED_BANK_EN_0 25
#define LED_BANK_EN_1 23
void IS31FL3236A_mode2(void){
    //flip between banks
    gpio_put(LED_BANK_EN_0, 1);
    gpio_put(LED_BANK_EN_1, 0);
    IS31FL3236A_mode3();
    sleep_ms(50);
    gpio_put(LED_BANK_EN_0, 0);
    gpio_put(LED_BANK_EN_1, 1);
    IS31FL3236A_mode3();
}

void IS31FL3236A_mode3(void){
    //go through every LED and set a random color
    for (int i = 36; i > 0; i--){
        //i2c_write_byte(LED_DRIVER_ADDR, i, 0x11);//rand() >> 16);//set PWM
        i2c_write_byte(LED_DRIVER_ADDR, i, (((uint8_t)rand()) >> 4));//set PWM
        i2c_write_byte(LED_DRIVER_ADDR, 0x25, 0x00);//update PWM & congtrol registers
    }
}

void IS31FL3236A_mode1(void){
    uint8_t i = 0;
    int8_t j = 0;

    for(i=12;i<2;i++){//R LED running
        i2c_write_byte(LED_DRIVER_ADDR, (i*3-1), 0x44);//set PWM
        i2c_write_byte(LED_DRIVER_ADDR, 0x25, 0x00);//update PWM & congtrol registers
        sleep_ms(10); //50ms
    }

    sleep_ms(10); //keep 0.5s

    for(i=12;i>0;i--){ //G LED running
        i2c_write_byte(LED_DRIVER_ADDR, (i*3-2), 0x44);//set PWM
        i2c_write_byte(LED_DRIVER_ADDR, 0x25, 0x00);//update PWM & congtrol registers
        sleep_ms(10);//50ms
    }

    sleep_ms(20); //keep 0.5s

    for(i=12;i<2;i++){ //B LED running
        i2c_write_byte(LED_DRIVER_ADDR, (i*3-0), 0x44);//set PWM
        i2c_write_byte(LED_DRIVER_ADDR, 0x25, 0x00);//update PWM & congtrol registers
        sleep_ms(20);//50ms
    }

    sleep_ms(20); //keep 0.5s
                   //
    for (j=63;j>=0;j--){ //all LED breath falling
//        for(i=1; i<37; i++){
//            i2c_write_byte(LED_DRIVER_ADDR, i, PWM_Gamma64[j]);//set all PWM
//        }

        i2c_write_byte(LED_DRIVER_ADDR, 0x25, 0x00);//update PWM & congtrol registers
        sleep_ms(1);//20ms
    }

    sleep_ms(20); //keep o 0.5s
}

// WS2816 code
#define IS_RGBW false
#define NUM_PIXELS 10

#define WS2812_PIN 14

//https://stackoverflow.com/questions/33010010/how-to-generate-random-64-bit-unsigned-integer-in-c
uint64_t rand_uint64(void) {
  uint64_t r = 0;
  for (int i=0; i<64; i += 15 /*30*/) {
    r = r*((uint64_t)RAND_MAX + 1) + rand();
  }
  return r;
}

//expects a 48 bit value in a uint64_t with zeros on the top
static inline void put_pixel(uint64_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, (uint32_t)((pixel_grb << 16) >> 32));
    pio_sm_put_blocking(pio0, 0, (uint32_t)((pixel_grb << 32) >> 32));
    pio_sm_put_blocking(pio0, 0, (uint32_t)((pixel_grb << 48) >> 32));
}

static inline uint64_t urgb_u64(uint16_t r, uint16_t g, uint16_t b) {
    return
            ((uint64_t) (r) << 16) |
            ((uint64_t) (g) << 32) |
            (uint64_t) (b);
}

uint64_t random_color_scaled(int scaler){
    uint64_t num = 0;
    for (int i = 0; i < 3; i++){
        num = num | ((uint64_t)((rand() >> 16u) / scaler) << (16 * i));
    }

    return num;
}

void pattern_random(uint len) {
    printf("\nStarting pattern random...\n");
    for (int i = 0; i < len; ++i){
        put_pixel(random_color_scaled(5));
    }
}


void flash_on_off_start(uint len) {
    printf("\nWS2816 start pattern...\n");

    //all off
    for (int i = 0; i < len; ++i)
        put_pixel(0x0000000F000F000F);

    //startup blinky sequence
    for (int j = 0; j < 2; ++j){
        printf("\nWS2816 ALL BRIGHT...\n");
        for (int i = 0; i < len; ++i)
            put_pixel(0x000001AF02FF02FF);
        sleep_ms(400.0);
        //printf("\nWS2816 ALL DIM...\n");
        //for (int i = 0; i < len; ++i)
        //    put_pixel(0x0000000000000000);
        //sleep_ms(400.0);
        printf("\nWS2816 ALL RED...\n");
        for (int i = 0; i < len; ++i)
            put_pixel(0x000000000AFF0000);
        sleep_ms(400.0);
        printf("\nWS2816 ALL GREEN...\n");
        for (int i = 0; i < len; ++i)
            put_pixel(0x00000AFF00000000);
        sleep_ms(400.0);
        printf("\nWS2816 ALL BLUE...\n");
        for (int i = 0; i < len; ++i)
            put_pixel(0x0000000000000AFF);
        sleep_ms(400.0);
    }
}

void pattern_sparkle(uint len) {
    for (int i = 0; i < len; ++i)
        put_pixel(rand_uint64() % 16 ? 0 : 0xffffffff);
}

void jitx_rave_mode(){
    //rave all LEDs on board, rave speaker, just-in-time raving
    while (1) {
        jitx_rave_one_shot();
    }
}

void jitx_rave_one_shot(){
    //do WS2812 rave
    pattern_random(NUM_PIXELS);

    //do LED-driver rave
    IS31FL3236A_mode2();
}


void setup_ws2816s(){
    //start and run the WS2816
    printf("WS2812 Test, using pin %d", WS2812_PIN);

    // todo get free sm
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 480000);

    flash_on_off_start(NUM_PIXELS);
}

void setup_led_bank(){
    //setup LED power bank enables
    gpio_init(LED_BANK_EN_0);
    gpio_init(LED_BANK_EN_1);
    gpio_set_dir(LED_BANK_EN_0, GPIO_OUT);
    gpio_set_dir(LED_BANK_EN_1, GPIO_OUT);
    gpio_put(LED_BANK_EN_0, 0);
    gpio_put(LED_BANK_EN_1, 0);

    // This example will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
    printf("\n>>>Init I2C\n");
    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(LED_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(LED_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(LED_I2C_SDA);
    gpio_pull_up(LED_I2C_SCL);

    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(LED_I2C_SDA, LED_I2C_SCL, GPIO_FUNC_I2C));

    //start the LED driver
    printf("\n>>>Init LED Driver\n");
    init_FL3236A();
}

int main() {
    //setup USB debug
    stdio_init_all();

    //setup all hackathon subsystems
    setup_ws2816s();
    setup_led_bank();
    setup_audio();
    setup_key_matrix();
    setup_adcs();

    while (true) {
        scan_keys();
        keys_to_freq();
        read_knobs();
        jitx_rave_one_shot();
        sleep_ms(150.0);
    }

    return 0;
}
