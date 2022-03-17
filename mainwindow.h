/****************************************************************************
**
**  (c) Copyright 2022 by Dr. Andreas Untergasser
**      All rights reserved.
**
**  This file is part of ParticulateMatterView.
**
**  ParticulateMatterView is free software: you can redistribute it and/or
**  modify it under the terms of the GNU General Public License as published
**  by the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  ParticulateMatterView is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with ParticulateMatterView.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopServices>
#include <QFileDialog>
#include <QDateTime>
#include <QByteArray>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include "dialogabout.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleReadyRead();
    void handleReadError();
    void on_SaveFile_clicked();
    void on_secondsEdit_textChanged(const QString &arg1);
    void on_commentButton_clicked();
    void on_licenseButton_clicked();
    void on_gitHubButton_clicked();

    void on_portSelection_currentIndexChanged(int index);

private:
    QSerialPort m_serialPort;
    QList<QString> *serialPorts;
    QByteArray readData;
    QLineSeries *serPM2_5 = new QLineSeries();
    QLineSeries *serPM10 = new QLineSeries();
    qint64 viewSeconds = 300;
    qint64 startSecond = 0;
    QValueAxis *axisX = new QValueAxis;
    QValueAxis *axisY = new QValueAxis;
    QString saveFileName = "";
    QString commentTxt = "";
    QString programVersion;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
