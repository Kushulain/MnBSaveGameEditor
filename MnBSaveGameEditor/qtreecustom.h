#ifndef QTREECUSTOM_H
#define QTREECUSTOM_H


#include <QTreeWidgetItem>
#include <string>
#include <vector>
#include "savegameinfos.h"

using namespace std;

enum QTreeItemState
{
    NORMAL = 0,
    BEGIN = 1,
    END = 2
};

class QTreeCustom : public QTreeWidgetItem
{
public:
    QTreeCustom(QTreeWidget *view, const QStringList &strings, SaveGameInfos* aModInfos, int aState);
    QTreeCustom(QTreeWidgetItem *parent, const QStringList &strings, SaveGameInfos* aModInfos, int aState);

    int state;
    vector<unsigned int> pathInTree;
    vector<QTreeCustom*> childs;
    SaveGameInfos* modInfos;
};

#endif // QTREECUSTOM_H
