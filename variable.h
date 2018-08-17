#ifndef _VARIABLE_H
#define _VARIABLE_H
#include "qcustomplot.h"

#define COUNT_PHI 2
#define DEBUGE true // flag to lot estimation or comparison
#define CONSENSUS true // try not to draw and see how long to take to consensus
#define COUNT 16  // number of agents
#define CALCULATE 20
#define COUNT_LEADER COUNT/CALCULATE + 1  // COUNT / CALCULATE
#define STAY false
#define PURE_FLOCKING true  // flag which define pure flocking or not
#define MY_ESTIMATION true  // use (estimation to leader) or not
#define SCALA 60 // parameter to multiple the arx which is human's input
#define SIZEOF(array, len){len = (sizeof(array) / sizeof(array[0]));}


#endif // _PLOT_H
