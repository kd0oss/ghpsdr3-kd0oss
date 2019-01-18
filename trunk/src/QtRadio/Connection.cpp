/* 
 * File:   Connection.cpp
 * Author: John Melton, G0ORX/N6LYT
 * 
 * Created on 16 August 2010, 07:40
 */

/* Copyright (C)
* 2009 - John Melton, G0ORX/N6LYT
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*/


/* Copyright (C) 2012 - Alex Lee, 9V1Al
* modifications of the original program by John Melton
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Pl
*/


/*  Copyright (C) 2019 - Rick Schnicker, KD0OSS
 * Rewrite to QTRadioII.
 */

#include "Connection.h"
#include <QDebug>
#include <QRegExp>

Connection::Connection()
{
    qDebug() << "Connection::Connection";
    tcpSocket = NULL;
    state = READ_HEADER_TYPE;
    bytes = 0;
    hdr = (char*)malloc(HEADER_SIZE_2_1);  // HEADER_SIZE is larger than AUTIO_HEADER_SIZE so it is OK for both
    SemSpectrum.release();
    muted = false;
    serverver = 0;
} // end constructor


Connection::~Connection()
{
    qDebug() << "Connection::~Connection";
} // end destructor


QString Connection::getHost()
{
    qDebug() << "Connection::getHost: " << host;
    return host;
} // end getHost


void Connection::connect(QString h, int p)
{
    host = h;
    port = p;

    // cleanup previous object, if any
    if (tcpSocket)
    {
        delete tcpSocket;
    }

    tcpSocket = new QTcpSocket(this);

    QObject::connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(socketError(QAbstractSocket::SocketError)));

    QObject::connect(tcpSocket, SIGNAL(connected()),
            this, SLOT(connected()));

    QObject::connect(tcpSocket, SIGNAL(disconnected()),
            this, SLOT(disconnected()));

    QObject::connect(tcpSocket, SIGNAL(readyRead()),
            this, SLOT(socketData()));

    // set the initial state
    state = READ_HEADER_TYPE;
    // cleanup dirty value eventually left from previous usage
    bytes = 0;
    qDebug() << "Connection::connect: connectToHost: " << host << ":" << port;
    tcpSocket->connectToHost(host, port);
} // end connect


void Connection::disconnected()
{
    qDebug() << "Connection::disconnected: emits: " << "Remote disconnected";
    emit disconnected("Remote disconnected");

    if (tcpSocket != NULL)
    {
        QObject::disconnect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(socketError(QAbstractSocket::SocketError)));

        QObject::disconnect(tcpSocket, SIGNAL(connected()),
                this, SLOT(connected()));

        QObject::disconnect(tcpSocket, SIGNAL(disconnected()),
                this, SLOT(disconnected()));

        QObject::disconnect(tcpSocket, SIGNAL(readyRead()),
                this, SLOT(socketData()));
    }
} // end disconnected


void Connection::disconnect()
{
    qDebug() << "Connection::disconnect Line " << __LINE__;

    if (tcpSocket != NULL)
    {
        tcpSocket->close();
        // object deletion moved in connect method 
        // tcpSocket=NULL;

    }
    // close the hardware panel, if any
    emit hardware(QString(""));
} // end disconnnect


void Connection::socketError(QAbstractSocket::SocketError socketError)
{
    switch (socketError)
    {
        case QAbstractSocket::RemoteHostClosedError:
            qDebug() << "Remote closed connection";
            break;
        case QAbstractSocket::HostNotFoundError:
            qDebug() << "Host not found";
            break;
        case QAbstractSocket::ConnectionRefusedError:
            qDebug() << "Remote host refused connection";
            break;
        default:
            qDebug() << "Socket Error: " << tcpSocket->errorString();
    }

    emit disconnected(tcpSocket->errorString());
    // memory leakeage !! 
    // tcpSocket=NULL;
} // end socketError


void Connection::connected()
{
    qDebug() << "Connection::Connected" << tcpSocket->isValid();
    emit isConnected();
    state = READ_HEADER_TYPE;
    lastFreq = 0;
    lastMode = 99;
    lastSlave = 1;
    QByteArray qbyte;
    qbyte.append((char)QUESTION);
    qbyte.append((char)QDSPVERSION);
    sendCommand(qbyte);
    qbyte.clear();
    qbyte.append((char)QUESTION);
    qbyte.append((char)QLOFFSET);
    sendCommand(qbyte);
    qbyte.clear();
    qbyte.append((char)QUESTION);
    qbyte.append((char)QCOMMPROTOCOL1);
    sendCommand(qbyte);
    qbyte.clear();
    amSlave = true;
    serverver = 0;
} // end connected


void Connection::sendCommand(QByteArray command, int size)
{
    int  i;
    char buffer[SEND_BUFFER_SIZE];
    int  bytesWritten;

  //  qDebug() << "Connection::sendCommand: "<< command.toStdString().c_str();
    for (i=0; i < SEND_BUFFER_SIZE; i++)
        buffer[i] = 0;
    if (tcpSocket != NULL && tcpSocket->isValid() && tcpSocket->isWritable())
    {
        mutex.lock();
        strcpy(buffer, command.constData());
        bytesWritten = tcpSocket->write(buffer, SEND_BUFFER_SIZE);
        if (bytesWritten != SEND_BUFFER_SIZE)
            qDebug() << "sendCommand: write error";
        //tcpSocket->flush();
        mutex.unlock();
    }
} // end sendCommand


void Connection::sendAudio(int length, unsigned char* data)
{
    char buffer[SEND_BUFFER_SIZE];
    int  i;
    int  bytesWritten;

    for (i=0; i < SEND_BUFFER_SIZE; i++)
        buffer[i] = 0;
    if (tcpSocket!=NULL && tcpSocket->isValid() && tcpSocket->isWritable())
    {
        buffer[0] = ISMIC;
        memcpy(&buffer[2], data, length);
        mutex.lock();
        bytesWritten = tcpSocket->write(buffer, SEND_BUFFER_SIZE);
        if (bytesWritten != SEND_BUFFER_SIZE)
            qDebug() << "sendCommand: write error";
        //tcpSocket->flush();
        mutex.unlock();
    }
} // end sendAudio


void Connection::socketData()
{
    int     toRead;
    int     bytesRead=0;
    int     thisRead=0;
    int     version;
    int     subversion;
    int     header_size=0;
    int     answer_size=0;
    char   *ans;
    QString answer;

    if (bytes < 0)
    {
        //fprintf(stderr,"QtRadio: FATAL: INVALID byte counter: %d\n", bytes);
        //tcpSocket->close();
        return;
    }

    toRead = tcpSocket->bytesAvailable();
    if (toRead <= 0)
    {
        return;
    }

    while (bytesRead < toRead)
    {
        fprintf (stderr, "%d of %d [%d]\n", bytesRead, toRead, state);
        switch (state)
        {
        case READ_HEADER_TYPE:
            thisRead = tcpSocket->read(&hdr[bytes], 3 - bytes);
            if (thisRead < 0)
            {
                fprintf(stderr,"QtRadioII: FATAL: READ_AUDIO_HEADER: error in read: %d\n", thisRead);
                tcpSocket->close();
                return;
            }
            bytes += thisRead;
            if (bytes == 3)
            {
                switch (hdr[0])
                {
                case AUDIO_BUFFER:
                    state = READ_AUDIO_HEADER;
                    break;
                case SPECTRUM_BUFFER:
                    version = hdr[1];
                    subversion = hdr[2];
                    switch (version)
                    {
                    case 2:
                        switch (subversion)
                        {
                        case 0:
                            header_size = HEADER_SIZE_2_0;
                            break;
                        case 1:
                            header_size = HEADER_SIZE_2_1;
                            break;
                        default:
                            fprintf(stderr, "QtRadioII: Invalid subversion. Expected %d.%d got %d.%d\n", HEADER_VERSION, HEADER_SUBVERSION, version, subversion);
                            break;
                        }
                        break;
                    default:
                        fprintf(stderr, "QtRadioII: Invalid version. Expected %d.%d got %d.%d\n", HEADER_VERSION, HEADER_SUBVERSION, version, subversion);
                        break;
                    }
                    state = READ_HEADER;
                    break;
                case BANDSCOPE_BUFFER:
                    break;
                case 52: //ANSWER_BUFFER
                    // answer size is in hdr index 1 max 255
                    state = READ_ANSWER;
                    bytes = 0;
                    answer_size = hdr[1];
                    ans = (char*)malloc(answer_size + 1);
                    break;
                }
            }
            break;

        case READ_AUDIO_HEADER:
            //fprintf (stderr, "READ_AUDIO_HEADER: hdr size: %d bytes: %d\n", AUDIO_HEADER_SIZE, bytes);
            thisRead = tcpSocket->read(&hdr[bytes], AUDIO_HEADER_SIZE - bytes);
            if (thisRead < 0)
            {
                fprintf(stderr, "QtRadioII: FATAL: READ_AUDIO_HEADER: error in read: %d\n", thisRead);
                tcpSocket->close();
                return;
            }
            bytes += thisRead;
            if (bytes == AUDIO_HEADER_SIZE)
            {
                length = ((hdr[2] & 0xFF) << 8) + (hdr[3] & 0xFF);
                buffer = (char*)malloc(length);
                bytes = 0;
                state = READ_BUFFER;
            }
            break;

        case READ_HEADER:
            //fprintf (stderr, "READ_HEADER: hdr size: %d bytes: %d\n", header_size, bytes);
            thisRead = tcpSocket->read(&hdr[bytes], header_size - bytes);
            if (thisRead < 0)
            {
                fprintf(stderr, "QtRadioII: FATAL: READ_HEADER: error in read: %d\n", thisRead);
                tcpSocket->close();
                return;
            }
            bytes += thisRead;
            if (bytes == header_size)
            {
                length = ((hdr[2] & 0xFF) << 8) + (hdr[3] & 0xFF);
                if ((length < 0) || (length > 4096))
                {
                    state = READ_HEADER_TYPE;
                }
                else
                {
                    buffer = (char*)malloc(length);
                    bytes = 0;
                    state = READ_BUFFER;
                }
            }
            break;

        case READ_BUFFER:
            //fprintf (stderr, "READ_BUFFER: length: %d bytes: %d\n", length, bytes);
            thisRead = tcpSocket->read(&buffer[bytes], length - bytes);
            if (thisRead < 0)
            {
                fprintf(stderr, "QtRadioII: FATAL: READ_BUFFER: error in read: %d\n", thisRead);
                tcpSocket->close();
                return;
            }
            bytes += thisRead;
            //qDebug() << "READ_BUFFER: read " << bytes << " of " << length;
            if (bytes == length)
            {
                version = hdr[1];
                subversion = hdr[2];
                queue.enqueue(new Buffer(hdr, buffer));
                QTimer::singleShot(0, this, SLOT(processBuffer()));
                hdr = (char*)malloc(HEADER_SIZE_2_1);
                bytes = 0;
                state = READ_HEADER_TYPE;
            }
            break;

        case READ_ANSWER:
            //qDebug() << "Connection READ ANSWER";
            thisRead = tcpSocket->read(&ans[bytes], answer_size - bytes);
            if (thisRead < 0)
            {
                fprintf(stderr, "QtRadioII: FATAL: READ_BUFFER: error in read: %d\n", thisRead);
                tcpSocket->close();
                return;
            }
            bytes += thisRead;
            if (bytes == answer_size)
            {
                //fprintf(stderr,"ans length = %lu\n",strlen(ans));
                ans[answer_size] = '\0';
                if (ans[0] == QDSPVERSION)
                {
                    //"20120107;-rxtx"; YYYYMMDD; text desc
                    answer = ans+1;
                    QString tmp = (QString)(answer.split(";").at(1));
#if QT_VERSION >= 0x050000
                    emit setdspversion(tmp.toLong(), (QString)(answer.split(";").at(2)));
#else
                    emit setdspversion(rx.cap(1).toLong(), rx.cap(2).toAscii());
#endif
                    serverver = tmp.toLong();
                    if (serverver < 20120201)
                    {  // tx login start
                        emit setCanTX(true);  //server to old to tell
                    }
                    QByteArray qbyte;
                    qbyte.append((char)QUESTION);
                    qbyte.append((char)QMASTER);
                    sendCommand(qbyte);
                }
                else
                    if (ans[0] == QSERVER)
                    {
                        answer = ans+1;
                        QString servername = (QString)(answer.split(" ").at(0));
                        emit setservername(servername);
                        QString hasTX = (QString)(answer.split(" ").at(1));
                        if (hasTX.compare("N") == 0)
                        {
                            emit setCanTX(false);
                        }
                        else
                            if (hasTX.compare("P") == 0)
                            {
                                emit setCanTX(false);
                                emit setChkTX(true);
                            }
                            else
                            {  // must be yes
                                //qDebug() <<"Yes Master";
                                if (amSlave)
                                {
                                    emit setCanTX(false);
                                    emit setChkTX(false);
                                }
                                else
                                {
                                    emit setCanTX(true);
                                    emit setChkTX(false);
                                }
                            }
                    }
                    else
                        if (ans[0] == QMASTER)
                        {
                            //qDebug() << "q-master:" << answer;
                            answer = ans+1;
                            if (answer.contains("slave"))
                            {
                                amSlave = true;
                                emit printStatusBar("  ...Slave Mode. ");
                            }
                            else
                            {
                                amSlave = false;
                                emit printStatusBar("  ...Master Mode. ");
                            }
                        }
                        else
                            if (ans[0] == QCANTX)
                            {
                                answer = ans+1;
                                QString TXNow = (QString)(answer.split(":").at(1));
                                if (TXNow.compare("Y") == 0)
                                {
                                    emit setCanTX(true);
                                }
                                else
                                {
                                    emit setCanTX(false);
                                }
                            }
                            else
                                if (ans[0] == QLOFFSET)
                                {
                                    answer = ans+1;
                                    QString tmp = (QString)(answer.split(";").at(0));
                                    double loffset = tmp.toDouble();
                                    emit resetbandedges(loffset);
                                }
                                else
                                    if (ans[0] == QINFO)
                                    {
                                        answer = ans+1;
                                        QString f = (QString)(answer.split(";").at(4));
                                        QString m = (QString)(answer.split(";").at(6));
                                        QString z = (QString)(answer.split(";").at(8));
                                        QString l = (QString)(answer.split(";").at(10));
                                        QString r = (QString)(answer.split(";").at(12));
                                        long long newf = f.toLongLong();
                                        int newmode = m.toInt();
                                        int zoom = z.toInt();
                                        int left = l.toInt();
                                        int right = r.toInt();
                                        emit slaveSetFreq(newf);
                                        emit slaveSetFilter(left, right);
                                        emit slaveSetZoom(zoom);
                                        if (newmode != lastMode)
                                        {
                                            emit slaveSetMode(newmode);
                                        }

                                        lastFreq = newf;
                                        lastMode = newmode;
                                    }
                                    else
                                        if (ans[0] == QCOMMPROTOCOL1)
                                        {
                                            answer = ans+1;
                                            emit setFPS();
                                        }
                                        else
                                            if (answer[0] == STARCOMMAND)
                                            {
                                                answer = ans+1;
                                                qDebug() << "--------------->" << answer;

                                                emit hardware(QString(answer));
                                            }

                //answer.prepend("  Question/Answer ");
                //emit printStatusBar(answer);
                qDebug() << "ANSWER bytes " << bytes << " answer " << ans;
                free(ans);
                bytes = 0;
                state = READ_HEADER_TYPE;
            }
            break;

        default:
            fprintf (stderr, "FATAL: WRONG STATUS !!!!!\n");
        }
        bytesRead += thisRead;
    }
} // end socketData


void Connection::processBuffer()
{
    Buffer *buffer;
    char   *nextHeader;
    char   *nextBuffer;

    // We only want to mute the audio (actually not for full duplex)
    // the spectrum display should show the microphone waveform
    /*
    if (muted)
    { // If Rx muted, clear queue and don't process buffer - gvj
        queue.clear();
    }
    */
    while (!queue.isEmpty())
    {
        buffer = queue.dequeue();
        nextHeader = buffer->getHeader();
        nextBuffer = buffer->getBuffer();
        // emit a signal to show what buffer we have
        //qDebug() << "processBuffer " << nextHeader[0];
        if (nextHeader[0] == SPECTRUM_BUFFER)
        {
            emit spectrumBuffer(nextHeader, nextBuffer);
        }
        else
            if (nextHeader[0] == AUDIO_BUFFER)
            {
                // need to add a duplex state
                if (!muted)
                    emit audioBuffer(nextHeader, nextBuffer);
            }
            else
                if (nextHeader[0] == BANDSCOPE_BUFFER)
                {
                    //qDebug() << "socketData: bandscope";
                    emit bandscopeBuffer(nextHeader, nextBuffer);
                }
                else
                {
                    qDebug() << "Connection::socketData: invalid header: " << nextHeader[0];
                    queue.clear();
                }
    }
} // end processBuffer


void Connection::freeBuffers(char* header,char* buffer)
{
    if (header != NULL) free(header);
    if (buffer != NULL) free(buffer);
} // end freeBuffers


bool Connection::getSlave()
{
    return amSlave;
} // end getSlave


// added by gvj
void Connection::setMuted(bool muteState)
{
    muted = muteState;
} // end setMuted
