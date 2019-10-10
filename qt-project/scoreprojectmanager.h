#ifndef SCOREPROJECTMANAGER_H
#define SCOREPROJECTMANAGER_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>

typedef struct _ScoreItem
{
   bool selected;
QString filePath;
}ScoreItem;

class ScoreProjectManager
{
public:
    ScoreProjectManager();
    ScoreProjectManager(QString projectDir);
    void saveProject(void);
private:
    bool validateProject(QString projectDir);
    QString currentPorjectDir;
    QList<ScoreItem> scoreItemList;

};

#endif // SCOREPROJECTMANAGER_H
