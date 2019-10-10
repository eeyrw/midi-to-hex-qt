#ifndef MIDIFILELISTWIDGET_H
#define MIDIFILELISTWIDGET_H

#include <QListWidget>
#include <QApplication>
#include <QMimeData>
#include <QDropEvent>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QMouseEvent>
#include <QFileInfo>

#define FILE_ITEM_PATH Qt::UserRole+1
#define FILE_ITEM_CHECK_STATUS Qt::UserRole+2

class MidiFileListWidget : public QListWidget
{
    Q_OBJECT
public:
    MidiFileListWidget(QWidget *parent = 0);
    void addFileItem(QString filePath,bool selected=false,int rowIndex=-1);
    void removeFileItem(int rowIndex=-1);
    void modifyFileTtem(QString filePath,int rowIndex=-1);


protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

private:
    void performDrag();

    QListWidgetItem *dragItem;
    QPoint startPos;
};

#endif // MIDIFILELISTWIDGET_H
