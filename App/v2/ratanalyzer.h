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

#ifndef RATANALYZER_H
#define RATANALYZER_H

#include "datapoint.h"

// #include <QThread>
// #include <QMutex>
#include <QObject>
#include <QDateTime>
#include <QTime>

#include <vector>

class ratAnalyzer : public QObject //public QThread - does not work on Android
{
    Q_OBJECT

public:
    ratAnalyzer();
    ratAnalyzer( std::vector<dataPoint>* data );
    ~ratAnalyzer();
    void resetAnalyzer();

protected:
    virtual void updateFrequency();
    virtual void updateCompressionCount( bool correct );
    virtual void updateVentilationCount( bool correct );
    virtual void updateVentilationDuration( int duration );

    void analyzeCompression();
    void analyzeVentilation();
    void analyzeVentilationVolume();
    void analyzeVentilationTime();

    void compression0To0();
    void compression0ToNon0();
    void compressionNon0To0();
    void compressionNon0ToNon0();

    void ventilation0ToNon0();
    void ventilationNon0To0();
    void ventilationNon0ToNon0();

    const double _minFrequency = 10;
    const double _maxFrequency = 250;
    const unsigned int _compTimeoutMs = 1500;
    const unsigned int _maxVentTimeMs = 2500;

    const int _compLimit1 = 5;
    const int _compLimit2 = 40;
    const int _compLimit3 = 50;
    const int _compLimit4 = 60;

    const int _ventLimit1 = 5;
    const int _ventLimit2 = 50;
    const int _ventLimit3 = 80;
    const int _ventLimit4 = 120;

    std::vector<dataPoint> _dataVector;
    std::vector<dataPoint> *_data;
    dataPoint* _currentData;
    dataPoint* _previousData;

    QDateTime _startDateTime;
    QTime _startCompression;
    QTime _startVentilation;
    bool _inCycle;
    bool _ppointOkay;
    bool _depthReached;
    bool _enoughVolume;
    bool _volumeTooHigh;
    double _frequencyNow;
    double _frequencyTotal;
    double _frequencyCount;
    unsigned int _ventilationDuration;
};

#endif // RATANALYZER_H
