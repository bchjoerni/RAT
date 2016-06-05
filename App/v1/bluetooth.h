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

#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QApplication>
#include <QObject>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothLocalDevice>
#include <QBluetoothSocket>
#include <QTimer>


const QString DEFAULT_DEVICE_NAME = "HC-06";
const QString DEFAULT_ADRESS      = "98:D3:31:50:10:50";
const QString DEFAULT_UUID        = "00001101-0000-1000-8000-00805F9B34FB";
const QString SEND_CMD            = "talkrats\n";
const QString STOP_CMD            = "silence\n";

class bluetooth : public QObject
{
    Q_OBJECT

public:
    bluetooth();
    ~bluetooth();
    bool valid();
    void scan();

signals:
    void deviceFound( const QString& nameAndAddress );
    void scanFinished();
    void connectionInfo( const QString& info );
    void messageReceived( const QString& message );
    void connectionStatus( bool connected );

public slots:
    void connectToDevice( const QString& btAddress );
    void stop();
    void startDemo();
    void stopDemo();

private slots:
    void foundDevice( const QBluetoothDeviceInfo& info );
    void finishedScan();
    void pairingError( QBluetoothLocalDevice::Error error );
    void pairingDone( const QBluetoothAddress &address,
                      QBluetoothLocalDevice::Pairing pairing );
    void readSocket();
    void connected();
    void disconnected();
    void newDemoValue();

private:
    void resetSocket();
    void pairDevice( const QString& address );
    void connectPaired( const QBluetoothAddress &address );

    QBluetoothLocalDevice _localDevice;
    QBluetoothDeviceDiscoveryAgent _discoveryAgent;
    QBluetoothSocket* _socket;
    QTimer _demoTimer;
    int _demoCounter;
};

#endif // BLUETOOTH_H
