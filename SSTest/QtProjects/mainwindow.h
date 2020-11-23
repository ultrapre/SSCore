#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QDebug>
//  SSTest.cpp
//  SSCore
//
//  Created by Tim DeBenedictis on 2/24/20.
//  Copyright © 2020 Southern Stars. All rights reserved.

#include <cstdio>

#if defined __APPLE__
#include <TargetConditionals.h>
#endif

#ifdef _WIN32
#include <Windows.h>
#endif

#include "SSCoordinates.hpp"
#include "SSOrbit.hpp"
#include "SSPlanet.hpp"
#include "SSStar.hpp"
#include "SSConstellation.hpp"
#include "SSImportHIP.hpp"
#include "SSImportSKY2000.hpp"
#include "SSImportNGCIC.hpp"
#include "SSImportMPC.hpp"
#include "SSImportGJ.hpp"
#include "SSJPLDEphemeris.hpp"
#include "SSTLE.hpp"
#include "SSEvent.hpp"
#include "VSOP2013.hpp"
#include "ELPMPP02.hpp"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void spendtime();
    void CometsListShow( string inputDir, string outputDir );
//public slots:
//    void TestDeepSkyNew ();

public slots:
    void update();
//signals:
////    // 信号与槽关联
//    void sigtest();
private slots:
    void updateUI();
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_18_clicked();

    void on_pushButton_19_clicked();

    void on_pushButton_20_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H


bool copyDirectoryFiles(const QString &fromDir, const QString &toDir, bool coverFileIfExist=true);
