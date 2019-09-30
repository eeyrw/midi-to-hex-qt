#include "mainwindow.h"
#include "ui_mainwindow.h"


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
    showProgress(0.7);

    connect(&m_thread, &MasterThread::response, this, &MainWindow::showResponse);
    connect(&m_thread, &MasterThread::error, this, &MainWindow::processError);
    connect(&m_thread, &MasterThread::timeout, this, &MainWindow::processTimeout);
    connect(&m_thread, &MasterThread::progress, this, &MainWindow::showProgress);

}

void MainWindow::showResponse(const QString &s)
{
    //setControlsEnabled(true);
    //m_trafficLabel->setText(tr("Traffic, transaction #%1:"
    //                           "\n\r-request: %2"
    //                           "\n\r-response: %3")
    //                        .arg(++m_transactionCount)
    //                        .arg(m_requestLineEdit->text())
    //                        .arg(s));

}

void MainWindow::processError(const QString &s)
{
    ui->logViewer->appendPlainText(tr("Status: Not running, %1.").arg(s));
}

void MainWindow::processTimeout(const QString &s)
{
    ui->logViewer->appendPlainText(tr("Status: Not running, %1.").arg(s));
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
    m_thread.download("COM1",10,"dfgfdg");

}
