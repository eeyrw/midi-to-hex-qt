#include "midifilelistwidget.h"

MidiFileListWidget::MidiFileListWidget(QWidget *parent)
    : QListWidget(parent)
{
    //允许拖放
        setAcceptDrops(true);
        isInDragExternalFileStatus=false;
}

void MidiFileListWidget::mousePressEvent(QMouseEvent *event)
{
    //获取鼠标按下时候的坐标
    if (event->button() == Qt::LeftButton)
        startPos = event->pos();
    QListWidget::mousePressEvent(event);
}

void MidiFileListWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        int distance = (event->pos() - startPos).manhattanLength();
        //如果长度大于推荐的拖动起始距离，则认为是拖动(存在用户手抖的情况)
        if (distance >= QApplication::startDragDistance())
            performDrag();
    }
    QListWidget::mouseMoveEvent(event);
}

//有文件拖动到窗口上时，触发此dragEnterEvent事件
void MidiFileListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    //当为同一个应用程序的一部分时，event->source()返回启动这个拖动窗口部件的指针
    MidiFileListWidget *source =
            qobject_cast<MidiFileListWidget *>(event->source());
    if (source && source == this) {
        //确认是一个移动的操作
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }else if(event->mimeData()->hasFormat("text/uri-list")) {
   isInDragExternalFileStatus=true;
   //setMouseTracking(isInDragExternalFileStatus);
        event->acceptProposedAction();
    }
    focusWidget();
}

void MidiFileListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    MidiFileListWidget *source =
            qobject_cast<MidiFileListWidget *>(event->source());
    if (source && source == this) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    auto item = this->itemAt(event->pos());
    setCurrentItem(item);
}

void MidiFileListWidget::dropEvent(QDropEvent *event)
{
    MidiFileListWidget *source =
            qobject_cast<MidiFileListWidget *>(event->source());
            //找到当前鼠标位置在部件中的项.
     auto item = this->itemAt(event->pos());
    if (source && source == this) {


        //
        if (!item)
            this->addItem("QListWidgetItem(dragItem)");
        else
            this->insertItem(this->row(item)+1,"QListWidgetItem(dragItem)");
        //得到数据并增加到列表中
        //addItem(dragItem);
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }else
    {
        QList<QUrl> urls = event->mimeData()->urls();
        if (urls.isEmpty()) {
            return;
        }
        for(auto &url:urls)
        {
            QString fileName = url.toLocalFile();
            if (!fileName.isEmpty()) {
                this->insertItem(this->row(item)+1,fileName);
            }
        }
   isInDragExternalFileStatus=false;
   //setMouseTracking(isInDragExternalFileStatus);

    }
}

void MidiFileListWidget::performDrag()
{
    //当前被拖动的选项
    QListWidgetItem *item = currentItem();
    dragItem=currentItem();
    if (item) {
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(item->text());

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(QPixmap(":/images/person.png"));
        //进行拖动操作
        auto result=drag->exec(Qt::MoveAction);
        if(result==Qt::MoveAction)
            delete  item;
    }
}
