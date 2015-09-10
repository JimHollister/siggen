/*
* Functions for handling the DDS chip.
*/

#ifndef DDS_H_
#define DDS_H_

void dds_initialize();
void dds_send_16_bits(uint16_t value);
unsigned long calc_tuning_word_fractional(unsigned long output_freq);
unsigned long calc_tuning_word_integral(unsigned long output_freq);
void dds_test1();
void dds_test2();
void dds_test3();
void dds_test4();
void dds_test5();

#endif /* DDS_H_ */