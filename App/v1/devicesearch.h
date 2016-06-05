/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Copyright 2015 bchjoerni
*/

#ifndef DEVICESEARCH_H
#define DEVICESEARCH_H

#include "bluetooth.h"

#include <QDialog>
#include <QString>
#include <QListWidgetItem>

#include <vector>

namespace Ui
{
    class deviceSearch;
}

class deviceSearch : public QDialog
{
    Q_OBJECT

public:
    explicit deviceSearch( QWidget *parent = 0 );
    ~deviceSearch();
    void init( QString* btAdress, bluetooth* btDevice );

private slots:
    void startScan();
    void deviceSelectionChanged();
    void connectToSelection();
    void connectToDefault();
    void demoCheck();

public slots:
    void deviceFound( const QString& nameAndAdress );
    void scanFinished();

private:    
    Ui::deviceSearch *_ui;
    bluetooth* _btDevice;
    QString* _btAdress;
    int _demoCounter;
};

#endif // DEVICESEARCH_H
