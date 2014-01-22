#ifndef SAVEGAMEINFOS_H
#define SAVEGAMEINFOS_H

#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <stdlib.h>
#include <ctime>
#include <stdio.h>
#include "CommonFunctions.h"
#include "ModInfos.h"
#include <QLineEdit>
#include <QProgressDialog>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "SaveGame.h"

struct SaveGameStructureItem;
class SaveGame;
struct SlotsInfos;

class SaveGameInfos
{
public:
    SaveGameInfos(string modDirectory, string SaveGamePath,
                  map<string,vector<SaveGameStructureItem> > SGStructure,
                  map<string,streamsize> aTypeSize,
                  map<string,string> aConditionalValueSave,
                  map<string,int64_t> aWatchedValues,
                  vector<string>* modINIValues,
                  QProgressDialog* aProgressBar,
                  SlotsInfos* slotsInfos);

    ~SaveGameInfos();

    string name;
    SaveGame* savegame;
    ModInfos* modInfos;
    bool loadedSuccefully;
};

#endif // SAVEGAMEINFOS_H
