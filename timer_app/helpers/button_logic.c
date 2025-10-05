#include "button_logic.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#ifndef BTN_PIN
#define BTN_PIN 15
#endif

#ifndef DEBOUNCE_MS
#define DEBOUNCE_MS 25
#endif

void button_init(void) {
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);
}

bool button_falling_edge(void) {
    static uint32_t last_ms = 0;
    static bool last_level = true;
    bool level = gpio_get(BTN_PIN);
    uint32_t ms = to_ms_since_boot(get_absolute_time());
    if (ms - last_ms < DEBOUNCE_MS) return false;
    bool edge = (last_level == true && level == false);
    if (edge) last_ms = ms;
    last_level = level;
    return edge;
}
