#include "proxmark3.h"
#include "apps.h"
#include "EEPROM.h"

uint8_t EEPROMSend(uint8_t data, uint8_t endOfTransfer)
{
  // 9th bit set for data, clear for command
  while ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);  // wait for the transfer to complete
  
  AT91C_BASE_SPI->SPI_TDR = data;              // Send the data/command

  while ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);  // wait for the transfer to complete

  return AT91C_BASE_SPI->SPI_RDR;

  // Wait for SPI to raise the CS line
  if (endOfTransfer == 1) {
    SpinDelay(10);
  }
}

uint8_t EEPROMReadByte (uint32_t address)
{
  // Wait for chip to be ready
  EEPROMBusyWait();

  // Read Byte
  EEPROMSend(EEPROM_READ, 0);
  EEPROMSend((address >> 16) & 0xFF, 0);
  EEPROMSend((address >>  8) & 0xFF, 0);
  EEPROMSend(address & 0xFF, 0);
  return EEPROMSend(0, 1);
}

void EEPROMWriteByte (uint32_t address, uint8_t data)
{
  // Wait for chip to be ready
  EEPROMBusyWait();

  // Write Enable
  EEPROMSend(EEPROM_WREN, 1);

  // Write Byte
  EEPROMSend(EEPROM_WRITE, 0);
  EEPROMSend((address >> 16) & 0xFF, 0);
  EEPROMSend((address >>  8) & 0xFF, 0);
  EEPROMSend(address & 0xFF, 0);
  EEPROMSend(data, 1);

}

void EEPROMChipErase (void)
{
  // Wait for chip to be ready
  EEPROMBusyWait();

  // Erase Chip
  EEPROMSend(EEPROM_CE, 1);
}

void EEPROMBusyWait (void) {
  do {
    EEPROMSend(EEPROM_RDSR, 0);
  }while (EEPROMSend(0, 1) & 0x01);
}

void EEPROMInit (void)
{
  SetupSpi(SPI_EEPROM_MODE);
}