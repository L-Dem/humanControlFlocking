#ifndef _VARIABLE_H
#define _VARIABLE_H
#include "qcustomplot.h"

#define COUNT_PHI 2

#define COUNT 16  // number of agents
#define CALCULATE 20
#define COUNT_LEADER COUNT/CALCULATE + 1  // COUNT / CALCULATE
#define STAY false
#define PURE_FLOCKING true
#define MY_ESTIMATION true  // use (estimation to leader) or not
#define SIZEOF(array, len){len = (sizeof(array) / sizeof(array[0]));}


#endif // _PLOT_H
