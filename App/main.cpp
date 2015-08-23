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


#include "mainwindow.h"
#include "bluetooth.h"
#include "currentanalyzer.h"
#include "datapoint.h"

#include <QApplication>
#include <QObject>
#include <QString>


#ifdef Q_OS_ANDROID
    #include <QtAndroidExtras>
#endif

Q_DECLARE_METATYPE( dataPoint )

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qRegisterMetaType<dataPoint>( "dataPoint" );

    bluetooth btDevice;
    mainWindow gui( &btDevice );
    currentAnalyzer dataAnalyzer;

    QObject::connect( &gui, SIGNAL( btConnect( QString ) ), &btDevice,
                      SLOT( connectToDevice( QString ) ) );
    QObject::connect( &gui, SIGNAL( btStop() ), &btDevice, SLOT( stop() ) );
    QObject::connect( &gui, SIGNAL( startDemo() ), &btDevice,
                      SLOT( startDemo() ) );
    QObject::connect( &gui, SIGNAL( stopDemo() ), &btDevice,
                      SLOT( stopDemo() ) );
    QObject::connect( &gui, SIGNAL( saveData( QString, QString ) ),
                      &dataAnalyzer, SLOT( saveData( QString, QString ) ) );
    QObject::connect( &gui, SIGNAL( showReport() ), &dataAnalyzer,
                      SLOT( showReport() ) );
    QObject::connect( &gui, SIGNAL( reset() ), &dataAnalyzer, SLOT( reset() ) );

    QObject::connect( &btDevice, SIGNAL( connectionInfo( QString ) ), &gui,
                      SLOT( updateInfoText( QString ) ) );
    QObject::connect( &btDevice, SIGNAL( connectionStatus( bool ) ), &gui,
                      SLOT( connectionStatus(bool) ) );
    QObject::connect( &btDevice, SIGNAL( messageReceived( QString ) ),
                      &dataAnalyzer, SLOT( newDataString( QString ) ) );

    QObject::connect( &dataAnalyzer, SIGNAL( infoText( QString ) ), &gui,
                      SLOT( updateInfoText( QString ) ) );
    QObject::connect( &dataAnalyzer, SIGNAL( newDataPoint( dataPoint, int ) ),
                      &gui, SLOT( newDataPoint( dataPoint, int ) ) );
    QObject::connect( &dataAnalyzer, SIGNAL( frequency( int, int ) ), &gui,
                      SLOT( updateFrequency( int, int ) ) );
    QObject::connect( &dataAnalyzer, SIGNAL( addCompression( bool ) ), &gui,
                      SLOT( updateCompressionCount( bool ) ) );
    QObject::connect( &dataAnalyzer, SIGNAL( addVentilation( bool ) ), &gui,
                      SLOT( updateVentilationCount( bool ) ) );

    // keep screen on:
    #if defined( Q_OS_ANDROID ) && !defined( QT_DEBUG )
        QAndroidJniObject activity = QtAndroid::androidActivity();
        if( activity.isValid() )
        {
            QAndroidJniObject window = activity.callObjectMethod( "getWindow",
                                           "()Landroid/view/Window;" );
            if( window.isValid() )
            {                
                const int FLAG_KEEP_SCREEN_ON = 128;
                window.callObjectMethod( "addFlags", "(I)V",
                                         FLAG_KEEP_SCREEN_ON );
                //window.callMethod<void>( "addFlags", "(I)V",
                //                         FLAG_KEEP_SCREEN_ON );
            }
        }
    #endif

    gui.showMaximized();

    return app.exec();
}

