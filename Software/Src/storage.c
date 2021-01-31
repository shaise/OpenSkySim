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

#include "storage.h"
#include "frsky.h"
#include "debug.h"
#include "string.h"
#include "stm32f1xx_hal_flash_ex.h"


// run time copy of persistant storage data:
STORAGE_DESC storage;

static const uint8_t *flash_target = (uint8_t *)(FLASH_BANK1_END - FLASH_PAGE_SIZE + 1);    // Last page start adress

uint8_t storage_read(uint8_t *dest, int len)
{
    memcpy(dest, flash_target, len);
    return 1;
}

uint8_t storage_write(uint8_t *source, int len)
{
    if (!memcmp(source, flash_target, len))
        return 1;

    len = (((len - 1) / 4) + 1) * 4; // len must be 4 bytes alligned

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef erase = {
        .Banks = FLASH_BANK_1,
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .NbPages = 1,
        .PageAddress = (uint32_t)flash_target
    };

    uint32_t error;

    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&erase, &error);

    uint16_t *data = (uint16_t *)source;
    uint32_t address = (uint32_t)flash_target, remaining = (uint32_t)len;

    while(remaining && status == HAL_OK) {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, *data++);
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address + 2, *data++);
        address += 4;
        remaining -= 4;
    }

    HAL_FLASH_Lock();

    return status == HAL_OK;
}

void storage_init(void) {
    uint8_t i;

    debug("storage: init\n"); debug_flush();

    // reload data from flash
    storage_read_from_flash();

    debug("storage: loaded hoptable[] = ");
    for (i = 0; i < FRSKY_HOPTABLE_SIZE; i++) {
        debug_put_hex8(storage.frsky_hop_table[i]);
        debug_putc(' ');
        debug_flush();
    }
    debug_put_newline();

    debug("storage: loaded failsafe[]: ");
    for (i = 0; i < 8; i ++) {
        debug_put_uint8(storage.fail_safe_state[i]);
        debug_putc(' ');
        debug_flush();
    }
    debug_put_newline();
}

static const uint8_t storage_default_hoptable[] =
      { 0x01, 0x42, 0x83, 0xC4, 0x1A, 0x5B, 0x9C, 0xDD, 0x33, 0x74, 0xB5, 0x0B,
        0x4C, 0x8D, 0xCE, 0x24, 0x65, 0xA6, 0xE7, 0x3D, 0x7E, 0xBF, 0x15, 0x56,
        0x97, 0xD8, 0x2E, 0x6F, 0xB0, 0x06, 0x47, 0x88, 0xC9, 0x1F, 0x60, 0xA1,
        0xE2, 0x38, 0x79, 0xBA, 0x10, 0x51, 0x92, 0xD3, 0x29, 0x6A, 0xAB };

void storage_read_from_flash(void) {
    uint8_t *storage_ptr;
    uint16_t len;
    uint8_t i;

    debug("storage: reading\n"); debug_flush();
    storage_ptr = (uint8_t*)&storage;
    len = sizeof(storage);

    storage_read(storage_ptr, len);

#ifdef FRSKY_USE_FIXED_ID
    // allow override for testing
    if (1) {
#else  // FRSKY_USE_FIXED_ID
    // only init with defaults when no valid storage id was found
    if (storage.version != STORAGE_VERSION_ID) {
#endif  // FRSKY_USE_FIXED_ID
        debug("storage: init with defaults (txid 0x1668)\n");

        storage.version = STORAGE_VERSION_ID;

        // hard coded config for debugging:
        storage.frsky_txid[0] = 0x16;
        storage.frsky_txid[1] = 0x68;
        storage.frsky_freq_offset = DEFAULT_FSCAL_VALUE;

        for (i = 0; i < FRSKY_HOPTABLE_SIZE; i++) {
            storage.frsky_hop_table[i] = storage_default_hoptable[i];
        }

        // fill default failsafe state
        for (i = 0; i < 8; i++) {
            storage.fail_safe_state[i] = 150; // = 1.5 ms
        }
        storage.fail_safe_state[2] = 100; // channel 3 (throttle) to minimum
        
        // store settings
        storage_write_to_flash();
    }
}

void storage_write_to_flash(void) {
    uint8_t *storage_ptr;
    uint16_t len;

    debug("storage: writing\n"); debug_flush();
    storage.version = STORAGE_VERSION_ID;

    storage_ptr = (uint8_t*)&storage;
    len = sizeof(storage);

    // execute flash write:
    storage_write(storage_ptr, len);
}

