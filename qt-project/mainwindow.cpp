#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //    QListWidgetItem *item;
    //    for (int i = 0; i < 20; ++i) {
    //        item = new QListWidgetItem();
    //        QString l=QString("%1").arg(i);
    //        item->setText(l);
    //        item->setToolTip(l);
    //        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    //        item->setCheckState(Qt::Unchecked);
    //        ui->fileListView->addItem(item);
    //    }

    fileListModel=new QStandardItemModel();

    for (int row = 0; row < 20; ++row){

        QStandardItem *item = new QStandardItem(QString("%1").arg(row) );    //    行2
        item->setCheckable( true );
        item->setCheckState( Qt::Unchecked );
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        fileListModel->appendRow(item);
    }
    ui->fileListView->setModel (fileListModel);

    connect(&m_thread, &MasterThread::response, this, &MainWindow::showResponse);
    connect(&m_thread, &MasterThread::error, this, &MainWindow::processError);
    connect(&m_thread, &MasterThread::timeout, this, &MainWindow::processTimeout);
    connect(&m_thread, &MasterThread::progress, this, &MainWindow::showProgress);

    updateSerialPortList();
    autoSelectSerialPortByVidPid(0x1a86,0x7523);

}

void MainWindow::updateSerialPortList()
{
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

void MainWindow::autoSelectSerialPortByVidPid(uint16_t vid,uint16_t pid)
{
    for(auto i=0;i!=ui->serialPortComobox->count();i++)
    {
        auto info=ui->serialPortComobox->itemData(i);
        if(info.canConvert<QSerialPortInfo>())
        {
            QSerialPortInfo sinfo=info.value<QSerialPortInfo>();
            if(vid==sinfo.vendorIdentifier() &&
                    pid==sinfo.productIdentifier())
            {
                ui->serialPortComobox->setCurrentIndex(i);
            }
        }

    }
}

void MainWindow::showResponse(const QString &s)
{
    ui->logViewer->appendPlainText("Status: "+s);
}

void MainWindow::processError(const QString &s)
{
    ui->logViewer->appendPlainText("Error: "+s);
}

void MainWindow::processTimeout(const QString &s)
{
    ui->logViewer->appendPlainText("TimeOut: "+s);
}

void MainWindow::showProgress(float progress)
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
    //定义文件对话框类
    QFileDialog *fileDialog = new QFileDialog(this);
    //定义文件对话框标题
    fileDialog->setWindowTitle(tr("Open Score Data File."));
    //设置默认文件路径
    fileDialog->setDirectory(".");
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
            showResponse("Load Score Data File: "+currentScoreDataFilePath);
        }
    }
}
