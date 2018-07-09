#ifndef PARTICLE_H
#define PARTICLE_H
#include "vector"
#include "variable.h"



using namespace std;
class particle
{
public:
    int id;
    int leader_id;
    int count_phi = COUNT_PHI;
    double desiredMoment[COUNT_LEADER][COUNT_PHI];
    float r = 0;
    vector<particle> neighbor;  //neighbor of this particle


//    int gamma[COUNT_PHI][COUNT_PHI] = {0};
    double dt = 0.1;
    double positionX;
    double positionY;
    double position[2];
    double acceleration[2] = {0, 0};
    double velocity[2] = {0, 0};
    double velocityX = velocity[0];
    double velocityY = velocity[1];
    int r_com = 15;  // communicate radius
//    static vector<double> omega;
    double *omega = new double[count_phi]();
    double *omega_d = new double[count_phi]();
    int **gamma = new int*[count_phi]();
    double *x = new double[count_phi]();  // state of each agent
    double *x_before = new double[count_phi]();
    double *x_d = new double[count_phi]();
    double *phi = new double[count_phi]();  // moment of each agent
    double **phi_d =  new double*[2]();
    /*try to find how many leaders */
    bool is_leader;  // leader or not flag
    double order = 2;
    /*control parameters*/
    int r_obs = 7;
    double c1 = 0.5;
    double c2 = 0.5;
    double epsilon = 0.5;
    double h = 0.2;
    double B = 5;

    particle(int count, double **moment);
    void done_all();
//    ~particle();
    void estimation_a();
    void estimation_b();
//    estimation_b();
    void compute_phi();
    void find_neighbour(vector<particle> particle_list);
    void control_run();
    void human_control_run_pure(double arx, double ary);
    void human_control_run(double arx, double ary);
    double phi_alpha(double z);
    double phi_h(double z);
    double sigma1(double z);

private:
    int time_count;


};

#endif // PARTICLE_H
