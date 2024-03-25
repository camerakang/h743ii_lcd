#ifndef __KD024VGFPD094_H__
#define __KD024VGFPD094_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
    void KD024VGFPD094_init(void);
    void SSD_SEND(uint8_t numArgs, ...);
    void write_command(unsigned char y);
    void write_data(unsigned char w);
    void read_data(unsigned char y, unsigned char *readValue);
    void SPI_Send9BitData(uint16_t data);
#ifdef __cplusplus
}
#endif

#endif /* __KD024VGFPD094_H__ */
