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

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    programVersion = "Version 0.9.0 BETA";
    ui->setupUi(this);

    serPM2_5->setName("PM2.5");
    serPM10->setName("PM10");

    QDateTime current = QDateTime::currentDateTime();
    startSecond = current.toSecsSinceEpoch();
    serPM2_5->append(-1, 0);
    serPM10->append(-1, 0);
    serPM2_5->append(0, 0);
    serPM10->append(0, 0);

    QChart *curveChart = new QChart();
    curveChart->addSeries(serPM2_5);
    curveChart->addSeries(serPM10);

    axisX->setLabelFormat("%i");
    axisX->setTitleText("Seconds");
    axisX->setMin(-300.0);
    axisX->setMax(0.0);
    axisX->setReverse(true);
    curveChart->addAxis(axisX, Qt::AlignBottom);
    serPM2_5->attachAxis(axisX);
    serPM10->attachAxis(axisX);

    axisY->setLabelFormat("%i");
    axisY->setTitleText("ug/m3");
    axisY->setMin(0.0);
    axisY->setMax(5.0);
    curveChart->addAxis(axisY, Qt::AlignLeft);
    serPM2_5->attachAxis(axisY);
    serPM10->attachAxis(axisY);

    ui->curveChartView->setChart(curveChart);
    ui->curveChartView->setRenderHint(QPainter::Antialiasing);

    QString serialPortName = "";
    serialPorts = new QList<QString>;
    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &serialPortInfo : serialPortInfos) {
        QString portInfo = serialPortInfo.portName();
        const QStringView description = serialPortInfo.description();
        if(!description.isEmpty()) {
            portInfo += " - ";
            portInfo += description;
        }
        if (serialPortName.isEmpty()) {
            serialPortName = serialPortInfo.portName();
            m_serialPort.setPortName(serialPortName);
            m_serialPort.setBaudRate(QSerialPort::Baud9600);
            if (!m_serialPort.open(QIODevice::ReadOnly)) {
                ui->SensorID->setText("Sensor ID: Error!");
            } else {
                connect(&m_serialPort, &QSerialPort::readyRead, this, &MainWindow::handleReadyRead);
                connect(&m_serialPort, &QSerialPort::errorOccurred, this, &MainWindow::handleReadError);
            }
        }
        serialPorts->append(serialPortInfo.portName());
        ui->portSelection->addItem(portInfo);
    }
}

void MainWindow::on_portSelection_currentIndexChanged(int index)
{
    if (m_serialPort.isOpen()) {
        m_serialPort.close();
    }
    m_serialPort.setPortName(serialPorts->at(index));
    m_serialPort.setBaudRate(QSerialPort::Baud9600);
    if (!m_serialPort.open(QIODevice::ReadOnly)) {
        ui->SensorID->setText("Sensor ID: Error!");
    }
}

void MainWindow::handleReadyRead()
{
    ui->errorNote->setText("");
    readData.append(m_serialPort.readAll());
    if (readData.length() != 10) {
        ui->errorNote->setText("Error: Invalid length.");
        readData.clear();
        return;
    }
    if (readData.at(0) != '\xaa') {
        ui->errorNote->setText("Error: Invalid start byte.");
        readData.clear();
        return;
    }
    if (readData.at(1) != '\xc0') {
        ui->errorNote->setText("Error: Invalid second byte.");
        readData.clear();
        return;
    }
    if (readData.at(9) != '\xab') {
        ui->errorNote->setText("Error: Invalid terminal byte.");
        readData.clear();
        return;
    }
    unsigned long checksum = 0x00;
    for (int i=2; i < 8; i++) {
        checksum = checksum + (static_cast<unsigned int>(readData.at(i)) & 0xFF);
    }
    if ((checksum % 256) != (static_cast<unsigned int>(readData.at(8)) & 0xFF)) {
        ui->errorNote->setText("Error: Checksum not correct.");
    }

    double pm2_5 = ((static_cast<unsigned int>(readData.at(3)) & 0xFF) * 256.0 + ((static_cast<unsigned int>(readData.at(2)) & 0xFF))) / 10.0;
    double pm10 = ((static_cast<unsigned int>(readData.at(5)) & 0xFF) * 256.0 + ((static_cast<unsigned int>(readData.at(4)) & 0xFF))) / 10.0;
    QByteArray idRaw = "";
    idRaw.append(readData.at(6));
    idRaw.append(readData.at(7));
    QString idStr = idRaw.toHex();
    QString pm2_5Str = QString("%1").arg(pm2_5, 0, 'D', 1);
    QString pm10Str = QString("%1").arg(pm10, 0, 'D', 1);
    pm2_5Str.replace('.', ',');
    pm10Str.replace('.', ',');
    QDateTime current = QDateTime::currentDateTime();
    QString measureTime = current.toString("hh:mm:ss");
    qint64 sec = current.toSecsSinceEpoch();

    serPM2_5->append(sec - startSecond, pm2_5);
    serPM10->append(sec - startSecond, pm10);
    axisX->setMin(sec - startSecond - viewSeconds);
    axisX->setMax(sec - startSecond);

    double maxDust = 5.0;
    QList<QPointF> pm2_5Points = serPM2_5->points();
    QList<QPointF> pm10Points = serPM10->points();

    bool keepRemoving = true;
    while (keepRemoving) {
        if (!(pm2_5Points.isEmpty())) {
            QPointF point = pm2_5Points.first();
            if (point.x() < sec - startSecond - viewSeconds) {
                serPM2_5->removePoints(0,1);
                pm2_5Points = serPM2_5->points();
            } else {
                keepRemoving = false;
            }
        } else {
            keepRemoving = false;
        }
    }
    keepRemoving = true;
    while (keepRemoving) {
        if (!(pm10Points.isEmpty())) {
            QPointF point = pm10Points.first();
            if (point.x() < sec - startSecond - viewSeconds) {
                serPM10->removePoints(0,1);
                pm10Points = serPM10->points();
            } else {
                keepRemoving = false;
            }
        } else {
            keepRemoving = false;
        }
    }

    for (int i = 0; i < pm2_5Points.count(); i++) {
        double currDust = pm2_5Points.at(i).y() * 1.1;
        if (maxDust < currDust) {
            maxDust = currDust;
        }
    }
    for (int i = 0; i < pm10Points.count(); i++) {
        double currDust = pm10Points.at(i).y() * 1.1;
        if (maxDust < currDust) {
            maxDust = currDust;
        }
    }
    axisY->setMax(maxDust);

    if (!(saveFileName.isEmpty())) {
        QFile fh(saveFileName);
        if (fh.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&fh);
            out << idStr + "\t";
            out << measureTime + "\t";
            out << pm2_5Str + "\t";
            out << pm10Str + "\t";
            out << commentTxt + "\n";
            fh.close();
            commentTxt = "";
        }
    }

    ui->SensorID->setText(idStr);
    ui->measTime->setText(measureTime);
    ui->pm2_5->setText(pm2_5Str + " µg/m³");
    ui->pm10->setText(pm10Str + " µg/m³");

    readData.clear();
}

void MainWindow::handleReadError() {
    ui->errorNote->setText("Error: Read error.");
}

void MainWindow::on_SaveFile_clicked()
{
    saveFileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Text files (*.tsv)"));
    QFile fh(saveFileName);
    if (fh.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&fh);
        out << "Sensor ID\t";
        out << "Time\t";
        out << "PM2.5\t";
        out << "PM10\t";
        out << "Comment\n";
        fh.close();
    }

}

void MainWindow::on_secondsEdit_textChanged(const QString &seconds)
{
    bool ok = false;
    int readSec = seconds.toInt(&ok);
    if (ok) {
        viewSeconds = readSec;
    }
}

void MainWindow::on_commentButton_clicked()
{
    commentTxt = ui->commentEdit->text();
    ui->commentEdit->setText("");
}

void MainWindow::on_gitHubButton_clicked()
{
    QUrl urlLink("https://github.com/untergasser/ParticulateMatterView");
    QDesktopServices::openUrl(urlLink);
}

void MainWindow::on_licenseButton_clicked()
{
    DialogAbout *about = new DialogAbout(programVersion, this);
    about->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
