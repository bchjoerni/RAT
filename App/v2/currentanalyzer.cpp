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


#include "currentanalyzer.h"

currentAnalyzer::currentAnalyzer()
{
}

currentAnalyzer::~currentAnalyzer()
{
}

void currentAnalyzer::newDataString( const QByteArray& data )
{
    if( data.startsWith( "d" ) )
    {
        decodeDataPoint( data );        
        run();
    }
    else if( data.startsWith( "r" ) )
    {
        if( data.startsWith( "rstart" ) )
        {
            _startTime = QTime::currentTime();
            int timeBegin = data.indexOf( ':' );

            char t1 = data.at( timeBegin+1 );
            char t2 = data.at( timeBegin+2 );
            char t3 = data.at( timeBegin+3 );
            char t4 = data.at( timeBegin+4 );

            _startMillis = (t1-32)*95*95*95 + (t2-32)*95*95
                    + (t3-32)*95 + (t4-32);
        }
    }
    else if( data.startsWith( "u" ) )
    {
        emit infoText( "Achtung: unbekannter Befehl gesendet!" );
    }
    else
    {
        emit infoText( "Ungültiges Übertragunsprotokoll! "
                       "(Alte Programmversion?)" );
    }
}

void currentAnalyzer::decodeDataPoint( const QByteArray& dp )
{
    char comp = dp.at( 1 ) - 32;
    char vent = dp.at( 2 ) - 32;
    _tempDataPoint.compPosition = dp.at( 3 ) - 32;
    char t1 = dp.at( 4 );
    char t2 = dp.at( 5 );
    char t3 = dp.at( 6 );
    char t4 = dp.at( 7 );

    if( comp > _compLimit1 && comp < _compLimit2 )
    {
        _tempDataPoint.compDepth = 1;
    }
    else if( comp >= _compLimit2 && comp < _compLimit3 )
    {
        _tempDataPoint.compDepth = 2;
    }
    else if( comp >= _compLimit3 && comp <= _compLimit4 )
    {
        _tempDataPoint.compDepth = 3;
    }
    else
    {
        _tempDataPoint.compDepth = 0;
    }

    if( vent > _ventLimit1 && vent < _ventLimit2 )
    {
        _tempDataPoint.ventVolume = 1;
    }
    else if( vent >= _ventLimit2 && vent <= _ventLimit3 )
    {
        _tempDataPoint.ventVolume = 2;
    }
    else if( vent > _ventLimit3 && vent < _ventLimit4 )
    {
        _tempDataPoint.ventVolume = 3;
    }
    else
    {
        _tempDataPoint.ventVolume = 0;
    }

    int millis = (t1-32)*95*95*95 + (t2-32)*95*95 + (t3-32)*95 + (t4-32);
    _tempDataPoint.time = _startTime.addMSecs( millis - _startMillis );
}

void currentAnalyzer::run()
{
    bool sensorDataDifferent = true;

    if( _dataVector.size() > 0 )
    {
        _previousData = &(_dataVector[_dataVector.size()-1]);
        _currentData  = &_tempDataPoint;
        analyzeCompression();
        analyzeVentilation();

        sensorDataDifferent = !sensorDataEqual(
                    _dataVector[_dataVector.size()-1], _tempDataPoint );
    }

    if( sensorDataDifferent )
    {
        _dataVector.push_back( _tempDataPoint );
        _data = &_dataVector;
    }
    // _mutex.unlock();
}

void currentAnalyzer::updateFrequency()
{
    emit frequency( static_cast<int>( _frequencyNow ),
                    static_cast<int>( _frequencyTotal ) );
}

void currentAnalyzer::updateCompressionCount( bool correct )
{
    emit addCompression( correct );
}

void currentAnalyzer::updateVentilationCount( bool correct )
{
    emit addVentilation( correct );
}

void currentAnalyzer::updateVentilationDuration( int duration )
{
    emit newDataPoint( _tempDataPoint, duration );
}

bool currentAnalyzer::sensorDataEqual( const dataPoint& p1,
                                       const dataPoint& p2 )
{
    return (p1.compDepth    == p2.compDepth
         && p1.compPosition == p2.compPosition
         && p1.ventVolume   == p2.ventVolume);
}

void currentAnalyzer::reset()
{
    resetAnalyzer();
}

void currentAnalyzer::saveData( const QString& prefix, QString delimiter )
{
    QString path = getSavePath();
    if( path.isEmpty() )
    {
        return;
    }

    QString filename = path + "/" + prefix +
            _startDateTime.toString( "yyyy-MM-dd__hh-mm-ss" ) + ".csv";
    QFile file( filename );
    if( file.open( QIODevice::ReadWrite ) )
    {
        QTextStream stream( &file );
        for( unsigned int i = 0; i < _dataVector.size(); i++ )
        {
            stream << _dataVector[i].time.toString( "hh:mm:ss,zzz" )
                   << delimiter
                   << QString::number( _dataVector[i].compPosition )
                   << delimiter
                   << QString::number( _dataVector[i].compDepth )
                   << delimiter
                   << QString::number( _dataVector[i].ventVolume )
                   << "\r\n"; // \r so that it is readable on windows as well
        }
        emit infoText( "Daten gespeichert (" + filename + ")." );
    }
    else
    {
        emit infoText( "Fehler: Daten konnten nicht in " + path +
                       " gespeichert werden!" );
    }
}

QString currentAnalyzer::getSavePath()
{
    // QStandardPaths did not work ...
    QString path = qgetenv( "EXTERNAL_STORAGE" );
    if( path.isEmpty() )
    {
        emit infoText( "Fehler: Pfad konnte nicht bestimmt werden!" );
        return path;
    }

    path += "/rat";

    QDir dir( path );
    if( !dir.exists() )
    {
        if( !dir.mkpath( path ) )
        {
            emit infoText( "Fehler: Dateiordner konnte nicht erstellt "
                           "werden!" );
            return "";
        }
    }

    return path;
}

void currentAnalyzer::showReport()
{
    if( _dataVector.size() > 0 )
    {
        finalAnalyzer evaluator( &_dataVector );

        result report;
        report.setReportHtml( evaluator.getHtmlReport() );
        report.showFullScreen();
        report.exec();
    }
}
