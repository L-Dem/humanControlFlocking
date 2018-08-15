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
    connect(&dataTimer, &QTimer::timeout, this, &MainWindow::flocking);
//    connect(&dataTimer, &QTimer::timeout, this, &MainWindow::test);
//    connect(&dataTimer, &QTimer::timeout, [&](std::vector<particle>& element){
//    });
    dataTimer.start(200);
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

    while(i < COUNT){
        element[i].compute_phi();
        element[i].find_neighbour(element);
//        element[i].estimation_a();
        element[i].estimation_b();
        element[i].human_control_run(arx, ary);
//        element[i].human_control_run_pure(arx, ary);

        element_b[i].compute_phi();
        element_b[i].find_neighbour(element_b);
//        element_b[i].estimation_a();
        element_b[i].estimation_b();
        element_b[i].human_control_run(arx, ary);
//        element_b[i].human_control_run_pure(arx, ary);

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

//        qDebug() << i<<":X:" << element[i].positionX << endl;
//        qDebug() << i<<":Y:" << element[i].positionY << endl;
        i++;
    }
    ui->customPlot->graph(0)->setData(draw_leader_x, draw_leader_y);
    ui->customPlot->graph(1)->setData(draw_follower_x, draw_follower_y);
    ui->customPlot_x->graph(0)->setData(draw_follower_xx, draw_follower_xy);
    ui->customPlot_x->graph(1)->setData(draw_leader_xx, draw_leader_xy);
    ui->customPlot->xAxis->setScaleRatio(ui->customPlot->yAxis, 1);
    ui->customPlot_x->xAxis->setScaleRatio(ui->customPlot_x->yAxis, 1);
    ui->customPlot->replot();
    ui->customPlot_x->replot();

}

void MainWindow::test(){

    test_count ++;
    qDebug() << test_count << endl;

}

