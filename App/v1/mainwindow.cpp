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
#include "ui_main_portrait.h"
#include "ui_main_landscape.h"

#include <QDebug>

mainWindow::mainWindow( bluetooth* btDevice, QWidget *parent  ) :
    QMainWindow( parent ),
    _landscapeWidget( 0 ), _portraitWidget( 0 ),
    _btAdress( DEFAULT_ADRESS ), _bluetooth( btDevice ),
    _correctCompressions( 0 ), _totalCompressions( 0 ),
    _correctVentilations( 0 ), _totalVentilations( 0 ),
    _eventNumber( 0 ), _lastEventValue( 0 ), _eventCorrect( 0 ),
    _eventFinished( true )
{
    _landscapeWidget = new QWidget( this );
    _main_landscape.setupUi( _landscapeWidget );

    _portraitWidget = new QWidget( this );
    _main_portrait.setupUi( _portraitWidget );

    bool isLandscape = this->geometry().width() > this->geometry().height();
    _landscapeWidget->setVisible( isLandscape );
    _portraitWidget->setVisible( !isLandscape );

    initButtons();
    initSettingsVar();

    connect( _main_portrait.btn_resetValues, SIGNAL( clicked() ), this,
             SLOT( resetValues() ) );
    connect( _main_landscape.btn_resetValues, SIGNAL( clicked() ), this,
             SLOT( resetValues() ) );
    connect( _main_portrait.btn_connect, SIGNAL( clicked() ), this,
             SLOT( start() ) );
    connect( _main_landscape.btn_connect, SIGNAL( clicked() ), this,
             SLOT( start() ) );
    connect( _main_portrait.btn_stop, SIGNAL( clicked() ), this,
             SLOT( stop() ) );
    connect( _main_landscape.btn_stop, SIGNAL( clicked() ), this,
             SLOT( stop() ) );
    connect( _main_portrait.btn_save, SIGNAL( clicked() ), this,
             SLOT( save() ) );
    connect( _main_landscape.btn_save, SIGNAL( clicked() ), this,
             SLOT( save() ) );
    connect( _main_portrait.btn_settings, SIGNAL( clicked() ), this,
             SLOT( showSettings() ) );
    connect( _main_landscape.btn_settings, SIGNAL( clicked() ), this,
             SLOT( showSettings() ) );

    connect( &_clock, SIGNAL( timeout() ), this, SLOT( updateClock() ) );

    // without this gui at startup is very very small - I have not idea why .. ?
    QTimer::singleShot( 100, this, SLOT( initialResize() ) );
}

mainWindow::~mainWindow()
{
    delete _landscapeWidget;
    delete _portraitWidget;
}

void mainWindow::initialResize()
{
    QResizeEvent event( this->size(), this->size() );
    resizeEvent( &event );
}

void mainWindow::initButtons()
{
    _main_portrait.btn_stop->setEnabled( false );
    _main_landscape.btn_stop->setEnabled( false );
    _main_portrait.btn_save->setEnabled( false );
    _main_landscape.btn_save->setEnabled( false );
    _main_portrait.btn_settings->setEnabled( true );
    _main_landscape.btn_settings->setEnabled( true );

    if( _bluetooth->valid() )
    {
        updateInfoText( "Drücke 'Verbinden' um die aktuellen Werte zu "
                        "bekommen." );
        _main_portrait.btn_connect->setEnabled( true );
        _main_landscape.btn_connect->setEnabled( true );
    }
    else
    {
        updateInfoText( "Bluetooth ist deaktiviert. Aktiviere "
                        "Bluetooth um Daten erhalten zu können!" );
        _main_portrait.btn_connect->setEnabled( false );
        _main_landscape.btn_connect->setEnabled( false );
    }
}

void mainWindow::initSettingsVar()
{
    _settings.selectDevice = false;
    _settings.styleSheet   = "standard";
    _settings.drawSpeed    = 10;
    _settings.delimiter    = ";";
    _settings.prefix       = "rat_";

    settings set( &_settings );
    set.setAppStyle( _settings.styleSheet );
    setDrawSpeed();
}

void mainWindow::resizeEvent( QResizeEvent *event )
{
    QSize size = event->size();
    _portraitMode = size.height() > size.width();

    if( _portraitMode )
    {
        _portraitWidget->setFixedSize( size );
        _main_portrait.mainGrid->update();
    }
    else
    {
        _landscapeWidget->setFixedSize( size );
        _main_landscape.wgt_timeScale->update();
        _main_landscape.gph_compression->clearGraph();
        _main_landscape.gph_ventilation->clearGraph();
        _main_landscape.mainGrid->update();
    }

    _portraitWidget->setVisible( _portraitMode );
    _landscapeWidget->setVisible( !_portraitMode );
}

void mainWindow::selectDevice()
{
    deviceSearch search;
    search.init( &_btAdress, _bluetooth );
    search.showFullScreen();
    search.exec();

    if( _btAdress == DEFAULT_ADRESS )
    {
        updateInfoText( "Standardadapter ausgewählt. Drücke "
                        "'Verbinden' um die Verbindung herzustellen." );
    }
    else
    {
        updateInfoText( "Gerät gewählt (" + _btAdress + ")."
                        "Achtung: Nicht Standardgerät - ich hoffe, du weißt, "
                        "was du tust ... Drücke 'Verbinden' um die Verbindung "
                        "herzustellen." );
    }

    _main_portrait.btn_connect->setEnabled( true );
    _main_landscape.btn_connect->setEnabled( true );
}

void mainWindow::showSettings()
{
    settings set( &_settings );
    set.showFullScreen();
    set.exec();

    setDrawSpeed();

    if( _settings.selectDevice )
    {
        _settings.selectDevice = false;
        selectDevice();
    }
}

void mainWindow::start()
{
    setButtonsStart( true );

    if( _btAdress == "de:mo:de:mo:de:mo" )
    {
        emit startDemo();
    }
    else
    {
        emit btConnect( _btAdress );
    }
}

void mainWindow::stop()
{
    updateInfoText( "" );

    if( _btAdress == "de:mo:de:mo:de:mo" )
    {
        emit stopDemo();
    }
    else
    {
        emit btStop();
    }

    setButtonsStart( false );
    clearEvents();

    if( _totalCompressions > 0 || _totalVentilations > 0 )
    {
        QMessageBox msg;
        msg.setText( "Bericht anzeigen?" );
        msg.setWindowTitle( "Bericht" );
        msg.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
        msg.setButtonText( QMessageBox::Yes, "Ja" );
        msg.setButtonText( QMessageBox::No, "Nein" );
        msg.setDefaultButton( QMessageBox::Yes );
        msg.setDefaultButton( QMessageBox::No );

        if( msg.exec() == QMessageBox::Yes )
        {
            emit showReport();
        }
    }
}

void mainWindow::setButtonsStart( bool start )
{
    _main_portrait.btn_connect->setEnabled( !start );
    _main_landscape.btn_connect->setEnabled( !start );
    _main_portrait.btn_stop->setEnabled( start );
    _main_landscape.btn_stop->setEnabled( start );
    _main_portrait.btn_save->setEnabled( !start );
    _main_landscape.btn_save->setEnabled( !start );
    _main_portrait.btn_settings->setEnabled( !start );
    _main_landscape.btn_settings->setEnabled( !start );
}

QString mainWindow::getValueText( bool compression, int value )
{
    QString compressionDepth[4] = { "-", "< 4 cm", "4-5 cm", "5-6 cm" };
    QString ventilationVolume[4] = { "-", "zu gering", "korrekt", "zu hoch" };

    if( compression )
    {
        return compressionDepth[value];
    }
    else
    {
        return ventilationVolume[value];
    }
}

QString mainWindow::getValueStyle( bool compression, int value )
{
    if( compression )
    {
        switch( value )
        {
            case 1: return STYLE_WRONG;
            case 2: return STYLE_HALF;
            case 3:return STYLE_RIGHT;
            default: return STYLE_NONE;
        }
    }
    else
    {
        switch( value )
        {
            case 1: return STYLE_WRONG;
            case 2: return STYLE_RIGHT;
            case 3: return STYLE_WRONG;
            default: return STYLE_NONE;
        }
    }
}

QString mainWindow::getTechniqueText( int correct )
{
    if( correct == 0 )
    {
        return "korrekt";
    }
    else
    {
        return "falsch";
    }
}

QString mainWindow::getTechniqueStyle( int correct )
{
    if( correct == 0 )
    {
        return STYLE_RIGHT;
    }
    else
    {
        return STYLE_WRONG;
    }
}

QString mainWindow::getCurrentTechniqueStyle( int eventNum )
{
    switch( eventNum )
    {
        case 1:
        {
            updateInfoText( "Style:" + _main_portrait.lbl_eventTechnique1->styleSheet() );
            return _main_portrait.lbl_eventTechnique1->styleSheet();
            break;
        }
        case 2:
        {
            return _main_portrait.lbl_eventTechnique2->styleSheet();
            break;
        }
        case 3:
        {
            return _main_portrait.lbl_eventTechnique3->styleSheet();
            break;
        }
        case 4:
        {
            return _main_portrait.lbl_eventTechnique4->styleSheet();
            break;
        }
        case 5:
        {
            return _main_portrait.lbl_eventTechnique5->styleSheet();
            break;
        }
        default: updateInfoText( "Ups, falsche eventNumber (getCurrentTechniqueStyle)" );
    }
    return "";
}

QString mainWindow::getEventText( bool compression )
{
    QString text = "Kompression";
    if( !compression )
    {
        text = "Beatmung";
    }
    return text;
}

void mainWindow::setEvent( bool compression, int value, int correct )
{
    if( _eventFinished )
    {
        _eventNumber = nextEventNum();
        if( _eventNumber == 1 )
        {
           clearEvents();
        }
        _eventFinished = false;
    }

    if( value > _lastEventValue )
    {
        setEventType( getEventText( compression ), _eventNumber );
        setEventValue( getValueText( compression, value ),
                       getValueStyle( compression, value ), _eventNumber );
        _lastEventValue = value;
    }
    if( compression )
    {
        if( value >= 2
            && getCurrentTechniqueStyle( _eventNumber ) != STYLE_WRONG )
        {
            setEventTechnique( getTechniqueText( correct ),
                               getTechniqueStyle( correct ), _eventNumber );
        }
    }
    else
    {
        setEventTechnique( getTechniqueText( correct ),
                           getTechniqueStyle( correct ), _eventNumber );
    }
}

void mainWindow::clearEvents()
{
    for( int i = 1; i < 6; i++ )
    {
        blankEvent( i );
    }
}

void mainWindow::blankEvent( int eventNum )
{
    setEventType( "-", eventNum );
    setEventValue( "-", STYLE_NONE, eventNum );
    setEventTechnique( "-", STYLE_NONE, eventNum );
}

void mainWindow::setEventType( QString type, int eventNum )
{
    switch( eventNum )
    {
        case 1: _main_portrait.lbl_eventType1->setText( type );
            break;
        case 2: _main_portrait.lbl_eventType2->setText( type );
            break;
        case 3: _main_portrait.lbl_eventType3->setText( type );
            break;
        case 4: _main_portrait.lbl_eventType4->setText( type );
            break;
        case 5: _main_portrait.lbl_eventType5->setText( type );
            break;
        default: updateInfoText( "Ups, falsche eventNumber (setEventText)" );
    }
}

void mainWindow::setEventValue(  QString value, QString styleValue, int eventNum )
{
    switch( eventNum )
    {
        case 1:
        {
            _main_portrait.lbl_eventValue1->setText( value );
            _main_portrait.lbl_eventValue1->setStyleSheet( styleValue );
            break;
        }
        case 2:
        {
            _main_portrait.lbl_eventValue2->setText( value );
            _main_portrait.lbl_eventValue2->setStyleSheet( styleValue );
            break;
        }
        case 3:
        {
            _main_portrait.lbl_eventValue3->setText( value );
            _main_portrait.lbl_eventValue3->setStyleSheet( styleValue );
            break;
        }
        case 4:
        {
            _main_portrait.lbl_eventValue4->setText( value );
            _main_portrait.lbl_eventValue4->setStyleSheet( styleValue );
            break;
        }
        case 5:
        {
            _main_portrait.lbl_eventValue5->setText( value );
            _main_portrait.lbl_eventValue5->setStyleSheet( styleValue );
            break;
        }
        default: updateInfoText( "Ups, falsche eventNumber (setEventText)" );
    }
}

void mainWindow::setEventTechnique( QString technique, QString styleTechnique,
                        int eventNum )
{
    switch( eventNum )
    {
        case 1:
        {
            _main_portrait.lbl_eventTechnique1->setText( technique );
            _main_portrait.lbl_eventTechnique1->setStyleSheet( styleTechnique );
            break;
        }
        case 2:
        {
            _main_portrait.lbl_eventTechnique2->setText( technique );
            _main_portrait.lbl_eventTechnique2->setStyleSheet( styleTechnique );
            break;
        }
        case 3:
        {
            _main_portrait.lbl_eventTechnique3->setText( technique );
            _main_portrait.lbl_eventTechnique3->setStyleSheet( styleTechnique );
            break;
        }
        case 4:
        {
            _main_portrait.lbl_eventTechnique4->setText( technique );
            _main_portrait.lbl_eventTechnique4->setStyleSheet( styleTechnique );
            break;
        }
        case 5:
        {
            _main_portrait.lbl_eventTechnique5->setText( technique );
            _main_portrait.lbl_eventTechnique5->setStyleSheet( styleTechnique );
            break;
        }
        default: updateInfoText( "Ups, falsche eventNumber (setEventText)" );
    }
}

void mainWindow::connectionStatus( bool connected )
{
    if( connected )
    {
        _main_portrait.btn_connect->setEnabled( false );
        _main_landscape.btn_connect->setEnabled( false );
        _main_portrait.btn_stop->setEnabled( true );
        _main_landscape.btn_stop->setEnabled( true );
        updateInfoText( "Verbunden." );
        _clock.start( 1000 );        
    }
    else
    {
        _main_portrait.btn_connect->setEnabled( true );
        _main_landscape.btn_connect->setEnabled( true );
        _main_portrait.btn_stop->setEnabled( false );
        _main_landscape.btn_stop->setEnabled( false );
        updateInfoText( "Verbindung unterbrochen." );
        _clock.stop();
    }
}

void mainWindow::save()
{
    _main_portrait.btn_save->setEnabled( false );
    _main_landscape.btn_save->setEnabled( false );
    emit saveData( _settings.prefix, _settings.delimiter );
}

void mainWindow::resetValues()
{
    _correctCompressions = 0;
    _totalCompressions   = 0;
    _correctVentilations = 0;
    _totalVentilations   = 0;

    _main_portrait.time_clock->setTime( QTime( 0, 0, 0 ) );
    _main_landscape.time_clock->setTime( QTime( 0, 0, 0 ) );

    _main_portrait.lbl_compressions->setText( "0 / 0" );
    _main_portrait.lbl_ventilations->setText( "0 / 0" );
    _main_portrait.lbl_frequencyNow->setText( "- / min" );
    _main_portrait.lbl_frequencyTotal->setText( "- / min" );
    _main_landscape.lbl_frequency->setText(
                "Frequenz: momentan | gesamt:       - / min   |   - / min" );

    _main_landscape.gph_compression->clearGraph();
    _main_landscape.gph_ventilation->clearGraph();

    initButtons();
    emit reset();
}

void mainWindow::updateInfoText( const QString &text )
{
    _main_portrait.lbl_infoText->setText( text );
    _main_landscape.lbl_infoText->setText( text );
}

void mainWindow::newDataPoint( const dataPoint& dp, int ventilationDuration )
{
    if( _portraitMode )
    {
        if( dp.compDepth > 0 )
        {
            setEvent( true, dp.compDepth, dp.compPosition );
        }
        else if( dp.ventVolume > 0 )
        {
            int ventilationTime = 0;
            if( ventilationDuration >= 2 )
            {
                ventilationTime = 1;
            }
            setEvent( false, dp.ventVolume, ventilationTime );
        }
        else
        {
            _eventFinished = true;
            _lastEventValue = 0;
        }
    }
    else
    {
        int ventilationTime = 0;
        if( ventilationDuration >= 2 )
        {
            ventilationTime = 1;
        }
        _main_landscape.gph_compression->newPoint( dp.time, dp.compDepth,
                                                   dp.compPosition );
        _main_landscape.gph_ventilation->newPoint( dp.time, dp.ventVolume,
                                                   ventilationTime );
    }
}

void mainWindow::updateFrequency( int frequencyNow, int frequencyTotal )
{
    _main_portrait.lbl_frequencyNow->setText( QString::number( frequencyNow )
                                              + " / min" );
    _main_portrait.lbl_frequencyTotal->setText(
                QString::number( frequencyTotal ) + " / min" );
    _main_landscape.lbl_frequency->setText(
                "Frequenz: momentan | gesamt:       " +
                QString::number( frequencyNow ) + " / min   |   " +
                QString::number( frequencyTotal ) + " / min" );

}

void mainWindow::updateCompressionCount( bool correct )
{
    if( correct )
    {
        _correctCompressions++;
    }
    _totalCompressions++;

    if( _portraitMode )
    {
        _main_portrait.lbl_compressions->setText(
            QString::number( _correctCompressions ) + " / "
                + QString::number( _totalCompressions ) );
    }
    else
    {
        _main_landscape.gph_compression->colorLastEvent( correct );
    }
}

void mainWindow::updateVentilationCount( bool correct )
{
    if( correct )
    {
        _correctVentilations++;
    }
    _totalVentilations++;

    if( _portraitMode )
    {
        _main_portrait.lbl_ventilations->setText(
            QString::number( _correctVentilations ) + " / "
                + QString::number( _totalVentilations ) );
    }
    else
    {
        _main_landscape.gph_ventilation->colorLastEvent( correct );
    }
}

void mainWindow::updateClock()
{
    _main_portrait.time_clock->setTime(
                _main_portrait.time_clock->time().addSecs( 1 ) );
    _main_landscape.time_clock->setTime(
                _main_landscape.time_clock->time().addSecs( 1 ) );
}

void mainWindow::setDrawSpeed()
{
    _main_landscape.gph_compression->setDrawSpeed( _settings.drawSpeed );
    _main_landscape.gph_ventilation->setDrawSpeed( _settings.drawSpeed );
    _main_landscape.wgt_timeScale->setDrawSpeed( _settings.drawSpeed );
    _main_landscape.wgt_timeScale->update();
}

int mainWindow::nextEventNum()
{
    int nextEvent = (_eventNumber+1)%6;
    if( nextEvent == 0 )
    {
        nextEvent = 1;
    }
    return nextEvent;
}
