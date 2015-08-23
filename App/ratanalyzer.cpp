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


#include "ratanalyzer.h"

ratAnalyzer::ratAnalyzer() : _data( &_dataVector ),
    _startDateTime( QDateTime::currentDateTime() ),
    _inCycle( false ), _ppointOkay( true ), _depthReached( false ),
    _enoughVolume( false ), _volumeTooHigh( false ), _frequencyNow( 0.0 ),
    _frequencyTotal( 0.0 ), _frequencyCount( 0 ), _ventilationDuration( 0 )
{
}

ratAnalyzer::ratAnalyzer( std::vector<dataPoint> *data ) : _data( data ),
    _startDateTime( QDateTime::currentDateTime() ),
    _inCycle( false ), _ppointOkay( true ), _depthReached( false ),
    _enoughVolume( false ), _volumeTooHigh( false ), _frequencyNow( 0.0 ),
    _frequencyTotal( 0.0 ), _frequencyCount( 0 ), _ventilationDuration( 0 )
{
}

ratAnalyzer::~ratAnalyzer()
{
}

void ratAnalyzer::updateFrequency()
{
    // reimplement this
}

void ratAnalyzer::updateCompressionCount( bool correct )
{
    // reimplement this
}

void ratAnalyzer::updateVentilationCount( bool correct )
{
    // reimplement this
}

void ratAnalyzer::updateVentilationDuration( int duration )
{
    // reimplement this
}

void ratAnalyzer::analyzeCompression()
{
    if( _currentData->compDepth == 0 && _previousData->compDepth != 0 )
    {
        compressionNon0To0();
    }
    else if( _currentData->compDepth != 0 && _previousData->compDepth == 0 )
    {
        compression0ToNon0();
    }
    else if( _currentData->compDepth != 0 && _previousData->compDepth != 0 )
    {
        compressionNon0ToNon0();
    }
    else
    {
        compression0To0();
    }
}

void ratAnalyzer::compression0To0()
{
    if( _startCompression.addMSecs( _compTimeoutMs ) < _currentData->time )
    {
        _inCycle = false;
    }
}

void ratAnalyzer::compression0ToNon0()
{
    if( _inCycle )
    {
        _frequencyNow = 60000.0 /
                _startCompression.msecsTo( _currentData->time );
        _frequencyTotal = (_frequencyTotal*_frequencyCount + _frequencyNow)
                / (_frequencyCount + 1);
        _frequencyCount++;
        updateFrequency();
    }
    if( _currentData->compPosition != 0 && _currentData->compDepth > 1 )
    {
        _ppointOkay = false; // pressure for pp  is too low on compDepth 1
    }
    if( _currentData->compDepth == 3 )
    {
        _depthReached = true;
    }
    _inCycle = true;
    _startCompression = _currentData->time;
}

void ratAnalyzer::compressionNon0To0()
{
    if( _ppointOkay && _depthReached )
    {
        _depthReached = false;
        updateCompressionCount( true );
    }
    else
    {
        updateCompressionCount( false );
    }

    _ppointOkay = true;
    _depthReached = false;
}

void ratAnalyzer::compressionNon0ToNon0()
{
    if( _currentData->compPosition != 0 && _currentData->compDepth > 1 )
    {
        _ppointOkay = false; // pressure for pp  is too low on compDepth 1
    }
    if( _currentData->compDepth == 3 )
    {
        _depthReached = true;
    }
}

void ratAnalyzer::analyzeVentilation()
{
    analyzeVentilationVolume(); // has to be first!
    analyzeVentilationTime();
}

void ratAnalyzer::analyzeVentilationVolume()
{
    if( _currentData->ventVolume == 2 )
    {
        _enoughVolume = true;
    }
    else if( _currentData->ventVolume == 3 )
    {
        _volumeTooHigh = true;
    }
}

void ratAnalyzer::analyzeVentilationTime()
{
    if( _currentData->ventVolume != 0 && _previousData->ventVolume == 0 )
    {
        ventilation0ToNon0();
    }
    else if( _currentData->ventVolume != 0 && _previousData->ventVolume != 0 )
    {
        ventilationNon0ToNon0();
    }
    else if( _currentData->ventVolume == 0 && _previousData->ventVolume != 0 )
    {
        ventilationNon0To0();
    }
    else
    {
        updateVentilationDuration( 0 );
    }
}

void ratAnalyzer::ventilation0ToNon0()
{
    _startVentilation = _currentData->time;
    updateVentilationDuration( 1 );
}

void ratAnalyzer::ventilationNon0To0()
{
    _ventilationDuration = _startVentilation.msecsTo( _previousData->time );

    if( _ventilationDuration < _maxVentTimeMs && _enoughVolume &&
            !_volumeTooHigh )
    {
        updateVentilationCount( true );
    }
    else
    {
        updateVentilationCount( false );
    }
    updateVentilationDuration( 0 );
    _enoughVolume = false;
    _volumeTooHigh = false;
}

void ratAnalyzer::ventilationNon0ToNon0()
{
    unsigned int duration = _startVentilation.msecsTo( _previousData->time );

    if( duration < _maxVentTimeMs )
    {
        updateVentilationDuration( 1 );
    }
    else
    {
        updateVentilationDuration( 2 );
    }
}

void ratAnalyzer::resetAnalyzer()
{
    _dataVector.clear();
    _startDateTime       = QDateTime::currentDateTime();
    _inCycle             = false;
    _ppointOkay          = true;
    _depthReached        = false;
    _enoughVolume        = false;
    _volumeTooHigh       = false;
    _frequencyNow        = 0.0;
    _frequencyTotal      = 0.0;
    _frequencyCount      = 0;
    _ventilationDuration = 0;
}
