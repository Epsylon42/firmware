#include "uwu.h"

#include "buspal/microseconds/microseconds.h"
#include "segment_display.h"

static uint64_t millis;
static uint64_t millis_target;

enum AnimationType {
    ANIMATION_LONG,
    ANIMATION_SHORT,
} animation_type;

static uint32_t long_state;
static uint32_t short_state;

static bool uwu_enabled;

void uwu_init() {
    uwu_enabled = false;
}

// approx 10 to 60 seconds
static uint64_t random_delay() {
    return (microseconds_get_ticks() % 50 + 10) * 1000;
}

static enum AnimationType random_animation_type() {
    if (microseconds_get_ticks() % 10 == 0) {
        return ANIMATION_LONG;
    } else {
        return ANIMATION_SHORT;
    }
}

static void setText(const char* text)
{
    SegmentDisplay_SetText(strlen(text), text, SegmentDisplaySlot_Keymap);
}

static void long_cycle(uint32_t state) {
    switch (state % 21) {
        case 0:
            setText("UWU");
            break;
        case 1:
            setText("bWb");
            break;
        case 2:
            setText("aWa");
            break;
        case 3:
            setText("OWO");
            millis_target = millis + 2000;
            break;
        case 4:
            setText("aWa");
            break;
        case 5:
            setText("bWb");
            break;
        case 6:
            setText("UWU");
            break;
        case 7:
            setText("bWb");
            break;
        case 8:
            setText("aWa");
            break;
        case 9:
            setText("OWO");
            break;
        case 10:
            setText("cWc");
            break;
        case 11:
            setText("dWd");
            millis_target = millis + 100;
            break;
        case 12:
            setText("eWe");
            millis_target = millis + 1000;
            break;
        case 13:
            setText("dWd");
            millis_target = millis + 100;
            break;
        case 14:
            setText("cWc");
            millis_target = millis + 1000;
            break;
        case 15:
            setText("dWd");
            millis_target = millis + 100;
            break;
        case 16:
            setText("eWe");
            millis_target = millis + 1000;
            break;
        case 17:
            setText("OWO");
            millis_target = millis + 20000;
            break;
        case 18:
            setText("aWa");
            break;
        case 19:
            setText("bWb");
            break;
        case 20:
            setText("UWU");
            millis_target = millis + random_delay();
            animation_type = random_animation_type();
            break;
    }
}

static void short_cycle(uint32_t state) {
    switch (state % 13) {
        case 0:
            setText("UWU");
            break;
        case 1:
            setText("bWb");
            break;
        case 2:
            setText("aWa");
            break;
        case 3:
            setText("OWO");
            millis_target = millis + 3000;
            break;
        case 4:
            setText("aWa");
            break;
        case 5:
            setText("bWb");
            break;
        case 6:
            setText("UWU");
            break;
        case 7:
            setText("bWb");
            break;
        case 8:
            setText("aWa");
            break;
        case 9:
            setText("OWO");
            millis_target = millis + 3000;
            break;
        case 10:
            setText("aWa");
            break;
        case 11:
            setText("bWb");
            break;
        case 12:
            setText("UWU");
            millis_target = millis + random_delay();
            animation_type = random_animation_type();
            break;
    }
}

void uwu_ticks(uint64_t ticks_inc) {
    if (uwu_enabled) {
        millis += ticks_inc / 10;

        if (millis >= millis_target) {
            if (animation_type == ANIMATION_LONG) {
                long_state += 1;
            } else {
                short_state += 1;
            }
            millis_target = millis + 500;

            if (animation_type == ANIMATION_LONG) {
                long_cycle(long_state);
            } else {
                short_cycle(short_state);
            }
        }
    }
}

void uwu_switch(bool enabled) {
    uwu_enabled = enabled;
    if (enabled) {
        long_state = 0;
        short_state = 0;
        animation_type = ANIMATION_LONG;
        millis = 0;
        millis_target = 1000;
        uwu_ticks(0);
    }
}
