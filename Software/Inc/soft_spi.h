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

#ifndef SOFT_SPI_H_
#define SOFT_SPI_H_

#include "main.h"

#define soft_spi_csn_lo() { HAL_GPIO_WritePin(CC2500_CSN_GPIO_Port, CC2500_CSN_Pin, 0); delay_us(1); }
#define soft_spi_csn_hi() { delay_us(1); HAL_GPIO_WritePin(CC2500_CSN_GPIO_Port, CC2500_CSN_Pin, 1); }
void soft_spi_init(void);
uint8_t soft_spi_tx(uint8_t value);
#define soft_spi_rx()  soft_spi_tx(0)

#endif  // SOFT_SPI_H_
