#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "masterthread.h"
#include <QStandardItemModel>
#include <QFileDialog>
#include <QDebug>

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
    void showResponse(const QString &s);
    void processError(const QString &s);
    void processTimeout(const QString &s);
    void showProgress(float progress);

    void on_downloadButton_clicked();

    void on_loadScoreDataFileButton_clicked();

private:
    Ui::MainWindow *ui;
    MasterThread m_thread;
    QStandardItemModel *fileListModel;
    QString currentScoreDataFilePath;
};

#endif // MAINWINDOW_H
