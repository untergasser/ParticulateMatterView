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

#include "dialogabout.h"
#include "ui_dialogabout.h"

DialogAbout::DialogAbout(QString ver, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAbout)
{
    ui->setupUi(this);
    ui->labelVersion->setText(ver);
}

DialogAbout::~DialogAbout()
{
    delete ui;
}

void DialogAbout::setLicense(QString text)
{
    ui->plainTextEdit->setPlainText(text);
}
