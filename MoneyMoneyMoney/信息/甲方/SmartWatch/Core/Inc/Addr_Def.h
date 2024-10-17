#ifndef __ADDR_DEF_H__
#define __ADDR_DEF_H__

#define ADDR_EEPROM         0x2000
#define ADDR_EEPROM_END     0x2FFF
#define EEPROM_PAGE_SIZE    32              // * 每页32byte; 4096byte 共 128 page

#define EE_PAGE_START_ADDR(PAGE)  (PAGE##U * EEPROM_PAGE_SIZE + ADDR_EEPROM)

/*
    Page 0 - 9 = 320 bytes
*/
#define ADDR_DEVICE_NUMBER          EE_PAGE_START_ADDR(0)       // 0x2000, 8 bytes
#define ADDR_DEVICE_ACTIVATE        ADDR_DEVICE_NUMBER + 8      // 0x2008, 1 bytes

/*
    Page 10 - 127  = 3776 bytes
*/
#define ADDR_GPS_BLOOD_DATA_PT       EE_PAGE_START_ADDR(10)       // 0x2140, 2 bytes , 数据保存个数
#define ADDR_GPS_BLOOD_DATA_BUF      ADDR_GPS_BLOOD_DATA_PT + 2   // 0x2142, 12 * 6 * 50 = 3600 bytes , 48byte数据 + 2byteCRC
#define ADDR_GPS_BLOOD_DATA_END      ADDR_GPS_BLOOD_DATA_BUF + 3600 

#endif
