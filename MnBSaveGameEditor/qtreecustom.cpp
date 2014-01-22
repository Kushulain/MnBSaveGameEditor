#include "qtreecustom.h"

using namespace std;

QTreeCustom::QTreeCustom(QTreeWidget *view, const QStringList &strings, SaveGameInfos* aModInfos, int aState) : QTreeWidgetItem(view,strings)
{

    state = aState;
    modInfos = aModInfos;
    // QTreeWidgetItem(view, QStringList(QString("item: %1").arg(10)));
}


QTreeCustom::QTreeCustom(QTreeWidgetItem *parent, const QStringList &strings, SaveGameInfos* aModInfos, int aState) : QTreeWidgetItem(parent,strings)
{
    state = aState;
    modInfos = aModInfos;
    /*
    QStringList QSL;

    for (unsigned int i=0; i < name.size(); i++)
    {
        QSL.append(QString::fromStdString(name[i]));
    }*/

     //QTreeWidgetItem(parent, QStringList(QString("item: %1").arg(10)));
}
