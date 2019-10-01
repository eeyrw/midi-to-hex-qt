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
    ui->fileListView->setModel (fileListModel );
    showProgress(0.7f);

    connect(&m_thread, &MasterThread::response, this, &MainWindow::showResponse);
    connect(&m_thread, &MasterThread::error, this, &MainWindow::processError);
    connect(&m_thread, &MasterThread::timeout, this, &MainWindow::processTimeout);
    connect(&m_thread, &MasterThread::progress, this, &MainWindow::showProgress);

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
    m_thread.download("COM6",10,currentScoreDataFilePath);

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
