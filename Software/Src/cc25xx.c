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

#include "cc25xx.h"
#include "soft_spi.h"
#include "debug.h"
#include "string.h"
#include "timeout.h"

uint8_t cc25xx_current_antenna;
#define cc25xx_get_gdo_status() ((CC2500_MISO_GPIO_Port->IDR & CC2500_MISO_Pin) != 0)


void cc25xx_init(void) {
    debug("cc25xx: init\n"); debug_flush();
    soft_spi_init();
    cc25xx_current_antenna = 0;
    cc25xx_set_antenna(cc25xx_current_antenna);

    // PA/LNA:
    cc25xx_enter_rxmode();
}

inline uint32_t cc25xx_set_antenna(uint8_t id) {
    // select antenna 0 or 1:
    return id; // we use only 1 antenna. Fix here if diversity available
}

inline void cc25xx_set_gdo_mode(void) {
    cc25xx_set_register(IOCFG0, 0x1B);  // PA;
    cc25xx_set_register(IOCFG1, 0x01);
    cc25xx_set_register(IOCFG2, 0x1C);  // PD);
}

inline uint8_t cc25xx_set_register(uint8_t address, uint8_t data) {
    // select device
    soft_spi_csn_lo();

    // wait for ready signal
    while (cc25xx_get_gdo_status()) {}

    uint8_t status = soft_spi_tx(address);
    soft_spi_tx(data);

    // deslect
    soft_spi_csn_hi();
    return status;
}

inline uint8_t cc25xx_get_register(uint8_t address) {
    uint8_t result;

    // select device:
    soft_spi_csn_lo();

    // wait for RDY signal:
    while (cc25xx_get_gdo_status());
 
    // request address (read request has bit7 set)
    /*uint8_t status = */ soft_spi_tx(address | 0x80);
    // debug_put_hex8(status);

    // fetch result:
    result = soft_spi_rx();

    // deselect device
    soft_spi_csn_hi();

    // return result
    return(result);
}

inline uint8_t cc25xx_strobe(uint8_t address) {
    soft_spi_csn_lo();
    uint8_t status = soft_spi_tx(address);
    // debug("s"); debug_put_hex8(status); debug_put_newline();
    soft_spi_csn_hi();
    return status;
}

inline uint8_t cc25xx_get_status(void) {
    soft_spi_csn_lo();
    uint8_t status = soft_spi_tx(0xBD);
    soft_spi_csn_hi();
    return status;
}

uint8_t cc25xx_transmission_completed(void) {
    // after tx cc25xx goes back to RX (configured by mcsm1 register)
    return ((cc25xx_get_status() & (0x70)) == CC2500_STATUS_STATE_RX);
}

inline void cc25xx_enter_rxmode(void) {
    // add pa/lna config bit setting here
    // should be done automatically
    delay_us(25);
}

inline void cc25xx_enter_txmode(void) {
    // add pa/lna config bit setting here
    // should be done automatically
    delay_us(25);
}


inline void cc25xx_enable_receive(void) {
    // switch on rx again
    cc25xx_enter_rxmode();
}



inline void cc25xx_read_fifo(uint8_t *buf, uint8_t len) {
    cc25xx_register_read_multi(CC25XX_FIFO | READ_FLAG | BURST_FLAG, buf, len);
}

inline void cc25xx_register_read_multi(uint8_t address, uint8_t *buffer, uint8_t len) {
    // select device:
    soft_spi_csn_lo();

    // wait for ready signal
    while (cc25xx_get_gdo_status()) {}

    // debug("read "); debug_put_uint8(len); debug_flush();
    // request address (read request)
    /* uint8_t status = */ soft_spi_tx(address);

    // fill buffer with read commands:
    memset(buffer, 0xFF, len);

    while (len--) {
        *buffer = soft_spi_rx();
        buffer++;
    }

    // deselect device
    soft_spi_csn_hi();
}

#if 0
inline void cc25xx_register_write_multi(uint8_t address, uint8_t *buffer, uint8_t len) {
    // s elect device:
    soft_spi_csn_lo();

    // wait for RDY signal:
    while (cc25xx_get_gdo_status()) {}

    // request address (write request)
    soft_spi_tx(address | BURST_FLAG);

    // send array
    while (len--) {
        soft_spi_tx(*buffer);
        buffer++;
    }

    // deselect device
    soft_spi_csn_hi();
}
#endif

inline void cc25xx_process_packet(volatile uint8_t *packet_received,
                                      volatile uint8_t *buffer, uint8_t maxlen) {
    *packet_received = 0;
    if (cc25xx_get_gdo_status() == 1) {
        // data received, fetch data
        timeout2_set_100us(5);  // set 0.5ms timeout before next hop (used for telemetry: frsky.c:786)
        // there is a bug in the cc2500
        // see p3 http:// www.ti.com/lit/er/swrz002e/swrz002e.pdf
        // workaround: read len register very quickly twice:
        uint8_t len1, len2, len, i;

        // try this 10 times befor giving up:
        for (i = 0; i < 10; i++) {
            len1 = cc25xx_get_register_burst(RXBYTES) & 0x7F;
            len2 = cc25xx_get_register_burst(RXBYTES) & 0x7F;
            if (len1 == len2) break;
        }

        // valid len found?
        if (len1 == len2) {
            len = len1;

            // packet received, grab data
            uint8_t tmp_buffer[len];
            cc25xx_read_fifo(tmp_buffer, len);

            // only accept valid packet lenbghts:
            if (len == maxlen) {
                uint8_t i;
                for (i = 0; i < maxlen; i++) {
                    buffer[i] = tmp_buffer[i];
                }
                *packet_received = 1;
            }
        } else {
            // no, ignore this
            debug_putc('B');
            len = 0;
        }
    }
 }

void cc25xx_transmit_packet(volatile uint8_t *buffer, uint8_t len) {
    // flush tx fifo
    cc25xx_strobe(RFST_SFTX);
    // copy to fifo

    // register write multi
    soft_spi_csn_lo();

    // wait for RDY signal:
    while (cc25xx_get_gdo_status()) {}

    // request address (write request)
    soft_spi_tx(CC25XX_FIFO | BURST_FLAG);

    // send array
    while (len--) {
        soft_spi_tx(*buffer);
        buffer++;
    }

    // deselect device
    soft_spi_csn_hi();

    //cc25xx_register_write_multi(CC25XX_FIFO, (uint8_t *)buffer, buffer[0]+1);
    // and send!
    cc25xx_strobe(RFST_STX);
}

void cc25xx_switch_antenna(void) {
    // switch to next antenna
    if (cc25xx_current_antenna) {
        cc25xx_current_antenna = cc25xx_set_antenna(0);
    } else {
        cc25xx_current_antenna = cc25xx_set_antenna(1);
    }
}

void cc25xx_wait_for_transmission_complete(void) {
    // after STX we go back to RX state (see MCSM1 register)
    // so wait a maximum of 9ms for completion
    timeout2_set_100us(90);

    while (!timeout2_timed_out()) {
        if (cc25xx_transmission_completed()) {
            // done with tx, return
            return;
        }
        delay_us(20);
    }

    // if we reach this point, tx timed out:
    debug("!TX");
}

