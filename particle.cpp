#include "particle.h"
#include "vector"
#include "math.h"
#include <string.h>
#include <ctime>
#include <iostream>



using namespace std;
particle::particle(int count, double **moment)  // moment=[leader's number][moment's number]
{
    id = count;
//    count_phi = sizeof(moment[0]) / sizeof(moment[0][0]);

    count_phi = COUNT_PHI;
//    int test_leader = COUNT_LEADER;
    for(int i = 0; i < COUNT_LEADER; i++){
        for(int j = 0; j < count_phi; j++){
            desiredMoment[i][j] = moment[i][j];
        }
    }
    omega[count_phi] = {0};
    omega_d[count_phi] = {0};
    for(int i = 0; i < count_phi; i++){
        gamma[i] = new int[count_phi]();
    }
    for(int i = 0; i < 2; i++){
        phi_d[i] = new double[count_phi]();
    }
//    srand((unsigned)time(NULL)); // need time exceed 1s;
//    time_count++;
//    srand((unsigned)time_count);
    srand((unsigned)id);
    positionX = (rand() % 6 + 5);
    positionY = (rand() % 6 + 5);

    for(int i = 0; i < count_phi; i ++){
        for(int j = 0;j < count_phi; j ++){
            if(i == j){
                gamma[i][j] = 1;
            }
        }
    }
    /*try to find how many leaders */
    if(COUNT_LEADER != 1){
        if(id % CALCULATE == 0){
            is_leader = true;
        }else{
            is_leader = false;
        }
    }else{
        if(id == 0){
            is_leader = true;
        }else{
            is_leader = false;
        }
    }
    if (is_leader == true){
        leader_id = id / CALCULATE;
        if(STAY == true){
            positionX = desiredMoment[leader_id][0];
            positionY = desiredMoment[leader_id][1];
        }
    }else{
        leader_id = -1;
    }
    position[0] = positionX;  // position matrix
    position[1] = positionY;
}

void particle::done_all(){
    cout << "function destructor" << endl;
    for(int i = 0; i < count_phi; i++){
        delete [] gamma[i];
        delete [] phi_d[i];
    }
    delete [] gamma;
    delete [] x;
    delete [] x_before;
    delete [] x_d;
    delete [] phi;
    delete [] phi_d;
    delete [] omega;
    delete [] omega_d;
}

void particle::estimation_a(){
    r = 0.001;
    int len_x = count_phi;
    for(int k= 0; k < len_x; k++){
        x_before[k] = x[k];
    }

    vector<double> middle(count_phi);
    int len = count_phi;


    if(MY_ESTIMATION){
        if(is_leader){
            for(int k = 0; k < len; k++){
                x[k] = phi[k];
            }
        }else{
            for(unsigned int j = 0; j < neighbor.size(); j++){
                double a = 0.5;
                for(int k = 0; k < len; k++){
                    middle[k] = middle[k] + a * (x[k] - neighbor[j].x[k]);
                }
            }
            for(int k = 0; k < len; k++){
                omega_d[k] = -r * omega[k] - middle[k];
                omega[k] = omega[k] + omega_d[k] * dt;
                x[k] = omega[k] + phi[k];
            }
        }
    }else{
        for(unsigned int j = 0; j < neighbor.size(); j++){
            double a = 0.5;
            for(int k = 0; k < len; k++){
                middle[k] = middle[k] + a * (x[k] - neighbor[j].x[k]);
            }
        }
        for(int k = 0; k < len; k++){
            omega_d[k] = -r * omega[k] - middle[k];
            omega[k] = omega[k] + omega_d[k] * dt;
            x[k] = omega[k] + phi[k];
        }
    }
    for(int k = 0; k < len; k++){
        x_d[k] = x[k] - x_before[k];
    }
}

/*compute each agent's Jacobi matrix*/
void particle::compute_phi(){
    int count = 0;
//    int len_phi, len_phi_d;
//    SIZEOF(phi, len_phi);
//    SIZEOF(phi_d[0], len_phi_d);
//    for(int i = 0; i < len_phi; i++){
//        phi[i] = 0;
//    }
//    for(int i = 0; i < 2; i++){
//        for(int j = 0; j < len_phi_d; j++){
//            phi_d[i][j] = 0;
//        }
//    }
    while(true){
        for(int k = 1; k < 10; k++){
            int j = 0;
            while(j<=k){
                phi[count] = pow(positionX, (k-j)) * pow(positionY, j);
                phi_d[0][count] = (k - j) * pow(positionX, (k -j -1)) * pow(positionY, j);
                double fine1 = pow(positionY, (j - 1));
                double fine2 = pow(positionX, (k - j));
                double fine3 = j * fine1 * fine2;
                phi_d[1][count] = fine3;
                count ++;
                j ++;
                if(count == count_phi)
                    break;
            }
            if(count == count_phi)
                break;
        }
        if(count == count_phi)
            break;
    }
}

/*find agents' neighbor*/
void particle::find_neighbour(vector<particle> particle_list){  //rewrite
    neighbor.clear();
    double distance_x, distance_y, distance;

    for(unsigned int i = 0;i < particle_list.size(); i++){
        distance_x = (particle_list)[i].position[0] - position[0];
        distance_y = (particle_list)[i].position[1] - position[1];
        distance = sqrt(pow(distance_x, 2) + pow(distance_y, 2));
        int r_communicate = r_com;
        if(distance != 0 && distance < r_communicate){
            (particle_list)[i].neighbor.clear();
            neighbor.push_back((particle_list)[i]);
        }
    }
}

/*control*/
void particle::control_run(){  // debug
    int len = count_phi;
    if(is_leader == false){
        int count_nei = neighbor.size();
        double n[count_nei][COUNT_PHI] = {0};
        double u1[len], u2[len], u3[len] = {0};
        double distance, distance2;
        for(int i = 0;i < count_nei; i++){
            distance2 = pow(neighbor[i].phi[0] - phi[0], order) + pow(neighbor[i].phi[1] - phi[1], order);
            distance = sqrt(distance2);
            for(int k = 0; k < len; k++){
                n[i][k] = (neighbor[i].phi[k] - phi[k]) / sqrt(1 + epsilon * distance2);
                u1[k] = phi_alpha(distance) * n[i][k] + u1[k];
                u2[k] = 0.3 * (neighbor[i].velocity[k] - velocity[k]) + u2[k];
            }
        }
        for(int k = 0; k < len; k ++){
            u3[k] = -c1 * (position[k] - x[k]) - c2 * (velocity[k] - x_d[k]);
        }
        for(int k = 0; k < len; k++){
            if(PURE_FLOCKING == false){
                acceleration[k] = u1[k] + u2[k] + u3[k];
            }else {
                acceleration[k] = u1[k] + u2[k];
            }
        }
    }else{
        double acceleration_x, acceleration_y = 0;
        double middle[2][COUNT_PHI] = {0};
        double last[2] = {0};
        for(int i = 0; i < 2; i ++){  //2_n * n_n = 2_n
            for(int j = 0;j < count_phi; j++){
                for(int k = 0; k < count_phi; k++){
                    middle[i][j] = middle[i][j] + phi_d[i][k]*gamma[k][j];
                }
            }
        }
        for(int i = 0; i < 2; i ++){  // 2_n * n_1 = 2*1
            for(int j = 0;j < count_phi; j++){
                    last[i] = last[i] + middle[i][j] * (desiredMoment[leader_id][j] - x[j]);
            }
        }
        acceleration_x = -B * velocityX + last[0];
        acceleration_y = -B * velocityY + last[1];
        acceleration[0] = acceleration_x;
        acceleration[1] = acceleration_y;
    }
    velocityX = velocityX + acceleration[0] * dt;
    velocityY = velocityY + acceleration[1] * dt;
    velocity[0] = velocityX;
    velocity[1] = velocityY;
    position[0] = position[0] + velocity[0] * dt;
    position[1] = position[1] + velocity[1] * dt;
    positionX = position[0];
    positionY = position[1];
}




double particle::phi_alpha(double z){  // potential function
    double a = 6;
    double b = 8;
    double c = abs(a - b) / sqrt(4 *a *b);
    double x1 = phi_h(z / r_obs);
    double middle1 = (a + b) * sigma1(z - r_obs + c);
    double middle2 = middle1 / 2;
    double x2 = middle2 + b - a;
    double y = x1 * x2;
    if (z > r_com){
        y = 0;
    }
    return y;
}

double particle::phi_h(double z){
    double y = 0;
    if(z>=0 && z < h){
        y = 1;
    }else if(z>= h && z < 1){
        y = (1 + cos(3.14 * (z - h)/(1 - h))) / 2;
    }else{
        y = 0;
    }
    return y + 0.3;
}

double particle::sigma1(double z){
    double y = z / sqrt(1 + pow(z, 2));
    return y;
}

void particle::human_control_run(double arx, double ary){
    int len = count_phi;
    if(is_leader == false){
        int count_nei = neighbor.size();
        double n[count_nei][COUNT_PHI] = {0};
        double u1[len], u2[len], u3[len] = {0};
        double distance, distance2;
        for(int i = 0;i < count_nei; i++){
            distance2 = pow(neighbor[i].phi[0] - phi[0], order) + pow(neighbor[i].phi[1] - phi[1], order);
            distance = sqrt(distance2);
            for(int k = 0; k < len; k++){
                n[i][k] = (neighbor[i].phi[k] - phi[k]) / sqrt(1 + epsilon * distance2);
                u1[k] = phi_alpha(distance) * n[i][k] + u1[k];
                u2[k] = 0.3 * (neighbor[i].velocity[k] - velocity[k]) + u2[k];
            }
        }
        for(int k = 0; k < len; k ++){
            u3[k] = -c1 * (position[k] - x[k]) - c2 * (velocity[k] - x_d[k]);
        }
        for(int k = 0; k < len; k++){
//            if(PURE_FLOCKING == false){
                acceleration[k] = u1[k] + u2[k] + u3[k];
//            }else {
//                acceleration[k] = u1[k] + u2[k];
//            }
        }
        velocityX = velocityX + acceleration[0] * dt;
        velocityY = velocityY + acceleration[1] * dt;
        velocity[0] = velocityX;
        velocity[1] = velocityY;
        position[0] = position[0] + velocity[0] * dt;
        position[1] = position[1] + velocity[1] * dt;
        positionX = position[0];
        positionY = position[1];
    }else{

        velocityX = arx * 10;
        velocityY = ary * 10;
        velocity[0] = velocityX;
        velocity[1] = velocityY;
        position[0] = position[0] + velocity[0] * dt;
        position[1] = position[1] + velocity[1] * dt;
        positionX = position[0];
        positionY = position[1];
    }

}


void particle::human_control_run_pure(double arx, double ary){
    int len = count_phi;
    if(is_leader == false){
        int count_nei = neighbor.size();
        double n[count_nei][COUNT_PHI] = {0};
        double u1[len], u2[len], u3[len] = {0};
        double distance, distance2;
        for(int i = 0;i < count_nei; i++){
            distance2 = pow(neighbor[i].phi[0] - phi[0], order) + pow(neighbor[i].phi[1] - phi[1], order);
            distance = sqrt(distance2);
            for(int k = 0; k < len; k++){
                n[i][k] = (neighbor[i].phi[k] - phi[k]) / sqrt(1 + epsilon * distance2);
                u1[k] = phi_alpha(distance) * n[i][k] + u1[k];
                u2[k] = 0.3 * (neighbor[i].velocity[k] - velocity[k]) + u2[k];
            }
        }
        for(int k = 0; k < len; k ++){
            u3[k] = -c1 * (position[k] - x[k]) - c2 * (velocity[k] - x_d[k]);
        }
        for(int k = 0; k < len; k++){
//            if(PURE_FLOCKING == false){
//                acceleration[k] = u1[k] + u2[k] + u3[k];
//            }else {
                acceleration[k] = u1[k] + u2[k];
//            }
        }
        velocityX = velocityX + acceleration[0] * dt;
        velocityY = velocityY + acceleration[1] * dt;
        velocity[0] = velocityX;
        velocity[1] = velocityY;
        position[0] = position[0] + velocity[0] * dt;
        position[1] = position[1] + velocity[1] * dt;
        positionX = position[0];
        positionY = position[1];
    }else{

        velocityX = arx * 10;
        velocityY = ary * 10;
        velocity[0] = velocityX;
        velocity[1] = velocityY;
        position[0] = position[0] + velocity[0] * dt;
        position[1] = position[1] + velocity[1] * dt;
        positionX = position[0];
        positionY = position[1];
    }

}