#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "downloadthread.h"
#include "NoteListProcessor.h"
#include "scoreprojectmanager.h"
#include <QFileDialog>
#include <QDebug>
#include <QtSerialPort>
#include <QByteArray>

#if defined(Q_OS_WIN32)
#include <windows.h>
#include <dbt.h>
#endif

Q_DECLARE_METATYPE(QSerialPortInfo)

using namespace std;
using namespace smf;
using bprinter::TablePrinter;
using noteListProcessor::NoteListProcessor;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_fileListView_currentRowChanged(int currentRow);
    void appendStatusInfo(const QString &s);
    void appendErrorInfo(const QString &s);
    void appendTimeoutInfo(const QString &s);
    void setProgressInfo(float progress);

    void on_downloadButton_clicked();

    void on_loadScoreDataFileButton_clicked();

    void on_serialPortListRefreshButton_clicked();

    void on_itemRemoveButton_clicked();

    void on_itemAddButton_clicked();

    void on_itemModifyButton_clicked();

private:
    Ui::MainWindow *ui;
    DownloadThread m_thread;
    QString currentScoreDataFilePath;
    QList<QSerialPortInfo> serialPortList;
    void updateSerialPortList();
    void autoSelectSerialPortByVidPid(uint16_t vid,uint16_t pid);
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);

};

#endif // MAINWINDOW_H
