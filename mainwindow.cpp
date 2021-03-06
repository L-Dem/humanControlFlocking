#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>
#include <qcustomplot.h>
#include <variable.h>
#include <plot_vector.h>
#include "iostream"
#include "variable.h"
#include "plot_vector.h"
#include "particle.h"
#include "gamepad.hpp"
#include "iostream"
#include "fstream"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    test_count = 0;
    QVector<double> draw_leader_x(COUNT_LEADER), draw_leader_y(COUNT_LEADER);
    QVector<double> draw_follower_x(COUNT), draw_follower_y(COUNT);
    QVector<double> draw_leader_xx(COUNT_LEADER), draw_leader_xy(COUNT_LEADER);
    QVector<double> draw_follower_xx(COUNT), draw_follower_xy(COUNT);

    int count = COUNT;  // create agent number
    double position[3][2] = {
        {10, 50},
        {10, 45},
        {10, 40}
    };
    double **desiredPosition = new double* [COUNT_LEADER]();
    for(int i = 0; i < COUNT_LEADER; i ++){
        desiredPosition[i] = new double[2]();
        desiredPosition[i] = position[i];
    }
    int i = 0;
    element.clear();
    element_b.clear();
    while(i < count){
        particle my_particle(i, desiredPosition);
        particle my_particle_b(i,desiredPosition);
        usleep(500);
        element.push_back(my_particle);  // create many particles
        element_b.push_back(my_particle_b);
        i++;
    }
    ofstream outfile;
    outfile.open("../result.txt", ios::out|ios::trunc);
    outfile << "flocking c2:" << element[0].c2 << endl;
    outfile.close();
    count_calculate_time = 0;
    write_flag = 0;
    if(CONSENSUS == false){// draw plot
        control = 200;
        for(int i = 0; i < COUNT; i++){
            if(element[i].is_leader){
                draw_leader_x.append(element[i].positionX);
                draw_leader_y.append(element[i].positionY);
                if(DEBUGE == true){
                   draw_leader_xx.append(element[i].x[0]);
                   draw_leader_xy.append(element[i].x[1]);
                }
            }
            else{
                draw_follower_x.append(element[i].positionX);
                draw_follower_y.append(element[i].positionY);
                if(DEBUGE == true){
                   draw_follower_xx.append(element[i].x[0]);
                   draw_follower_xy.append(element[i].x[1]);
                }
            }
            if(DEBUGE == false){
                if(element_b[i].is_leader){
                    draw_leader_xx.append(element_b[i].positionX);
                    draw_leader_xy.append(element_b[i].positionY);
                }else{
                    draw_follower_xx.append(element_b[i].positionX);
                    draw_follower_xy.append(element_b[i].positionY);
                }
            }

        }
        ui->customPlot->addGraph();
        ui->customPlot->graph(0)->setPen(QPen(Qt::blue));
        ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
        ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
        ui->customPlot->graph(0)->setData(draw_leader_x, draw_leader_y);
        ui->customPlot->addGraph();
        ui->customPlot->graph(1)->setPen(QPen(Qt::red));
        ui->customPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
        ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
        ui->customPlot->graph(1)->setData(draw_follower_x, draw_follower_y);

        ui->customPlot_x->addGraph();
        ui->customPlot_x->graph(0)->setPen(QPen(Qt::red));
        ui->customPlot_x->graph(0)->setLineStyle(QCPGraph::lsNone);
        ui->customPlot_x->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
        ui->customPlot_x->graph(0)->setData(draw_follower_xx, draw_follower_xy);

        ui->customPlot_x->addGraph();
        ui->customPlot_x->graph(1)->setPen(QPen(Qt::blue));
        ui->customPlot_x->graph(1)->setLineStyle(QCPGraph::lsNone);
        ui->customPlot_x->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
        ui->customPlot_x->graph(1)->setData(draw_leader_xx, draw_leader_xy);

        ui->customPlot_x->xAxis->setLabel("x");
        ui->customPlot_x->yAxis->setLabel("y");
        ui->customPlot_x->xAxis->setRange(-10, 60);
        ui->customPlot_x->yAxis->setRange(-10, 60);
        ui->customPlot_x->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
        ui->customPlot->xAxis->setLabel("x");
        ui->customPlot->yAxis->setLabel("y");
        ui->customPlot->xAxis->setRange(-10, 60);
        ui->customPlot->yAxis->setRange(-10, 60);
        ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

        ui->customPlot->xAxis->setScaleRatio(ui->customPlot->yAxis, 1);
        ui->customPlot_x->xAxis->setScaleRatio(ui->customPlot_x->yAxis, 1);
        ui->customPlot->replot();
        ui->customPlot_x->replot();
    }else{
        control = 20;
    }

    connect(&dataTimer, &QTimer::timeout, this, &MainWindow::flocking);
//    connect(&dataTimer, &QTimer::timeout, this, &MainWindow::test);
//    connect(&dataTimer, &QTimer::timeout, [&](std::vector<particle>& element){
//    });
    dataTimer.start(control); //200
}

MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::flocking(){
    int i = 0;
    /*2.compute each episode
     *  by ldem*/
    QVector<double> draw_leader_x(COUNT_LEADER), draw_leader_y(COUNT_LEADER);
    QVector<double> draw_follower_x(COUNT), draw_follower_y(COUNT);
    QVector<double> draw_leader_xx(COUNT_LEADER), draw_leader_xy(COUNT_LEADER);
    QVector<double> draw_follower_xx(COUNT - COUNT_LEADER), draw_follower_xy(COUNT - COUNT_LEADER);
    draw_leader_x.clear();
    draw_leader_y.clear();
    draw_leader_xx.clear();
    draw_leader_xy.clear();
    draw_follower_x.clear();
    draw_follower_y.clear();
    draw_follower_xx.clear();
    draw_follower_xy.clear();
    gamepad::Joystick human;
    human.connect();
    human.read();
    double arx = 0;
    double ary = 0;
    if(human.B() == true){
        arx = human.AxisLS().first;
        ary = human.AxisLS().second;
        cout << human.AxisLS().first << "::"<< human.AxisLS().second << endl;
    }

    double err_sum = 0;  // calculate error
    double threshold = 0.005;
    double standard_x, standard_y = 0; // consus result//leader
    while(i < COUNT){
            if(element[i].is_leader == true){
                standard_x = element[i].positionX;
                standard_y = element[i].positionY;
            }
        i++;
    }
//    int count_leader = COUNT_LEADER;
//    standard_x = standard_x / count_leader;
//    standard_y = standard_y / count_leader;
    i = 0;
    while(i < COUNT){

        element[i].compute_phi();
        element[i].find_neighbour(element);
//        element[i].estimation_a();
//        element[i].estimation_b();
//        element[i].estimation_b(arx, ary);
//        element[i].estimation_c();
        element[i].estimation_v(arx, ary);
        element[i].flocking_velocity();
//        element[i].human_control_run(arx, ary);
//        element[i].human_control_run_pure(arx, ary);

        element_b[i].compute_phi();
        element_b[i].find_neighbour(element_b);
        element_b[i].estimation_a();
//        element_b[i].estimation_b();
//        element_b[i].human_control_run(arx, ary);
        element_b[i].human_control_run_pure(arx, ary);

        if(CONSENSUS == true){
            //try to write file

            err_sum = err_sum + sqrt(  pow((standard_x - element[i].x[0]), 2.0) + pow((standard_y - element[i].x[1]), 2.0) );


        }else{
            if(element[i].is_leader){
                draw_leader_x.append(element[i].positionX);
                draw_leader_y.append(element[i].positionY);
                if(DEBUGE ==true){
                    draw_leader_xx.append(element[i].x[0]);
                    draw_leader_xy.append(element[i].x[1]);
                }

            }
            else{
                draw_follower_x.append(element[i].positionX);
                draw_follower_y.append(element[i].positionY);
                if(DEBUGE ==true){
                    draw_follower_xx.append(element[i].x[0]);
                    draw_follower_xy.append(element[i].x[1]);
                }

            }
            if(DEBUGE == false){
                if(element_b[i].is_leader){
                    draw_leader_xx.append(element_b[i].positionX);
                    draw_leader_xy.append(element_b[i].positionY);
                }else{
                    draw_follower_xx.append(element_b[i].positionX);
                    draw_follower_xy.append(element_b[i].positionY);
                }
            }
        }


        i++;
    }
    if(CONSENSUS == false){
        //draw customPlot
        ui->customPlot->graph(0)->setData(draw_leader_x, draw_leader_y);
        ui->customPlot->graph(1)->setData(draw_follower_x, draw_follower_y);
        ui->customPlot_x->graph(0)->setData(draw_follower_xx, draw_follower_xy);
        ui->customPlot_x->graph(1)->setData(draw_leader_xx, draw_leader_xy);
        ui->customPlot->xAxis->setScaleRatio(ui->customPlot->yAxis, 1);
        ui->customPlot_x->xAxis->setScaleRatio(ui->customPlot_x->yAxis, 1);
        ui->customPlot->replot();
        ui->customPlot_x->replot();
    }else{
        //write files to consensus  and calculate how long it costs the program to consensus

        count_calculate_time ++;
        if(err_sum > threshold){//40s
            ofstream outfile;
            outfile.open("../result.txt", ios::app);
            outfile << "time:" << count_calculate_time << "err_sum:" << err_sum << endl;
            outfile.close();
        }else{
            ofstream outfile;
            outfile.open("../result.txt", ios::app);
            outfile << "consensus---" << err_sum << endl;
            outfile.close();
            if(write_flag == 0){
                cout << "" << endl;
            }else{
                cout << "stop write file" << endl;
            }
            write_flag = 1;
        }

    }
}

void MainWindow::test(){

    test_count ++;
    qDebug() << test_count << endl;

}

