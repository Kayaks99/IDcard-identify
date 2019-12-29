#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QImage>
#include <QMessageBox>
#include <QScreen>
#include <QPainter>
#include <QFileDialog>
#include <QPixmap>
#include <Python.h>
#include <iostream>
#include <math.h>
#include <QTextCodec>
#include <QThread>
#include <QString>
#include <QProcess>
#include "preprocess.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void showPre();
    void showRet();

    void usePy();
    void usePy2();

private slots:
    void openButton_clicked();
    void runButton_clicked();

    void putText();
    void putHead();

    //void read_slot();

private:
    Ui::MainWindow *ui;
    QImage img;
    QImage tmp;
    QImage preimg;
    int h;

    QString fileName;
    QString pyName;

    QProcess *process;

    preprocess p;

    void labelShow(QImage);

};

#endif // MAINWINDOW_H
