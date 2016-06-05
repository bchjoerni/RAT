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

void currentAnalyzer::newDataString( const QString &dp )
{
    if( dp.indexOf( "<" ) != 0 || dp.indexOf( ">" ) != 4 )
    {
        if( dp.left( 3 ) != "aye" )
        {
            emit infoText( "Ungültiges Übertragunsprotokoll! "
                           "(Alte Programmversion?)" );
        }
        return;
    }

   // _mutex.lock();
   _tempDataPoint.time         = QTime::currentTime();
   _tempDataPoint.compPosition = dp.mid( 1, 1 ).toInt();
   _tempDataPoint.compDepth    = dp.mid( 2, 1 ).toInt();
   _tempDataPoint.ventVolume   = dp.mid( 3, 1 ).toInt();

   // start();
   run();
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
