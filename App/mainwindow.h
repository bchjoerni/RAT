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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_main_portrait.h"
#include "ui_main_landscape.h"

#include "bluetooth.h"
#include "devicesearch.h"
#include "datapoint.h"
#include "settings.h"

#include <QMainWindow>
#include <QTime>
#include <QTimer>
#include <QResizeEvent>
#include <QMessageBox>


namespace Ui
{
    class mainWindow;
}

class mainWindow : public QMainWindow
{
    Q_OBJECT

    const QString STYLE_WRONG = "background-color: red;\n color: black;";
    const QString STYLE_RIGHT = "background-color: lime;\n color: black;";
    const QString STYLE_HALF  = "background-color: yellow;\n color: black;";
    const QString STYLE_NONE  = "";

signals:
    void btConnect( const QString& adress );
    void btStop();
    void saveData( const QString& prefix, QString delimiter );
    void reset();
    void startDemo();
    void stopDemo();
    void showReport();

public:
    explicit mainWindow( bluetooth* _btDevice, QWidget *parent = 0 );
    ~mainWindow();

protected:
    void resizeEvent( QResizeEvent *event );

public slots:
    void updateInfoText( const QString& text );
    void connectionStatus( bool connected );
    void newDataPoint( const dataPoint& dp, int ventilationDuration );
    void updateFrequency( int frequencyNow, int frequencyTotal );
    void updateCompressionCount( bool correct );
    void updateVentilationCount( bool correct );    

private slots:
    void selectDevice();
    void start();
    void stop();
    void save();
    void showSettings();
    void updateClock();
    void initialResize();
    void resetValues();

private:
    void initButtons();
    void initSettingsVar();
    void setDrawSpeed();
    void setEvent( bool compression, int value, int correct );
    void setButtonsStart( bool start );    
    int nextEventNum();
    void getEventLabels( QLabel* text, QLabel* value, QLabel* correct );
    QString getValueText( bool compression, int value );
    QString getValueStyle( bool compression, int value );
    QString getTechniqueText( int correct );
    QString getTechniqueStyle( int correct );
    QString getEventText( bool compression );
    void clearEvents();
    void blankEvent( int eventNum );
    void setEventText( QString type, QString value, QString styleValue,
                       QString correct, QString styleCorrect, int EventNum );
    void getEventLabels( QLabel* text, QLabel* value, QLabel* correct,
                         int eventNum );

    Ui::frm_main_portrait _main_portrait;
    Ui::frm_main_landscape _main_landscape;
    QWidget *_landscapeWidget;
    QWidget *_portraitWidget;
    bool _portraitMode;

    QString _btAdress;
    bluetooth* _bluetooth;
    QTimer _clock;
    int _correctCompressions;
    int _totalCompressions;
    int _correctVentilations;
    int _totalVentilations;
    int _eventNumber;
    int _lastEventValue;
    bool _eventCorrect;
    bool _eventFinished;    
    settingsVar _settings;
};

#endif // MAINWINDOW_H
