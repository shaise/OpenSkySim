/*
    Copyright 2017 fishpepper <AT> gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http:// www.gnu.org/licenses/>.

   author: fishpepper <AT> gmail.com
*/

#include <io.h>
#include <io.h>
#include "failsafe.h"
#include "debug.h"
#include "storage.h"


volatile uint8_t failsafe_active;
volatile uint16_t failsafe_tick_counter;

// failsafe programming
volatile uint8_t failsafe_prog_cnt = 0;
static uint8_t failsafe_btn_cnt = 0; 
static uint8_t failsafe_prog_decim = 0; 


void failsafe_init(void) {
    debug("failsafe: init\n"); debug_flush();
    failsafe_tick_counter = 0;

    // start in failsafe mode
    failsafe_enter();
}

void failsafe_enter(void) {
    // debug("failsafe: enter\n");

    if (failsafe_active == 0) {
        led_red_on();
        led_green_off();

        failsafe_active = 1;
        failsafe_btn_cnt = 0;
    }
}

void failsafe_exit(void) {
    // reset tick counter
    failsafe_tick_counter = 0;

    if (failsafe_active) {
        // reset failsafe counter:
        failsafe_active = 0;

        //led_red_off();
        
        // debug("failsafe: left\n");
    }
}

// failsafe_programming_progress is called every ~1/90 second
void failsafe_programming_check(uint16_t *data)
{
    uint8_t i;
    uint8_t fsdata;
    uint8_t is_updated = 0;
    // decimate calling this functoin to once every ~1/10 second
    failsafe_prog_decim++;
    if (failsafe_prog_decim < 9) {
        return;
    }
    failsafe_prog_decim = 0;
    
    if (failsafe_prog_cnt == 0) {
        // check for button press
        if (!io_bind_request()) {
            failsafe_btn_cnt = 0;
            return;
        }
        failsafe_btn_cnt++;
        if (failsafe_btn_cnt > FAILSAFE_PRESS_TIME) {
            failsafe_prog_cnt = FAILSAFE_PROGRAM_WAIT_TIME;
            led_red_on();
        }
    }
    else {
        if (io_bind_request()) {
            // wait for button release
            return;
        }
        // initiate 10 seconds wait, then program current state
        failsafe_prog_cnt--;
        if ((failsafe_prog_cnt & 2) == 0) {
            led_red_off();
        }
        else {
            led_red_on();
        }

        if (failsafe_prog_cnt == 0) {
            debug("\nSet New failsafe: ");
            for (i = 0; i < 8; i ++) {
                if (data[i] == 0)
                    continue;
                fsdata = (uint8_t)(PWM_FRSKY_TO_US(data[i]) / 10);
                if (fsdata > 90 && fsdata < 210) {
                    storage.fail_safe_state[i] = fsdata;
                    is_updated = 1;
                }
                debug_put_uint8(storage.fail_safe_state[i]);
                debug_putc(',');
            }
            debug_putc('\n');

            if (is_updated) {
                storage_write_to_flash();
            }
            failsafe_btn_cnt = 0;
            led_red_off();
        }

    }
}
