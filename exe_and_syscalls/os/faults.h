#ifndef FAULTS_H
#define FAULTS_H

// §6: valores para pcb_ext_table[n].fault_type
#define FAULT_TYPE_NONE          0u
#define FAULT_TYPE_DATA_ABORT    1u
#define FAULT_TYPE_PREFETCH_ABORT 2u

void data_abort_c(unsigned int fault_addr);
void prefetch_abort_c(unsigned int fault_addr);

#endif // FAULTS_H