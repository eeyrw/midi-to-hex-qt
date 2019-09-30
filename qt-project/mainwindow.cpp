#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QListWidgetItem *item;
    for (int i = 0; i < 10; ++i) {
        item = new QListWidgetItem();
        QString l=QString("%1").arg(i);
        item->setText(l);
        item->setToolTip(l);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        ui->fileListView->addItem(item);
    }
    connect(&m_thread, &MasterThread::response, this, &MainWindow::showResponse);
    connect(&m_thread, &MasterThread::error, this, &MainWindow::processError);
    connect(&m_thread, &MasterThread::timeout, this, &MainWindow::processTimeout);
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
    //setControlsEnabled(true);
    //m_statusLabel->setText(tr("Status: Not running, %1.").arg(s));
    ui->logViewer->appendPlainText(tr("Status: Not running, %1.").arg(s));
    ui->logViewer->appendPlainText(tr("No traffic."));

    //m_trafficLabel->setText(tr("No traffic."));
}

void MainWindow::processTimeout(const QString &s)
{
    //setControlsEnabled(true);
    //m_statusLabel->setText(tr("Status: Running, %1.").arg(s));
    //m_trafficLabel->setText(tr("No traffic."));
    ui->logViewer->appendPlainText(tr("Status: Not running, %1.").arg(s));
    ui->logViewer->appendPlainText(tr("No traffic."));
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
    m_thread.transaction("COM1",10,"dfgfdg");
}
