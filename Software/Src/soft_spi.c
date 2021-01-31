#include "soft_spi.h"

void soft_spi_init(void)
{
	HAL_GPIO_WritePin(CC2500_CSN_GPIO_Port, CC2500_CSN_Pin, 1);
	HAL_GPIO_WritePin(CC2500_CLK_GPIO_Port, CC2500_CLK_Pin, 0);
}

int dummy = 1; // for timings

uint8_t soft_spi_tx(uint8_t value)
{
    uint8_t res = 0, i;
    // setup first bit
    if ((value & 0x80) != 0)
      CC2500_MOSI_GPIO_Port->BSRR = CC2500_MOSI_Pin;
    else
      CC2500_MOSI_GPIO_Port->BSRR = ((uint32_t)CC2500_MOSI_Pin << 16);
    for (i = 0; i < 8; i++)
    {
         // read bit
        res <<= 1;
        if ((CC2500_MISO_GPIO_Port->IDR & CC2500_MISO_Pin) != 0)
            res |= 1;
        // clock up
        if (dummy)
          CC2500_CLK_GPIO_Port->BSRR = CC2500_CLK_Pin;
        // setup next bit
        value <<= 1;
        if ((value & 0x80) != 0)
          CC2500_MOSI_GPIO_Port->BSRR = CC2500_MOSI_Pin;
        else
          CC2500_MOSI_GPIO_Port->BSRR = ((uint32_t)CC2500_MOSI_Pin << 16);
        // clk down
        if (dummy)
          CC2500_CLK_GPIO_Port->BSRR = ((uint32_t)CC2500_CLK_Pin << 16);
    }
    return res;
}

//uint8_t soft_spi_read_address(uint8_t address);
