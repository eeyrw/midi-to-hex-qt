#include "midifilelistwidget.h"

MidiFileListWidget::MidiFileListWidget(QWidget *parent)
    : QListWidget(parent)
{
    //允许拖放
    setAcceptDrops(true);
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
    }
    else if(event->mimeData()->hasFormat("text/uri-list")) {
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
    MidiFileListWidget *source =qobject_cast<MidiFileListWidget *>(event->source());
    //找到当前鼠标位置在部件中的项.
    auto item = this->itemAt(event->pos());
    if (source && source == this) {
        if (!item)
            this->addItem(dragItem->clone());
        else
            this->insertItem(this->row(item)+1,dragItem->clone());
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
                addFileItem(fileName,true,row(item)+1);
            }
        }
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
        //drag->setPixmap(QPixmap(":/images/person.png"));
        //进行拖动操作
        auto result=drag->exec(Qt::MoveAction);
        if(result==Qt::MoveAction)
            delete  item;
    }
}

void MidiFileListWidget::addFileItem(QString filePath,bool selected,int rowIndex)
{
    QListWidgetItem *item= new QListWidgetItem();
    QFileInfo fileInfo(filePath);
    QString filename(fileInfo.fileName());
    item->setText(filename);
    item->setToolTip(filePath);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(selected?Qt::Checked:Qt::Unchecked);
    item->setData(FILE_ITEM_PATH,filePath);
    item->setData(FILE_ITEM_CHECK_STATUS,selected);
    if(rowIndex>=0)
        insertItem(rowIndex,item);
    else
        addItem(item);
}

void MidiFileListWidget::removeFileItem(int rowIndex)
{
    if(rowIndex>=0)
        takeItem(rowIndex);
    else
        takeItem(currentRow());
}

void MidiFileListWidget::modifyFileTtem(QString filePath,int rowIndex)
{
    QListWidgetItem *item;
    if(rowIndex>=0)
        item=this->item(rowIndex);
    else
        item=currentItem();

    QFileInfo fileInfo(filePath);
    QString filename(fileInfo.fileName());
    item->setText(filename);
    item->setToolTip(filePath);
    item->setData(FILE_ITEM_PATH,filePath);
}

