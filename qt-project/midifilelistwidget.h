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

class MidiFileListWidget : public QListWidget
{
    Q_OBJECT
public:
    MidiFileListWidget(QWidget *parent = 0);

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
    bool isInDragExternalFileStatus;
};

#endif // MIDIFILELISTWIDGET_H
