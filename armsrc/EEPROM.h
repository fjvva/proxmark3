#ifndef __EEPROM_H
#define __EEPROM_H


// Microchip 25LC1024
#define EEPROM_READ    0x03
#define EEPROM_WRITE   0x02
#define EEPROM_WREN    0x06
#define EEPROM_WRDI    0x04
#define EEPROM_RDSR    0x05
#define EEPROM_WRSR    0x01
#define EEPROM_PE      0x42
#define EEPROM_SE      0xD8
#define EEPROM_CE      0xC7
#define EEPROM_RDID    0xAB
#define EEPROM_DPD     0xB9

#endif


void EEPROMSend(uint16_t data, uint8_t endOfTransfer);
uint8_t EEPROMReadByte (uint32_t address);
void EEPROMWriteByte (uint32_t address, uint16_t data);
void EEPROMChipErase (void);
void EEPROMBusy (void);
void EEPROMInit (void);
