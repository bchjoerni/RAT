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


#include "bluetooth.h"


bluetooth::bluetooth() : _socket( 0 ), _demoCounter( 0 )
{
    connect( &_discoveryAgent, SIGNAL( deviceDiscovered( QBluetoothDeviceInfo ) ),
             this, SLOT( foundDevice( QBluetoothDeviceInfo ) ) );
    connect( &_discoveryAgent, SIGNAL( finished() ), this,
             SLOT( finishedScan() ) );

    connect( &_localDevice, SIGNAL( pairingFinished( QBluetoothAddress,
             QBluetoothLocalDevice::Pairing ) ), this,
             SLOT( pairingDone( QBluetoothAddress,
                                QBluetoothLocalDevice::Pairing) ) );
    connect( &_localDevice, SIGNAL( error( QBluetoothLocalDevice::Error ) ),
             this, SLOT( pairingError( QBluetoothLocalDevice::Error ) ) );

    connect( &_demoTimer, SIGNAL( timeout() ), this, SLOT( newDemoValue() ) );
}

bluetooth::~bluetooth()
{    
    if( _discoveryAgent.isActive() )
    {
        _discoveryAgent.stop();
    }
    delete _socket;
    _socket = 0;
}

bool bluetooth::valid()
{
    if( _localDevice.isValid() )
    {
        _localDevice.powerOn();
        return true;
    }

    return false;
}

void bluetooth::scan()
{
    _discoveryAgent.start();
}

void bluetooth::foundDevice( const QBluetoothDeviceInfo &info )
{
    emit deviceFound( info.name() + " (" + info.address().toString() + ")" );
}

void bluetooth::finishedScan()
{
    emit scanFinished();
}

void bluetooth::resetSocket()
{
    disconnect( _socket, SIGNAL( readyRead() ), this, SLOT( readSocket() ) );
    disconnect( _socket, SIGNAL( connected() ), this, SLOT( connected() ) );
    disconnect( _socket, SIGNAL( disconnected() ), this,
                SLOT( disconnected() ) );

    delete _socket;
    _socket = new QBluetoothSocket;

    connect( _socket, SIGNAL( readyRead() ), this, SLOT( readSocket() ) );
    connect( _socket, SIGNAL( connected() ), this, SLOT( connected() ) );
    connect( _socket, SIGNAL( disconnected() ), this, SLOT( disconnected() ) );
}

void bluetooth::connectToDevice( const QString &btAddress )
{    
    resetSocket(); // to avoid irritating warnings

    if( !valid() )
    {
        emit connectionStatus( false );
        emit connectionInfo( "Bluetooth ist deaktiviert. Aktiviere "
                             "Bluetooth um eine Verbindung zu ermöglichen!" );
        return;
    }

    pairDevice( btAddress );
}

void bluetooth::pairDevice( const QString& address )
{
    QBluetoothLocalDevice::Pairing pairingStatus =
            _localDevice.pairingStatus( QBluetoothAddress( address ) );

    if( pairingStatus == QBluetoothLocalDevice::Paired
            || pairingStatus == QBluetoothLocalDevice::AuthorizedPaired )
    {
        connectPaired( QBluetoothAddress( address ) );
    }
    else
    {
        emit connectionStatus( false );
        emit connectionInfo( "Das Geräte ist noch nicht gekoppelt. Bitte "
                             "kopple es in den Bluetootheinstellungen!" );
        // old code (pairing does not work -.-)
        /*
        emit connectionInfo( "Gerät koppeln ... Dies kann einige Minuten in "
                             "Anspruch nehmen, bitte Geduld." );
        _localDevice.requestPairing( QBluetoothAddress( address ),
                                     QBluetoothLocalDevice::Paired );
        */
    }
}

void bluetooth::pairingError( QBluetoothLocalDevice::Error error )
{
    emit connectionStatus( false );
    emit connectionInfo( "Es konnte keine Verbindung hergestellt werden: "
                         "Kopplungsversuch fehlgeschlagen." );
}

void bluetooth::pairingDone( const QBluetoothAddress &address,
                             QBluetoothLocalDevice::Pairing pairing )
{
    if( pairing != QBluetoothLocalDevice::Paired )
    {
        emit connectionStatus( false );
        emit connectionInfo( "Fehler beim Koppeln, es konnte keine Verbindung "
                             "hergestellt werden!" );
        return;
    }
    else
    {
        emit connectionInfo( "Kopplung erfolgreich, verbinde Geräte." );
    }

    connectPaired( address );
}

void bluetooth::connectPaired( const QBluetoothAddress &address )
{
    if( _socket->state() != QBluetoothSocket::UnconnectedState )
    {
        emit connectionInfo( "Verbindungsproblem (schon verbunden?)!" );
        return;
    }

    _socket->connectToService( address, QBluetoothUuid( DEFAULT_UUID ) );

    if( _socket->errorString().size() > 0 )
    {
        emit connectionStatus( false );
        emit connectionInfo( "Ein Fehler ist aufgetreten, es konnte leider "
                             "keine Verbindung hergestellt werden!" );
    }
    else
    {
        emit connectionInfo( "Verbindungsaufbau ... bitte ein wenig Geduld." );
    }
}

void bluetooth::stop()
{
    if( _socket->isOpen() )
    {
        QByteArray text = STOP_CMD.toUtf8();
        _socket->write( text );
        emit connectionInfo( "Verbindung trennen ..." );

        while( _socket->bytesToWrite() > 0 )
        {
            qApp->processEvents();
        }
        _socket->disconnectFromService();
    }
    else
    {
        _socket->abort();
    }
}

void bluetooth::readSocket()
{
    while( _socket->canReadLine() )
    {
        QByteArray line = _socket->readLine();
        emit messageReceived( QString::fromUtf8( line.constData(),
                                                 line.length() ) );
    }
}

void bluetooth::connected()
{
    _socket->write( SEND_CMD.toUtf8() );

    if( _socket->errorString().size() > 0 )
    {
        emit connectionInfo( "Fehler beim Senden des Übertragungsbefehls!" );
    }
    else
    {
        emit connectionStatus( true );
    }
}

void bluetooth::disconnected()
{
    emit connectionStatus( false );
}

void bluetooth::startDemo()
{
    _demoTimer.start( 77 );
    emit connectionStatus( true );
}

void bluetooth::stopDemo()
{
    _demoTimer.stop();
    emit connectionStatus( false );
}

void bluetooth::newDemoValue()
{
    _demoCounter++;

    if( _demoCounter < 210 )
    {
        switch( _demoCounter%7 )
        {
            case 0: emit messageReceived( "<100>" );
                    break;
            case 1: emit messageReceived( "<010>" );
                    break;
            case 2: emit messageReceived( "<020>" );
                    break;
            case 3: emit messageReceived( "<030>" );
                    break;
            case 4: emit messageReceived( "<020>" );
                    break;
            case 5: emit messageReceived( "<010>" );
                    break;
            case 6: emit messageReceived( "<100>" );
                    break;
        }
    }
    else if( _demoCounter >= 210 && _demoCounter <  215 )
    {
        emit messageReceived( "<100>" );
    }
    else if( _demoCounter >= 215 && _demoCounter < 243)
    {
        switch( _demoCounter%14 )
        {
            case 5: emit messageReceived( "<100>" );
                    break;
            case 6: emit messageReceived( "<101>" );
                break;
            case 7: emit messageReceived( "<101>" );
                break;
            case 8: emit messageReceived( "<101>" );
                break;
            case 9: emit messageReceived( "<101>" );
                break;
            case 10: emit messageReceived( "<102>" );
                break;
            case 11: emit messageReceived( "<102>" );
                break;
            case 12: emit messageReceived( "<102>" );
                break;
            case 13: emit messageReceived( "<102>" );
                break;
            case 0: emit messageReceived( "<101>" );
                break;
            case 1: emit messageReceived( "<101>" );
                break;
            case 2: emit messageReceived( "<101>" );
                break;
            case 3: emit messageReceived( "<100>" );
                break;
            case 4: emit messageReceived( "<100>" );
                break;
        }
    }
    else
    {
        _demoCounter = 0;
        emit messageReceived( "<100>" );
    }
}
