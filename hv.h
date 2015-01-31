#ifndef HV_H
#define HV_H

extern void enter_hv(void);
extern void exit_hv(void);
extern uint8_t rw_byte(uint8_t sdi, uint8_t sii);
extern void write_lfuse_bits(uint8_t lfuse);
extern void write_hfuse_bits(uint8_t hfuse);
extern void write_efuse_bits(uint8_t efuse);

#endif