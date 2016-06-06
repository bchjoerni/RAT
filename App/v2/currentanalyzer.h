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

#ifndef CURRENTANALYZER_H
#define CURRENTANALYZER_H

#include "ratanalyzer.h"
#include "finalanalyzer.h"
#include "result.h"

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDir>

class currentAnalyzer : public ratAnalyzer
{
    Q_OBJECT

public:
    currentAnalyzer();
    ~currentAnalyzer();
    void run(); // Q_DECL_OVERRIDE;

signals:
    void infoText( const QString& text );
    void newDataPoint( const dataPoint& dp, int ventilationTime );    
    void frequency( int frequencyNow, int frequencyTotal );
    void addCompression( bool correct );
    void addVentilation( bool correct );
    void ventilationDuration( int duration );

public slots:
    void newDataString( const QByteArray& data );
    void saveData( const QString& prefix, QString delimiter );
    void reset();
    void showReport();

private:
    void decodeDataPoint( const QByteArray& dp );
    bool sensorDataEqual( const dataPoint& p1, const dataPoint& p2 );
    void updateFrequency() Q_DECL_OVERRIDE;
    void updateCompressionCount( bool correct ) Q_DECL_OVERRIDE;
    void updateVentilationCount( bool correct ) Q_DECL_OVERRIDE;
    void updateVentilationDuration( int duration ) Q_DECL_OVERRIDE;
    QString getSavePath();

    // QMutex _mutex;
    dataPoint _tempDataPoint;
    QTime _startTime;
    int _startMillis;
    bool _lastVentSetZero; // to eliminate wrong vent counts

    const int MIN_VENTILATION_PAUSE_MS = 10;
};

#endif // CURRENTANALYZER_H
