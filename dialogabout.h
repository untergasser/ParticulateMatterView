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


#ifndef DIALOGABOUT_H
#define DIALOGABOUT_H

#include <QDialog>

namespace Ui {
class DialogAbout;
}

class DialogAbout : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogAbout(QString ver, QWidget *parent = 0);
    ~DialogAbout();

    void setLicense(QString text);
    
private:
    Ui::DialogAbout *ui;
};

#endif // DIALOGABOUT_H
