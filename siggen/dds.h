/*
* Functions for handling the DDS chip.
*/

#ifndef DDS_H_
#define DDS_H_

void dds_initialize();
void dds_set_frequency_integral(unsigned long frequency);
void dds_set_frequency_fractional(unsigned long frequency);
void dds_test1();
void dds_test2();
void dds_test3();
void dds_test4();
void dds_test5();

#endif /* DDS_H_ */