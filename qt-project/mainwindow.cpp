#include "mainwindow.h"
#include "ui_mainwindow.h"
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    for (int i = 0; i < 5; ++i) {
        QString l=QString("%1").arg(i);
        ui->fileListWidget->addFileItem(l,true);
    }


    connect(&m_thread, &DownloadThread::response, this, &MainWindow::appendStatusInfo);
    connect(&m_thread, &DownloadThread::error, this, &MainWindow::appendErrorInfo);
    connect(&m_thread, &DownloadThread::timeout, this, &MainWindow::appendTimeoutInfo);
    connect(&m_thread, &DownloadThread::progress, this, &MainWindow::setProgressInfo);

    updateSerialPortList();
    autoSelectSerialPortByVidPid(0x1a86,0x7523);

}

void MainWindow::updateSerialPortList()
{
    ui->serialPortComobox->clear();
    serialPortList = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : serialPortList) {
        QString s = QObject::tr("Port: ") + info.portName() + "-"
                + QObject::tr("Serial number: ") + info.serialNumber() + "-"
                + QObject::tr("Vendor Identifier: ") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) + "-"
                + QObject::tr("Product Identifier: ") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) + "-"
                + QObject::tr("Busy: ") + (info.isBusy() ? QObject::tr("Yes") : QObject::tr("No"));
        ui->serialPortComobox->addItem(s,QVariant::fromValue(info));
    }

}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    //在这里做你想要的操作
    //其中eventType表明了此次消息的类型，message存储着具体是什么信息，result是个传出变量；
    //eventType在windows操作系统下是"windows_generic_MSG"字符串，可以查看Qt的文档知道
    //message表明这个信号附带哪些信息，在热插拔事件中是WM_DEVICECHANGE类型，具体windows定义了哪些，可以查看“Dbt.h”文件

    //
#if defined(Q_OS_WIN32)
    if(eventType == QByteArray("windows_generic_MSG"))
    {
        //MSG是winApi定义的结构体，具体定义如下：
        /*
         * typedef struct tagMSG {
         * HWND hwnd;
         * UINT message;   //消息类型，热插拔是WM_DEVICECHANGE
         * WPARAM wParam;  //对消息的进一步描述
         * LPARAM lParam;  //指向一个结构体，结构中有好多消息
         * DWORD time;     //产生时间
         * POINT pt;       //鼠标坐标
         * } MSG;
        */
        MSG *pMsg = reinterpret_cast<MSG*>(message);

        if(pMsg->message == WM_DEVICECHANGE)
        {
            switch(pMsg->wParam)
            {
            //设备连上
            case DBT_DEVICEARRIVAL:
                updateSerialPortList();
                autoSelectSerialPortByVidPid(0x1a86,0x7523);
                break;
                //设备断开
            case DBT_DEVICEREMOVECOMPLETE:
                updateSerialPortList();
                autoSelectSerialPortByVidPid(0x1a86,0x7523);
                break;
                //其他的消息可以查看“Dbt.h”文件
            }
        }

    }
#endif

    return false;
}

void MainWindow::autoSelectSerialPortByVidPid(uint16_t vid,uint16_t pid)
{
    auto i=0;
    for(i=0;i!=ui->serialPortComobox->count();i++)
    {
        auto info=ui->serialPortComobox->itemData(i);
        if(info.canConvert<QSerialPortInfo>())
        {
            QSerialPortInfo sinfo=info.value<QSerialPortInfo>();
            if(vid==sinfo.vendorIdentifier() &&
                    pid==sinfo.productIdentifier())
            {
                ui->serialPortComobox->setCurrentIndex(i);
                break;
            }
        }
    }
    if(i==ui->serialPortComobox->count())
        ui->serialPortComobox->setCurrentIndex(-1);
}

void MainWindow::appendStatusInfo(const QString &s)
{
    ui->logViewer->appendPlainText("Status: "+s);
}

void MainWindow::appendErrorInfo(const QString &s)
{
    ui->logViewer->appendPlainText("Error: "+s);
}

void MainWindow::appendTimeoutInfo(const QString &s)
{
    ui->logViewer->appendPlainText("TimeOut: "+s);
}

void MainWindow::setProgressInfo(float progress)
{
    ui->downloadProgressBar->setValue(progress*ui->downloadProgressBar->maximum());
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_fileListView_currentRowChanged(int currentRow)
{
    qDebug(QString("%1").arg(currentRow).toLatin1());
}


void MainWindow::on_downloadButton_clicked()
{
    auto sinfo=ui->serialPortComobox->currentData().value<QSerialPortInfo>();
    auto selectedPort=sinfo.portName();
    m_thread.download(selectedPort,10,currentScoreDataFilePath);
}

void MainWindow::on_loadScoreDataFileButton_clicked()
{
    QSettings setting("./midi-to-hex-qt-setting.ini", QSettings::IniFormat);          //为了能记住上次打开的路径
    QString lastPath = setting.value("LastFilePath").toString();

    //定义文件对话框类
    QFileDialog *fileDialog = new QFileDialog(this);
    //定义文件对话框标题
    fileDialog->setWindowTitle(tr("Open Score Data File."));
    //设置默认文件路径
    fileDialog->setDirectory(lastPath);
    //设置文件过滤器
    fileDialog->setNameFilter(tr("Images(*.raw)"));
    //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles
    //fileDialog->setFileMode(QFileDialog::ExistingFiles);
    //设置视图模式
    fileDialog->setViewMode(QFileDialog::Detail);
    //打印所有选择的文件的路径
    QStringList fileNames;
    if(fileDialog->exec())
    {
        fileNames = fileDialog->selectedFiles();
        if(fileNames.size()>0)
        {
            currentScoreDataFilePath=fileNames[0];
            appendStatusInfo("Load Score Data File: "+currentScoreDataFilePath);
            QFileInfo fi(currentScoreDataFilePath);
            setting.setValue("LastFilePath",fi.absoluteDir().absolutePath());
            auto sinfo=ui->serialPortComobox->currentData().value<QSerialPortInfo>();
            auto selectedPort=sinfo.portName();
            m_thread.download(selectedPort,10,currentScoreDataFilePath);
            m_thread.download(selectedPort,10,currentScoreDataFilePath);
        }
    }
}


void generateScoreListMemAndScore(QVector<ScoreItem> &scoreList)
{
      std::string path;
      vector<char> scoreMem;
      vector<string> pathList;

      for(ScoreItem &score:scoreList)
      {    if (score.selected)
          {
                ifstream f(path.c_str());
                if (f.good())
                      pathList.push_back(path);
                else
                      cout << "Cannot find midi file:" << path;
          }

      }

      ByteStream scoreListMem = ByteStream(128 * 1024);

      string identifer = "SCRE";
      scoreListMem.writeBytes(identifer.c_str(), 4);
      scoreListMem.writeUInt32(pathList.size());
      int scoreMemPointer = scoreListMem.size() + sizeof(uint32_t) * pathList.size();
      int headerOffest;
      double totalDuration = 0;

      std::ofstream logFileStream;
      logFileStream.open("ScoreListGen.log");

      for (auto midiFilePath : pathList)
      {
            vector<char> mem;
            NoteListProcessor np = NoteListProcessor(midiFilePath, &logFileStream);
                  //np.recommLowestPitch = options.getInteger("lower");
                  //np.recommHighestPitch = options.getInteger("upper");

            logFileStream << "File: " << midiFilePath << '\n';
            np.analyzeNoteMapByCentroid();
            np.transposeTickNoteMap();
            np.generateDeltaBin(mem);
            scoreListMem.writeUInt32(scoreMemPointer);
            scoreMemPointer += mem.size();
            std::move(mem.begin(), mem.end(), std::back_inserter(scoreMem));
            totalDuration += np.midiDuration;
            logFileStream << "\n\n";
      }
      headerOffest = scoreListMem.size();

      scoreListMem.writeBytes(scoreMem.data(), scoreMem.size());

      vector<char> scoreListMemVector(scoreListMem.size());
      scoreListMem.readBytes(scoreListMemVector.data(), scoreListMem.size(), 0);
      std::ofstream ofile("scoreList_qt.raw", std::ios::binary);
      ofile.write(scoreListMemVector.data(), scoreListMem.size());

      logFileStream << "\n\n\n=============================================\n";
      logFileStream << "Score Count: " << pathList.size() << '\n';
      logFileStream << "Total Mem Size (byte): " << scoreListMem.size() + headerOffest << '\n';
      logFileStream << "Score Data Mem Size (byte): " << scoreListMem.size() << '\n';
      int dur = static_cast<int>(totalDuration);
      logFileStream << "Total Duration: " << dur / 3600 << "h "
                    << dur / 60 % 60 << "m "
                    << dur % 60 << "s " << '\n';
      logFileStream.close();
}

void MainWindow::on_serialPortListRefreshButton_clicked()
{
    updateSerialPortList();
    autoSelectSerialPortByVidPid(0x1a86,0x7523);
}

void MainWindow::on_itemRemoveButton_clicked()
{
    ui->fileListWidget->removeFileItem();
}

void MainWindow::on_itemAddButton_clicked()
{
    QSettings setting("./midi-to-hex-qt-setting.ini", QSettings::IniFormat);          //为了能记住上次打开的路径
    QString lastPath = setting.value("LastMidiFilePath").toString();

    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("Add midi file(s)."));
    fileDialog->setDirectory(lastPath);
    fileDialog->setNameFilter(tr("Midi(*.mid)"));
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    fileDialog->setViewMode(QFileDialog::Detail);

    QStringList fileNames;
    if(fileDialog->exec())
    {
        fileNames = fileDialog->selectedFiles();
        for(auto &fileName:fileNames)
        {
            appendStatusInfo("Add midi file: "+fileName);
            QFileInfo fi(fileName);
            setting.setValue("LastMidiFilePath",fi.absoluteDir().absolutePath());
            ui->fileListWidget->addFileItem(fileName,true);
        }
    }
}

void MainWindow::on_itemModifyButton_clicked()
{
    QSettings setting("./midi-to-hex-qt-setting.ini", QSettings::IniFormat);          //为了能记住上次打开的路径
    QString lastPath = setting.value("LastMidiFilePath").toString();

    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("Modify midi file."));
    fileDialog->setDirectory(lastPath);
    fileDialog->setNameFilter(tr("Midi(*.mid)"));
    fileDialog->setViewMode(QFileDialog::Detail);

    QStringList fileNames;
    if(fileDialog->exec())
    {
        fileNames = fileDialog->selectedFiles();
        for(auto &fileName:fileNames)
        {
            appendStatusInfo("Add midi file: "+fileName);
            QFileInfo fi(fileName);
            setting.setValue("LastMidiFilePath",fi.absoluteDir().absolutePath());
            ui->fileListWidget->modifyFileTtem(fileName);
        }
    }
}
