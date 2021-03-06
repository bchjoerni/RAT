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

    connect( &_keepSendingTimer, SIGNAL( timeout() ), this,
             SLOT( sendSendCmd() ) );
}

bluetooth::~bluetooth()
{
    _keepSendingTimer.stop();

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
    if( _socket != 0 )
    {
        disconnect( _socket, SIGNAL( readyRead() ), this,
                    SLOT( readSocket() ) );
        disconnect( _socket, SIGNAL( connected() ), this,
                    SLOT( connected() ) );
        disconnect( _socket, SIGNAL( disconnected() ), this,
                    SLOT( disconnected() ) );
    }

    delete _socket;
    _socket = new QBluetoothSocket;

    connect( _socket, SIGNAL( readyRead() ), this, SLOT( readSocket() ) );
    connect( _socket, SIGNAL( connected() ), this, SLOT( connected() ) );
    connect( _socket, SIGNAL( disconnected() ), this, SLOT( disconnected() ) );
}

void bluetooth::connectToDevice( const QString &btAddress )
{
    _keepSendingTimer.stop();
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
    _keepSendingTimer.stop();

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
        emit messageReceived( line );
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
        //_keepSendingTimer.start( _sendSendCmdIntervalMs );
    }
}

void bluetooth::sendSendCmd()
{
    if( _socket->isOpen() )
    {
        //_socket->write( KEEPSENDING_CMD.toUtf8() );
    }
    else
    {
        _keepSendingTimer.stop();
    }
}

void bluetooth::disconnected()
{
    _keepSendingTimer.stop();
    emit connectionStatus( false );
}

void bluetooth::startDemo()
{
    _demoTimer.start( 77 );
    emit connectionStatus( true );

    int time = _demoCounter*77;
    char t1 = (time/95/95/95)%95 + 32;
    char t2 = (time/95/95)%95 + 32;
    char t3 = (time/95)%95 + 32;
    char t4 = time%95 + 32;
    emit messageReceived( "rstart:" + t1 + t2 + t3 + t4 );
}

void bluetooth::stopDemo()
{
    _demoTimer.stop();
    emit connectionStatus( false );
}

void bluetooth::newDemoValue()
{
    _demoCounter++;
    int time = _demoCounter*77;
    char t1 = (time/95/95/95)%95 + 32;
    char t2 = (time/95/95)%95 + 32;
    char t3 = (time/95)%95 + 32;
    char t4 = time%95 + 32;
    QString tstr = "";
    tstr = t1 + t2 + t3 + t4;

    if( (_demoCounter%243) < 210 )
    {
        switch( (_demoCounter%243)%7 )
        {
            case 0: emit messageReceived( QString( "d  !" + tstr ).toLatin1() );
                    break;
            case 1: emit messageReceived( QString( "d4  " + tstr ).toLatin1() );
                    break;
            case 2: emit messageReceived( QString( "dA  " + tstr ).toLatin1() );
                    break;
            case 3: emit messageReceived( QString( "dW  " + tstr ).toLatin1() );
                    break;
            case 4: emit messageReceived( QString( "dA  " + tstr ).toLatin1() );
                    break;
            case 5: emit messageReceived( QString( "d4  " + tstr ).toLatin1() );
                    break;
            case 6: emit messageReceived( QString( "d  !" + tstr ).toLatin1() );
                    break;
        }
    }
    else if( (_demoCounter%243) >= 210 && (_demoCounter%243) <  215 )
    {
        emit messageReceived( QString( "d  !" + tstr ).toLatin1() );
    }
    else if( (_demoCounter%243) >= 215 && (_demoCounter%243) < 243)
    {
        switch( (_demoCounter%243)%14 )
        {
            case 5: emit messageReceived( QString( "d  !" + tstr ).toLatin1() );
                    break;
            case 6: emit messageReceived( QString( "d 9!" + tstr ).toLatin1() );
                break;
            case 7: emit messageReceived( QString( "d 9!" + tstr ).toLatin1() );
                break;
            case 8: emit messageReceived( QString( "d 9!" + tstr ).toLatin1() );
                break;
            case 9: emit messageReceived( QString( "d 9!" + tstr ).toLatin1() );
                break;
            case 10: emit messageReceived( QString( "d a!" + tstr ).toLatin1() );
                break;
            case 11: emit messageReceived( QString( "d a!" + tstr ).toLatin1() );
                break;
            case 12: emit messageReceived( QString( "d a!" + tstr ).toLatin1() );
                break;
            case 13: emit messageReceived( QString( "d a!" + tstr ).toLatin1() );
                break;
            case 0: emit messageReceived( QString( "d 9!" + tstr ).toLatin1() );
                break;
            case 1: emit messageReceived( QString( "d 9!" + tstr ).toLatin1() );
                break;
            case 2: emit messageReceived( QString( "d 9!" + tstr ).toLatin1() );
                break;
            case 3: emit messageReceived( QString( "d  !" + tstr ).toLatin1() );
                break;
            case 4: emit messageReceived( QString( "d  !" + tstr ).toLatin1() );
                break;
        }
    }
    else
    {
        _demoCounter = 0;
        emit messageReceived( QString( "d  !" + tstr ).toLatin1() );
    }
}
