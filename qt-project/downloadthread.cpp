/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "downloadthread.h"



DownloadThread::DownloadThread(QObject *parent) :
    QThread(parent)
{
}

//! [0]
DownloadThread::~DownloadThread()
{
    m_mutex.lock();
    m_quit = true;
    m_cond.wakeOne();
    m_mutex.unlock();
    wait();
}



std::ifstream::pos_type DownloadThread::filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

bool DownloadThread::downloadProcess()
{
#if defined(Q_OS_WIN32)
    std::string dataFilePath=m_filePath.toLocal8Bit().constData();
#else
    std::string dataFilePath=m_filePath.toUtf8().constData();
#endif

    bool result = true;

    auto dataSize = filesize(dataFilePath.c_str());
    auto totalSize = dataSize;
    std::stringstream ss;
    std::ifstream scoreDataFile(dataFilePath,std::ios_base::binary);
    uint32_t flashSize;


    char blockTemp[512];
    char retData[4];
    int blockIndex=0;

    if(totalSize<=0)
    {
        emit error(tr("Fail to open data file!"));
        result=false;
        goto ret;
    }

    ss<<"Data File Size: "<<dataSize;
    emit response(QString::fromStdString(ss.str()));


    if(!readFlashSize(flashSize))
    {
        result=false;
        goto ret;
    }

    if(!flashStart())
    {
        result=false;
        goto ret;
    }
    emit response(tr("Downloading..."));

    while(dataSize>0)
    {
        scoreDataFile.read(blockTemp,512);
        auto readNum=scoreDataFile.gcount();
        ByteStream cmdData = ByteStream(1024);
        cmdData.writeUInt16(static_cast<uint16_t>(blockIndex));
        cmdData.writeUInt16(static_cast<uint16_t>(readNum));
        cmdData.writeBytes(blockTemp,static_cast<uint32_t>(readNum));
        if(sendCmdPacket(0x02,cmdData.getBuffer(),cmdData.size()))
        {
            if(!recvCmDRetData(0x02,retData,sizeof(retData)))
            {
                result=false;
                break;
            }

            dataSize-=readNum;
            blockIndex++;
            float p=(float)(totalSize-dataSize)/totalSize;
            emit progress(p);
        }else
        {
            result=false;
            break;
        }
    }
    if(!flashEnd())
    {
        result=false;
        goto ret;
    }
ret:
    return result;
}


bool DownloadThread::flashStart()
{
    bool result = true;
    if(sendCmdPacket(0x01,nullptr,0))
    {
        if(!recvCmDRetData(0x01,nullptr,0))
            result=false;
    }else
    {
        result=false;
    }
    return result;
}

bool DownloadThread::flashEnd()
{
    bool result = true;
    if(sendCmdPacket(0x03,nullptr,0))
    {
        if(!recvCmDRetData(0x03,nullptr,0))
            result=false;
    }else
    {
        result=false;
    }
    return result;
}

bool DownloadThread::readFlashSize(uint32_t &flahSize)
{
    char recvCmdData[4];
    bool result = true;
    if(sendCmdPacket(0x04,nullptr,0))
    {
        if(!recvCmDRetData(0x04,recvCmdData,sizeof(recvCmdData)))
        {
            result=false;
        }
    }else
    {
        result=false;
    }
    flahSize=*((uint32_t*)recvCmdData);
    return result;

}

bool DownloadThread::recvCmDRetData(char cmd, char cmdData[], uint32_t cmdDataLen)
{
    uint32_t totalFrameLength = 2 + 2 + 1 + cmdDataLen;
    char* frameBuf =new char[totalFrameLength];
    bool result = false;
    // read response
    int acutalRead=0;
    if (serial.waitForReadyRead(500)) {
        acutalRead=serial.read(frameBuf,totalFrameLength);
        if(acutalRead!=totalFrameLength)
        {
            emit timeout(tr("Wrong frame length!"));
            goto ret;
        }
        ByteStream cmdRetFrame(totalFrameLength);
        cmdRetFrame.writeBytes(frameBuf,totalFrameLength);
        if(cmdRetFrame.readUInt16(0)!=0x776e)
        {
            emit timeout(tr("Wrong frame ID received!"));
            goto ret;
        }
        int revCmdDataLen=cmdRetFrame.readUInt16(2)-1;
        if(revCmdDataLen!=cmdDataLen)
        {
            emit timeout(tr("Wrong wanted cmd return data length!"));
            goto ret;
        }
        char recvCmd=cmdRetFrame.readUInt8(4);
        if(recvCmd!=cmd)
        {
            emit timeout(tr("Wrong cmd ID!"));
            goto ret;
        }
        cmdRetFrame.readBytes(cmdData,cmdDataLen,5);

    } else {
        emit timeout(tr("Wait read response timeout %1")
                     .arg(QTime::currentTime().toString()));
        goto ret;
    }
    result=true;

ret:
    delete [] frameBuf;
    return result;

}

bool DownloadThread::sendCmdPacket(char cmd, const char cmdData[], uint32_t cmdLen)
{
    uint16_t totalFrameLength = 2 + 2 + 1 + cmdLen;
    ByteStream frame = ByteStream(totalFrameLength);
    frame.writeUInt16(0x776e);
    frame.writeUInt16(cmdLen+1);
    frame.writeUInt8(cmd);
    frame.writeBytes(cmdData, cmdLen);
    int written;
    // write request
    written = serial.write(frame.getBuffer(),frame.size());
    if (!serial.waitForBytesWritten(100)) {

        emit timeout(tr("Wait write request timeout %1")
                     .arg(QTime::currentTime().toString()));
    }
    if (written == frame.size())
        return true;
    else
        return false;
}
void DownloadThread::download(const QString &portName, int waitTimeout, const QString &filePath)
{

    const QMutexLocker locker(&m_mutex);
    m_portName = portName;
    m_waitTimeout = waitTimeout;
    m_filePath = filePath;
    if (!isRunning())
        start();
    else
        m_cond.wakeOne();
}


void DownloadThread::run()
{
    bool currentPortNameChanged = false;

    m_mutex.lock();

    QString currentPortName;
    if (currentPortName != m_portName) {
        currentPortName = m_portName;
        currentPortNameChanged = true;
    }

    int currentWaitTimeout = m_waitTimeout;
    QString currentRequest = m_filePath;
    m_mutex.unlock();



    if (currentPortName.isEmpty()) {
        emit error(tr("No port name specified"));
        return;
    }

    while (!m_quit) {

        if (currentPortNameChanged) {
            serial.close();
            serial.setPortName(currentPortName);
            serial.setBaudRate(115200);
            serial.setDataBits(QSerialPort::Data8);
            serial.setParity(QSerialPort::NoParity);
            serial.setStopBits(QSerialPort::OneStop);
            serial.setFlowControl(QSerialPort::NoFlowControl);
            if (!serial.open(QIODevice::ReadWrite)) {
                emit error(tr("Can't open %1, error code %2")
                           .arg(m_portName).arg(serial.error()));
                return;
            }
        }

        if(!downloadProcess())
            emit error(tr("Fail to download."));
        else
            emit response(tr("Download successfully"));


        m_mutex.lock();
        m_cond.wait(&m_mutex);
        if (currentPortName != m_portName) {
            currentPortName = m_portName;
            currentPortNameChanged = true;
        } else {
            currentPortNameChanged = false;
        }
        currentWaitTimeout = m_waitTimeout;
        currentRequest = m_filePath;
        m_mutex.unlock();
    }
}
