#include "proxmark3.h"
#include "apps.h"
#include "EEPROM.h"
#include "util.h"

uint8_t EEPROMReadSignature()
{
 // Read Byte with dummy address
  uint32_t address=0;
  EEPROMSend(EEPROM_RDSR,0);
  EEPROMSend((address >> 16) & 0xFF, 0);
  EEPROMSend((address >>  8) & 0xFF,0);
  EEPROMSend((address & 0xFF), 0);
  EEPROMSend(0xFF,1);
  while (!(AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY)){}  // wait for previous transfer$
  while (!(AT91C_BASE_SPI->SPI_SR & AT91C_SPI_RDRF)){}//wait for data to be in RX buffer
  return (AT91C_BASE_SPI->SPI_RDR & 0xFF);
}

void EEPROMSend(uint16_t data, uint8_t endOfTransfer)
{
 
  // 9th bit set for data, clear for command
  while (!(AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TDRE)){}  // wait for previous transfer to complete
  if(endOfTransfer == 0)//if it is not the end of transfer, dont set LASTTX
  {
	AT91C_BASE_SPI->SPI_TDR= data;
  }
  else//but if it is, set it to raise CS
  {
	AT91C_BASE_SPI->SPI_TDR=AT91C_SPI_LASTXFER | data;
  }
}


uint8_t EEPROMReadByte (uint32_t address)
{
  // Read Byte
  EEPROMSend(EEPROM_READ,0); 
  EEPROMSend((address >> 16) & 0xFF, 0);
  EEPROMSend((address >>  8) & 0xFF,0);
  EEPROMSend((address & 0xFF), 0);
  EEPROMSend(0xFF,1);
  while (!(AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY)){}  // wait for previous transfer to comple$
  while (!(AT91C_BASE_SPI->SPI_SR & AT91C_SPI_RDRF)){}//wait for data to be in RX buffer 
  return (AT91C_BASE_SPI->SPI_RDR & 0xFF);
}

void EEPROMReadPage (uint32_t page, uint16_t *eBuf, uint32_t len)
{
  // Read Byte
  uint32_t address=256*page; //address for page start
  EEPROMSend(EEPROM_READ,0);
  EEPROMSend((address >> 16) & 0xFF, 0);
  EEPROMSend((address >>  8) & 0xFF,0);
  EEPROMSend((address & 0xFF), 0);
  for(uint16_t ecnt=0;ecnt<(len - 1);ecnt++)
  {
    EEPROMSend(0xFF,0);
    while (!(AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY)){}  // wait for previous transfer to comple$
    while (!(AT91C_BASE_SPI->SPI_SR & AT91C_SPI_RDRF)){}//wait for data to be in RX buffer
    eBuf[ecnt]=(AT91C_BASE_SPI->SPI_RDR & 0xFF);
  }
   EEPROMSend(0xFF,1); //signal Last TX
   while (!(AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY)){}  // wait for previous transfer to compl$
   while (!(AT91C_BASE_SPI->SPI_SR & AT91C_SPI_RDRF)){}//wait for data to be in RX buffer
   eBuf[len-1]=(AT91C_BASE_SPI->SPI_RDR & 0xFF);
  return;
}


void EEPROMWritePage (uint32_t page, uint16_t *data, uint32_t len)
{
  // Write Enable
  EEPROMSend(EEPROM_WREN, 1);
  SpinDelay(1);
 // Write Byte
  uint32_t address=page*256;
  EEPROMSend(EEPROM_WRITE,0);
  EEPROMSend((address >> 16) & 0xFF, 0);
  EEPROMSend((address >>  8) & 0xFF,0);
  EEPROMSend((address & 0xFF), 0);
  for(uint16_t ecnt=0;ecnt<(len - 1);ecnt++)
  {
    EEPROMSend(data[ecnt],0);
  }
  EEPROMSend(data[len - 1],1); //send the last byte
  EEPROMBusy();
  return;
}

void EEPROMWriteByte (uint32_t address, uint16_t data)
{
  // Write Enable
  EEPROMSend(EEPROM_WREN, 1);
  SpinDelay(1);
 // Write Byte
  EEPROMSend(EEPROM_WRITE,0);
  EEPROMSend((address >> 16) & 0xFF, 0);
  EEPROMSend((address >>  8) & 0xFF,0);
  EEPROMSend((address & 0xFF), 0);
  EEPROMSend(data, 1);
  EEPROMBusy();
}


void EEPROMBusy()//Wait for WEL
{
  SpinDelay(1);
  while (1)
  {
    EEPROMSend(EEPROM_RDSR, 0);
    EEPROMSend(0xFF,1);
    while (!(AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY)){}  // wait for previous transfer to comple$
    while (!(AT91C_BASE_SPI->SPI_SR & AT91C_SPI_RDRF)){}//wait for data to be in RX buffer
   // Dbprintf("%08X",AT91C_BASE_SPI->SPI_RDR);
    if (!(AT91C_BASE_SPI->SPI_RDR & 0xFF) & 0x01)
    {
      return;
    }
    SpinDelay(1);
  }
}

void EEPROMChipErase (void)
{
  // Write Enable
  EEPROMSend(EEPROM_WREN, 1);
  SpinDelay(1);
  // Erase Chip
  EEPROMSend(EEPROM_CE, 1);
}

void EEPROMPageErase (uint32_t page)
{
  // Write Enable
  EEPROMSend(EEPROM_WREN, 1);
  SpinDelay(1);
  // Erase Chip
  uint32_t address=page*256;
  EEPROMSend(EEPROM_PE,0);
  EEPROMSend((address >> 16) & 0xFF, 0);
  EEPROMSend((address >>  8) & 0xFF,0);
  EEPROMSend((address & 0xFF), 1);

}


void EEPROMInit (void)
{
  SetupSpi(SPI_EEPROM_MODE);  
}
