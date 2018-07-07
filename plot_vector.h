#ifndef PLOT_VECTOR_H
#define PLOT_VECTOR_H
#include "qvector.h"
#include "variable.h"
#include "vector"
#include "particle.h"
extern QVector<double> draw_leader_x, draw_leader_y,draw_follower_x, draw_follower_y;
extern QVector<double> draw_leader_xx, draw_leader_xy;
extern QVector<double> draw_follower_xx, draw_follower_xy;
extern int ldemtest;
//extern vector<particle> element;
extern double **desiredPosition;
#endif // PLOT_VECTOR_H
