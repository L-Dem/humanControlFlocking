#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <qcustomplot.h>
#include "particle.h"
#include "qcustomplot.h"
#include "plot_vector.h"
#include "unistd.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void updatePlot();
private slots:
    void flocking();
    void test();


private:
    Ui::MainWindow *ui;
    QTimer dataTimer;
    vector<particle> element;
    vector<particle> element_b;
    int count_calculate_time;
    int write_flag;
    int test_count;
};

#endif // MAINWINDOW_H
