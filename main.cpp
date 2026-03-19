#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "DmxInput.h"
#include <time.h>
#include <string.h>
#include <ctype.h>

const uint DMXIN1_PIN = 7;
const uint DMXENA1_PIN = 3;

const uint OUTPUT_PIN1 = 15;
const uint OUTPUT_PIN2 = 16;

const uint LED_PIN_DMXPORTA = 24;
const uint LED_PIN_INDICATOR = 25;

DmxInput dmxInput1;

#define UNIVERSE_LENGTH 512
#define INPUT_TIMEOUT_MS 3000
#define OUTPUT_TIMEOUT_MS 3000

uint8_t dmxDataA1[UNIVERSE_LENGTH + 1];
uint8_t dmxDataB1[UNIVERSE_LENGTH + 1];
bool outputPrimaryBuffer1;
uint8_t dmxDataA2[UNIVERSE_LENGTH + 1];
uint8_t dmxDataB2[UNIVERSE_LENGTH + 1];
bool outputPrimaryBuffer2;

uint8_t dmxInputBuffer1[DMXINPUT_BUFFER_SIZE(512)];
uint8_t dmxInputBuffer2[DMXINPUT_BUFFER_SIZE(512)];

uint16_t mapping[512];

uint32_t timer_counter;
bool ledValue;
bool ledAValue;
bool ledBValue;

int led_mod_value;
int ledA_mod_value;
int ledB_mod_value;

clock_t lastInputFrame1;
clock_t lastInputFrame2;

bool inputActive1;
bool inputStatus1;

// Pulse tracking variables
bool lastChannelState = false;  // Track previous state to detect transitions
bool pulseActive = false;       // Whether pulse is currently active
uint64_t pulseStartTime = 0;    // When the pulse started

// Channel 2 pulse tracking
bool lastChannelState2 = false; // Track previous state for channel 2
bool pulseActive2 = false;      // Whether pulse 2 is currently active
uint64_t pulseStartTime2 = 0;   // When pulse 2 started


bool repeating_timer_callback(struct repeating_timer *t) {

    clock_t now = time_us_64();
    int flashSpeed = 0;

    // Check inputs for timeout
    int ms_since_last = (now - lastInputFrame1) / 1000;
    if (inputActive1 && ms_since_last > INPUT_TIMEOUT_MS)
    {
        // Flash slowly
        ledA_mod_value = 20;
    }

    timer_counter++;

    switch (flashSpeed)
    {
        case 1:
            led_mod_value = 10;
            break;

        case 2:
            led_mod_value = 5;
            break;

        default:
            led_mod_value = 40;
            break;
    }

    if (led_mod_value == 0)
    {
        gpio_put(LED_PIN_INDICATOR, 0);
    }
    else if (led_mod_value == 1)
    {
        gpio_put(LED_PIN_INDICATOR, 1);
    }
    else if (timer_counter % (led_mod_value - 1) == 0)
    {
        ledValue = !ledValue;
        gpio_put(LED_PIN_INDICATOR, ledValue);
    }

    if (ledA_mod_value == 0)
    {
        gpio_put(LED_PIN_DMXPORTA, 0);
    }
    else if (ledA_mod_value == 1)
    {
        gpio_put(LED_PIN_DMXPORTA, 1);
    }
    else if (timer_counter % (ledA_mod_value - 1) == 0)
    {
        ledAValue = !ledAValue;
        gpio_put(LED_PIN_DMXPORTA, ledAValue);
    }

    return true;
}

void __isr dmxDataReceived(DmxInput* instance) {
     // A DMX frame has been received

    if (instance == &dmxInput1 && inputActive1)
    {
        // Input 1
        lastInputFrame1 = time_us_64();

        if (dmxInputBuffer1[0] == 0)
        {
            // Start code 0
            ledA_mod_value = 3;

            //printf("DMX Input 1 dmxInputBuffer1[1]: %s\n" , dmxInputBuffer1[1]);

            // Have channel 1 control Output 1 with pulse, 2=2, 3=3, 4=4
            bool currentChannelState = dmxInputBuffer1[1] > 50;
            
            // Detect rising edge (transition from off to on)
            if (currentChannelState && !lastChannelState) {
                // Channel just turned on, start pulse
                pulseActive = true;
                pulseStartTime = time_us_64();
                gpio_put(OUTPUT_PIN1, 1);  // Turn on GPIO 1
                printf("DMX Channel 1 > 50: GPIO 1 ON\n");
            }
            
            lastChannelState = currentChannelState;
            
            // Have channel 2 control Output 2 with pulse
            bool currentChannelState2 = dmxInputBuffer1[2] > 50;
            
            // Detect rising edge for channel 2
            if (currentChannelState2 && !lastChannelState2) {
                // Channel 2 just turned on, start pulse
                pulseActive2 = true;
                pulseStartTime2 = time_us_64();
                gpio_put(OUTPUT_PIN2, 1);  // Turn on GPIO 2
                printf("DMX Channel 2 > 50: GPIO 2 ON\n");
            }
            
            lastChannelState2 = currentChannelState2;
        }
    }
}

int main() {
    stdio_init_all();

    // There isn't a strict rule that you must always call sleep_ms()
    // after stdio_init_all(). However, in some cases, it can be a helpful
    // precautionary measure to ensure that the UART has properly 
    // initialized and is ready to transmit data without any issues.
    sleep_ms(2000);

    printf("--==Init==--\n");

    // Init all inputs and outputs

    gpio_init(OUTPUT_PIN1);
    gpio_init(OUTPUT_PIN2);

    gpio_init(LED_PIN_DMXPORTA);
    //gpio_init(LED_PIN_DMXPORTB);
    gpio_init(LED_PIN_INDICATOR);

    gpio_set_dir(OUTPUT_PIN1, GPIO_OUT);
    gpio_set_dir(OUTPUT_PIN2, GPIO_OUT);

    gpio_set_dir(LED_PIN_DMXPORTA, GPIO_OUT);
    gpio_set_dir(LED_PIN_INDICATOR, GPIO_OUT);

    // Set initial state
    gpio_put(OUTPUT_PIN1, 0);
    gpio_put(OUTPUT_PIN2, 0);

    gpio_put(LED_PIN_DMXPORTA, 0);
    gpio_put(LED_PIN_INDICATOR, 0);

    // Init DMX pins
    gpio_init(DMXENA1_PIN);
    gpio_set_dir(DMXENA1_PIN, GPIO_OUT);
    
    // Start DMX inputs
    dmxInput1.begin(DMXIN1_PIN, 512);

    // Clear buffers
    memset(dmxDataA1, 0, sizeof(dmxDataA1));
    memset(dmxDataB1, 0, sizeof(dmxDataB1));
    memset(dmxDataA2, 0, sizeof(dmxDataA2));
    memset(dmxDataB2, 0, sizeof(dmxDataB2));

    // Start send timer
    struct repeating_timer timer;
    add_repeating_timer_ms(-25, repeating_timer_callback, NULL, &timer);

    // Wire up DMX input callbacks
    dmxInput1.read_async(dmxInputBuffer1, dmxDataReceived);

    // Set initial configuration
    inputActive1 = true;

    while (true)
    {
        tight_loop_contents();
        
        // Check if pulse timeout has elapsed (500ms = 500000 microseconds)
        if (pulseActive && (time_us_64() - pulseStartTime) > 500000) {
            gpio_put(OUTPUT_PIN1, 0);  // Turn off GPIO 1
            printf("GPIO 1 OFF after 500ms\n");
            pulseActive = false;
        }
        
        // Check if pulse 2 timeout has elapsed
        if (pulseActive2 && (time_us_64() - pulseStartTime2) > 500000) {
            gpio_put(OUTPUT_PIN2, 0);  // Turn off GPIO 2
            printf("GPIO 2 OFF after 500ms\n");
            pulseActive2 = false;
        }

        sleep_ms(500);
    }    
}
