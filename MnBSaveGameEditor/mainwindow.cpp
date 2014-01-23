#include "mainwindow.h"

using namespace CommonFunctions;

MainWindow::MainWindow(
                            map<string,vector<SaveGameStructureItem> >* aSGStructure,
                            map<string,streamsize>* aTypeSize,
                            map<string,string>* aConditionalValueSave,
                            map<string,int64_t>* aWatchedValues,
                            vector<string>* amodINIValues,
                            SlotsInfos* aSlotsInfos,
                            map<string,int>* aGlobalVarBehaviours) : QWidget()
{


    setMinimumSize(600,300);
    browseWindowOpen = false;
    convertWindowOpen = false;
    SGStructure = aSGStructure;
    TypeSize = aTypeSize;
    ConditionalValueSave = aConditionalValueSave;
    WatchedValues = aWatchedValues;
    modINIValues = amodINIValues;
    slotsInfos = aSlotsInfos;
    globalVarBehaviours = aGlobalVarBehaviours;
    modAToConvertIndex = 0;
    modBToConvertIndex = 0;
    createdCheckBoxes= false;

    QPushButton* btn_addSave = new QPushButton("Import new save...");
    QPushButton* btn_exportSave = new QPushButton("Export current save...");
    QPushButton* btn_convertSave = new QPushButton("Convert save...");
    btn_convertSave->setToolTip("Trys to convert current savegame to another mod version savegame.");

    QObject::connect(btn_addSave, SIGNAL(clicked()), this, SLOT(BrowseSGWindow()));
    QObject::connect(btn_exportSave, SIGNAL(clicked()), this, SLOT(ExportSave()));
    QObject::connect(btn_convertSave, SIGNAL(clicked()), this, SLOT(ConvertSaveWindow()));

    QVBoxLayout* verticalLayout = new QVBoxLayout();
    QHBoxLayout* btnLayout = new QHBoxLayout();


    btnLayout->addWidget(btn_addSave);
    btnLayout->addWidget(btn_exportSave);
    btnLayout->addWidget(btn_convertSave);


    tabSaves = new QTabWidget;


    verticalLayout->addLayout(btnLayout);
    verticalLayout->addWidget(tabSaves);

    QObject::connect(tabSaves, SIGNAL(currentChanged(int)),
                     this, SLOT(ChangeTree(int)));

    setLayout(verticalLayout);


    lastModDirectory = "Path to Module Directory File (e.g. : Steam\\SteamApps\\common\\MountBlade Warband\\Modules\\HairyVikingMod)";
    lastSaveDirectory = "Path to SaveGame File (e.g. : MyDocuments\\Mount&Blade Warband Savegames\\HairyVikingMod)";
/*
    DONTREPLACE = 0,                        //white
    REPLACE = 1,                            //red
    CLEAR = 2,                              // grey

    DONTREPLACE_EXCEPTPLAYERS = 3,
    REPLACE_EXCEPTPLAYERS = 4,

    REPLACE_BY_FACTION_ID = 5,              //purple
    REPLACE_BY_GLOBALVAR_ID = 6,            //none
    REPLACE_BY_PARTY_ID = 7,                // dark green //[game/party_records/id] = game/party_records/id/party/name
    REPLACE_BY_PARTY_TEMPLATE_ID = 8,       // light Green//party_template.txt
    REPLACE_BY_PARTY_RECORD_ID = 9,         // green//[game/party_records/raw_id] = game/party_records/id/party/name
    REPLACE_BY_QUEST_ID = 10,               // light blue
    REPLACE_BY_SCENE_ID = 11,               //yellow
    REPLACE_BY_TROOP_ID = 12,               //orange
    REPLACE_BY_ITEM_ID = 13                 //brown*/

    Colors[DONTREPLACE] = new QBrush(QColor(255,255,255));
    Colors[REPLACE] = new QBrush(QColor(255,130,130));
    Colors[CLEAR] = new QBrush(QColor(130,130,130));
    Colors[REPLACE_BY_FACTION_ID] = new QBrush(QColor(255,130,255));
    Colors[REPLACE_BY_PARTY_ID] = new QBrush(QColor(100,150,100));
    Colors[REPLACE_BY_PARTY_TEMPLATE_ID] = new QBrush(QColor(200,255,200));
    Colors[REPLACE_BY_PARTY_RECORD_ID] = new QBrush(QColor(150,255,150));
    Colors[REPLACE_BY_QUEST_ID] = new QBrush(QColor(200,200,255));
    Colors[REPLACE_BY_SCENE_ID] = new QBrush(QColor(255,255,200));
    Colors[REPLACE_BY_TROOP_ID] = new QBrush(QColor(255,220,200));
    Colors[REPLACE_BY_ITEM_ID] = new QBrush(QColor(240,200,160));

}


void MainWindow::BrowseSGWindow()
{

    if (browseWindowOpen)
        browseWindow->close();

    browseWindow = new QWidget();
    browseWindowOpen = true;
    browseWindow->setMinimumSize(600,100);
    //browseWindow->setWindowFlags(Popup);

    QHBoxLayout* modLayout = new QHBoxLayout();
    QHBoxLayout* SGLayout = new QHBoxLayout();
    QHBoxLayout* OKCANCELLayout = new QHBoxLayout();
    QVBoxLayout* globalLayout = new QVBoxLayout(browseWindow);

    ModPath = new QLineEdit();
    ModPath->setText(lastModDirectory);
    QPushButton* ModPathButton = new QPushButton("...");

    SGPath = new QLineEdit();
    SGPath->setText(lastSaveDirectory);
    QPushButton* SGPathButton = new QPushButton("...");

    QPushButton* Load = new QPushButton("Load");
    QPushButton* Cancel = new QPushButton("Cancel");

    modLayout->addWidget(ModPath);
    modLayout->addWidget(ModPathButton);

    SGLayout->addWidget(SGPath);
    SGLayout->addWidget(SGPathButton);

    OKCANCELLayout->addWidget(Load);
    OKCANCELLayout->addWidget(Cancel);


    globalLayout->addLayout(modLayout);
    globalLayout->addLayout(SGLayout);
    globalLayout->addLayout(OKCANCELLayout);


    QObject::connect(browseWindow, SIGNAL(destroyed()),
                     this, SLOT(CloseBrowse()));


/*
    QSignalMapper* signalMapper = new QSignalMapper (this) ;
    QObject::connect(ModPathButton, SIGNAL(clicked()),
                     signalMapper, SLOT(map()));
    signalMapper -> setMapping (ModPathButton, ModPath);
    connect (signalMapper, SIGNAL(mapped(QWidget*)), this, SLOT(BrowseDirectory(QWidget*)));*/




    QObject::connect(ModPathButton, SIGNAL(clicked()),
                     this, SLOT(BrowseModDirectory()));

    QObject::connect(SGPathButton, SIGNAL(clicked()),
                     this, SLOT(BrowseSGFile()));

    QObject::connect(Load, SIGNAL(clicked()),
                     this, SLOT(LoadSave()));

    QObject::connect(Cancel, SIGNAL(clicked()),
                     browseWindow, SLOT(close()));



    browseWindow->show();
}

void MainWindow::CloseBrowse()
{
    browseWindowOpen = false;
}

void MainWindow::BrowseModDirectory()
{
    lastModDirectory = QFileDialog::getExistingDirectory(browseWindow);
    ModPath->setText(lastModDirectory);
    browseWindow->raise();
}

void MainWindow::BrowseSGFile()
{
    lastSaveDirectory = QFileDialog::getOpenFileName(browseWindow, "Open File", QString(), "Mount&blade Save (*.sav)");
    SGPath->setText(lastSaveDirectory);
    browseWindow->raise();
}


void MainWindow::ExportSave()
{
    if (treeWidgets.size() > 0)
    {


        QString exportPath = QFileDialog::getSaveFileName(this, "Export SaveGame", lastSaveDirectory, "Mount&blade Save (*.sav)");

        QProgressDialog progress("Exporting...", "Abort Copy", 0, 100, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setCancelButton(0);

        ((QTreeCustom*)curTree->itemAt(0,0))->modInfos->savegame->progressBar = &progress;

        if (((QTreeCustom*)curTree->itemAt(0,0))->modInfos->savegame->ExportData(exportPath.toStdString()))
        {
            progress.close();
            QMessageBox::information(this, "Success", "Savegame succefully exported");
        }
        else
        {
            progress.close();
            QMessageBox::critical(this, "Fail", "Couldn't export savegame, target path file already in use.");
        }


    }
    else
    {
        QMessageBox::warning(this, "Fail", "Import a save game first.");
    }
    //((QTreeCustom*)curTree->itemAt(0,0))->modInfos->savegame->ExportSave(exportPath);
}


void MainWindow::LoadSave()
{

    browseWindow->close();

    QProgressDialog progress("Loading SaveGame File", "Abort Copy", 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setCancelButton(0);

    lastModDirectory = ModPath->text();
    lastSaveDirectory = SGPath->text();

    if (lastModDirectory == "Path to Module Directory File (e.g. : Steam\\SteamApps\\common\\MountBlade Warband\\Modules\\HairyVikingMod)" ||
        lastSaveDirectory == "Path to SaveGame File (e.g. : MyDocuments\\Mount&Blade Warband Savegames\\HairyVikingMod)")
    {
        InfoLog("Wrong path");
        return;
    }

    SaveGameInfos* SG = new SaveGameInfos(ModPath->text().toStdString(),
                                          SGPath->text().toStdString(),
                                          *SGStructure,
                                          *TypeSize,
                                          *ConditionalValueSave,
                                          *WatchedValues,
                                          modINIValues,
                                          &progress,
                                          slotsInfos,
                                          globalVarBehaviours);


    if (SG->loadedSuccefully)
    {
        tabSaves->addTab(CreateTabContent(SG), QString::fromStdString(SG->name));
        saves.push_back(SG);
    }
    else
    {
        delete(SG);
    }

    //((QLineEdit*)line)->setText( QFileDialog::getOpenFileName(this, "Open File", QString(), "Mount&blade Save (*.sav)"));
}



QWidget* MainWindow::CreateTabContent(SaveGameInfos* SGInfos)
{

    QTreeWidget *treeWidget = new QTreeWidget();
    treeWidgets.push_back(treeWidget);

    treeWidget->setColumnCount(3);
    treeWidget->setColumnWidth(1,100);
    QStringList ColumnNames;
    ColumnNames << "Variables" << "Value" << "Type";
    treeWidget->setHeaderLabels(ColumnNames);


    QObject::connect(treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)),
                     this, SLOT(AddTreeChild(QTreeWidgetItem*)));

    QObject::connect(treeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
                     this, SLOT(RemoveTreeChild(QTreeWidgetItem*)));

    QObject::connect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
                     this, SLOT(TryEditItem(QTreeWidgetItem*,int)));

    AddTreeChild(treeWidget,SGInfos);

    return treeWidget;
}

void MainWindow::AddTreeChild(QTreeWidgetItem* parent)
{
    int blockDepth = 0;
    QTreeCustom* parentItem = (QTreeCustom*)parent;

    for (unsigned int k=0; k<parentItem->childs.size();k++)
        delete(parentItem->childs[k]);
    parentItem->childs.clear();

    vector<unsigned int> path = parentItem->pathInTree;
    unsigned int start = 0;

    if (parentItem->state != NORMAL)
    {
        path.resize(path.size()-1);
        start = parentItem->pathInTree[parentItem->pathInTree.size()-1]+1;
    }

    parentItem->modInfos->savegame->currentBlockIdPath = path;
    List<SaveGameItem>* childList = parentItem->modInfos->savegame->GetCurrentVectorTree();

    bool drawID = false;
    if (parentItem->state  == BEGIN && parentItem->modInfos->savegame->typeSize.count((SGItemGetData((*childList)[start - 1])[2]).toStdString()) > 0)
        drawID = true;


    for (unsigned int i=start; i<childList->size(); i++)
    {
        int state = NORMAL;
        if ((*childList)[i].value.find("faketree_end") != (*childList)[i].value.npos)
        {
            blockDepth -= 1;
            if (blockDepth < 0)
                break;
            continue;
        }

        if ((*childList)[i].value.find("faketree_begin") != (*childList)[i].value.npos)
        {
            state = BEGIN;
            blockDepth += 1;
        }

        if (blockDepth < 0)
            break;

        if (blockDepth > 0 && !(blockDepth==1 && state==BEGIN))
            continue;


        QStringList Data = SGItemGetData((*childList)[i]);
        if (drawID)
            Data[0] += QString::fromStdString(" (" + Int64ToString(i-start) + ")");

        QTreeCustom* newItem = new QTreeCustom((QTreeCustom*)parentItem, Data, parentItem->modInfos,state);
        newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
        newItem->pathInTree = path;
        newItem->pathInTree.push_back(i);
        if (Colors.count((*childList)[i].finalBehaviour))
            newItem->setBackground(0,*Colors[(*childList)[i].finalBehaviour]);
        parentItem->childs.push_back(newItem);

        if ((*childList)[i].subItems.size() > 0 || state == BEGIN)
        {
            QTreeCustom* newFakeItem = new QTreeCustom(newItem, Data, parentItem->modInfos,state);
            parentItem->treeWidget()->insertTopLevelItem(0,newFakeItem);
            if (Colors.count((*childList)[i].finalBehaviour))
                newItem->setBackground(0,*Colors[(*childList)[i].finalBehaviour]);
            newItem->childs.push_back(newFakeItem);
        }

        parentItem->treeWidget()->insertTopLevelItem(0, (QTreeWidgetItem*)newItem);
    }
}

void MainWindow::AddTreeChild(QTreeWidget* parent, SaveGameInfos* SGInfos)
{
    int blockDepth = 0;
    SGInfos->savegame->currentBlockIdPath.clear();
    List<SaveGameItem>* childList = SGInfos->savegame->GetCurrentVectorTree();

    for (unsigned int i=0; i<childList->size(); i++)
    {
        int state = NORMAL;
        if ((*childList)[i].value.find("faketree_end") != (*childList)[i].value.npos)
        {
            blockDepth -= 1;
            if (blockDepth < 0)
                break;
            continue;
        }

        if ((*childList)[i].value.find("faketree_begin") != (*childList)[i].value.npos)
        {
            state = BEGIN;
            blockDepth += 1;
        }

        if (blockDepth < 0)
            break;

        if (blockDepth > 0 && !(blockDepth==1 && state==BEGIN))
            continue;

        QStringList Data = SGItemGetData((*childList)[i]);

        QTreeCustom* newItem = new QTreeCustom(parent, Data, SGInfos,state);
        newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
        newItem->pathInTree.push_back(i);
        if (Colors.count((*childList)[i].finalBehaviour))
            newItem->setBackground(0,*Colors[(*childList)[i].finalBehaviour]);

        if ((*childList)[i].subItems.size() > 0 || state == BEGIN)
        {
            QTreeCustom* newFakeItem = new QTreeCustom(newItem, Data, SGInfos,state);
            parent->insertTopLevelItem(0,newFakeItem);
            if (Colors.count((*childList)[i].finalBehaviour))
                newItem->setBackground(0,*Colors[(*childList)[i].finalBehaviour]);
            newItem->childs.push_back(newFakeItem);
        }
        parent->insertTopLevelItem(0, (QTreeWidgetItem*)newItem);
    }
    parent->resizeColumnToContents(0);
    parent->resizeColumnToContents(1);

}


void MainWindow::RemoveTreeChild(QTreeWidgetItem* parent)
{
    QTreeCustom* parentItem = (QTreeCustom*)parent;

    for (unsigned int k=0; k<parentItem->childs.size();k++)
    {
        delete(parentItem->childs[k]);
    }
    parentItem->childs.clear();

    parentItem->modInfos->savegame->currentBlockIdPath = parentItem->pathInTree;

    List<SaveGameItem>* childList = parentItem->modInfos->savegame->GetCurrentVectorTree();


    if (childList->size() > 0 || parentItem->state == BEGIN)
    {
        QStringList Data;
        Data << QString("fake") <<
                QString("fake") <<
                QString("fake");

        QTreeCustom* newFakeItem = new QTreeCustom(parent, Data, parentItem->modInfos,NORMAL);
        parentItem->treeWidget()->insertTopLevelItem(0,newFakeItem);
        parentItem->childs.push_back(newFakeItem);
    }

}

QStringList MainWindow::SGItemGetData(SaveGameItem& SGI)
{

    string name;
    string type;
    string count;
    ostringstream value;

    if (SGI.value.find("faketree_begin") != SGI.value.npos)
    {
        string val = SGI.value;

        val = val.substr(val.find("+")+1,val.npos);
        type = val.substr(0,val.find("+"));

        if (type == "party_record")
        {
            val = val.substr(val.find("+")+1,val.npos);
            name = "#" + val.substr(0,val.find("+"));
            int64_t Id = atoi(val.substr(0,val.find("+")).c_str());

            if (saves[tabSaves->currentIndex()]->modInfos->int_str_PartyRecord_Ids.count(Id))
                name += saves[tabSaves->currentIndex()]->modInfos->int_str_PartyRecord_Ids[Id];
        }
        else
        {
            val = val.substr(val.find("+")+1,val.npos);
            name = "#" + val.substr(0,val.find("+"));
        }


        val = val.substr(val.find("+")+1,val.npos);
        value << val.substr(0,val.npos);
    }
    else
    {
        if (SGI.infos->type == "int32")
            value << (*reinterpret_cast<int32_t*>(&SGI.value[0]));
        else if (SGI.infos->type == "int64")
            value << (*reinterpret_cast<int64_t*>(&SGI.value[0]));
        else if (SGI.infos->type == "uint32")
            value << (*reinterpret_cast<uint32_t*>(&SGI.value[0]));
        else if (SGI.infos->type == "uint64")
            value << (*reinterpret_cast<uint64_t*>(&SGI.value[0]));
        else if (SGI.infos->type == "bool")
            value << (SGI.value[0] == '\0' ? "0" : "1");
        else if (SGI.infos->type == "float")
            value << (*reinterpret_cast<float*>(&SGI.value[0]));
        else
            value << SGI.value;

        name = *SGI.displayName;
        type = SGI.infos->type;
    }


    QStringList Data;
    Data << QString::fromStdString(name) <<
            QString::fromStdString(value.str()) <<
            QString::fromStdString(type);

    return Data;
}

void MainWindow::TryEditItem(QTreeWidgetItem* item, int column)
{
    QTreeCustom* curItem = (QTreeCustom*)item;

    if (!item->isSelected())
        return;

    SaveGameItem* SGI = curItem->modInfos->savegame->GetItemAtPath(&curItem->pathInTree);

    if (column == 1)
    {
        string newValue = curItem->text(column).toStdString();

        if (SGI->isFake)
        {
            curItem->setData(column,0,SGItemGetData(*SGI)[1]);
            return;
        }

        SGI->value.resize((*TypeSize)[SGI->infos->type]);

        if (SGI->infos->type == "int32")
        {
            int32_t out;
            stringstream(newValue) >> out;
            memcpy( &SGI->value[ 0 ], &out, SGI->value.size());
        }
        else if (SGI->infos->type == "int64")
        {
            int64_t out;
            stringstream(newValue) >> out;
            memcpy( &SGI->value[ 0 ], &out, SGI->value.size());
        }
        else if (SGI->infos->type == "uint32")
        {
            uint32_t out;
            stringstream(newValue) >> out;
            memcpy( &SGI->value[ 0 ], &out, SGI->value.size());
        }
        else if (SGI->infos->type == "uint64")
        {
            uint64_t out;
            stringstream(newValue) >> out;
            memcpy( &SGI->value[ 0 ], &out, SGI->value.size());
        }
        else if (SGI->infos->type == "bool")
        {
            SGI->value[0] = (newValue.find_first_of("true1") != newValue.npos ? '\1' : '\0');
        }
        else if (SGI->infos->type == "float")
        {
            float out;
            stringstream(newValue) >> out;
            memcpy( &SGI->value[ 0 ], &out, SGI->value.size());
        }
        else
        {
            SGI->value = newValue;
        }

        if (SGI->infos->type != "char")
            SGI->value.resize((*TypeSize)[SGI->infos->type]);

    }
    curItem->setData(column,0,SGItemGetData(*SGI)[1]);
}

void MainWindow::ChangeTree(int id)
{
    curTree = treeWidgets[id];
}

void MainWindow::ConvertSaveWindow()
{
    if (convertWindowOpen)
        convertWindow->close();

    convertWindow = new QWidget();
    convertWindowOpen = true;
    convertWindow->setMinimumSize(600,100);
    modAToConvertIndex = 0;
    modBToConvertIndex = 0;

    QObject::connect(convertWindow, SIGNAL(destroyed()),
                     this, SLOT(CloseConvertWindow()));



    QVBoxLayout* globalVerticalLayout = new QVBoxLayout(convertWindow);






    QHBoxLayout* descriptionRow = new QHBoxLayout();
    QLabel* description = new QLabel();
    description->setText("In order to convert a save to another mod version, you need two game saves : \n - The save from the original mod version you wish to convert. \n - One random save from the new mod version that the program will use as template.");
    descriptionRow->addWidget(description);






    QHBoxLayout* listsRow = new QHBoxLayout();

    QComboBox *ModA = new QComboBox();
    for (unsigned int i=0; i<tabSaves->count(); i++)
        ModA->addItem(tabSaves->tabText(i));

    QComboBox *ModB = new QComboBox();
    for (unsigned int i=0; i<tabSaves->count(); i++)
        ModB->addItem(tabSaves->tabText(i));

    QLabel* mergeText = new QLabel();
    QLabel* ToText = new QLabel();
    mergeText->setText("Merge ");
    ToText->setText("To ");
    mergeText->setAlignment(Qt::AlignCenter);
    ToText->setAlignment(Qt::AlignCenter);

    listsRow->addWidget(mergeText);
    listsRow->addWidget(ModA);
    listsRow->addWidget(ToText);
    listsRow->addWidget(ModB);

    QObject::connect(ModA, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(ChangeModAIndex(int)));

    QObject::connect(ModB, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(ChangeModBIndex(int)));





    QHBoxLayout* optionsRow1 = new QHBoxLayout();


        replace_parties = new QCheckBox("Parties");
            replace_party_player = new QCheckBox("Player Party");
                replace_party_player_position = new QCheckBox("Position");
                replace_party_player_slots = new QCheckBox("Slots");
                replace_party_player_troops = new QCheckBox("Army");
            replace_party_lords = new QCheckBox("Lords parties");
                replace_party_lords_position = new QCheckBox("Position");
                replace_party_lords_slots = new QCheckBox("Slots");
                replace_party_lords_troops = new QCheckBox("Army");
            replace_party_towns = new QCheckBox("Towns");
                replace_party_towns_position = new QCheckBox("Position");
                replace_party_towns_slots = new QCheckBox("Slots");
                replace_party_towns_troops = new QCheckBox("Garrison");
                replace_party_lords_territories = new QCheckBox("Lords territories");
                replace_party_player_territories = new QCheckBox("Player territories");


        replace_quests = new QCheckBox("Quests");
            replace_quest_slots = new QCheckBox("Quests slots");


        replace_factions = new QCheckBox("Factions");
            replace_faction_relation = new QCheckBox("Factions Relations");
            replace_faction_slots = new QCheckBox("Factions slots");

        replace_party_templates = new QCheckBox("Parties Templates");
            replace_party_template_slots = new QCheckBox("Parties Templates Slots");

        replace_troops = new QCheckBox("Troops");
            replace_troop_player = new QCheckBox("Player");
                replace_troop_player_specs = new QCheckBox("Statistics");
                replace_troop_player_slots = new QCheckBox("Slots");
                replace_troop_player_equipment = new QCheckBox("Equipment");
            replace_troop_lords = new QCheckBox("Lords");
                replace_troop_lord_specs = new QCheckBox("Statistics");
                replace_troop_lord_slots = new QCheckBox("Slots");
                replace_troop_lord_equipment = new QCheckBox("Equipment");
            replace_troop_ladies = new QCheckBox("Ladies");
                replace_troop_lady_specs = new QCheckBox("Statistics");
                replace_troop_lady_slots = new QCheckBox("Slots");
                replace_troop_lady_equipment = new QCheckBox("Equipment");
            replace_troop_heroes = new QCheckBox("Heroes");
                replace_troop_heroes_specs = new QCheckBox("Statistics");
                replace_troop_heroes_slots = new QCheckBox("Slots");
                replace_troop_heroes_equipment = new QCheckBox("Equipment");
            replace_troop_nonheroes = new QCheckBox("Non-Heroes");
                replace_troop_nonheroes_specs = new QCheckBox("Statistics");
                replace_troop_nonheroes_slots = new QCheckBox("Slots");
                replace_troop_nonheroes_equipment = new QCheckBox("Equipment");



    GetSettings();



    QVBoxLayout* rightSettingsColumn = new QVBoxLayout();
    QVBoxLayout* leftSettingsColumn = new QVBoxLayout();


    QHBoxLayout* curRow = new QHBoxLayout();
    curRow->addWidget(replace_parties);
    rightSettingsColumn->addLayout(curRow);


        curRow = new QHBoxLayout();
        curRow->addItem(new QSpacerItem(15, 1));
        curRow->addWidget(replace_party_player);
        rightSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_party_player_position);
            rightSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_party_player_slots);
            rightSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_party_player_troops);
            rightSettingsColumn->addLayout(curRow);


        curRow = new QHBoxLayout();
        curRow->addItem(new QSpacerItem(15, 1));
        curRow->addWidget(replace_party_lords);
        rightSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_party_lords_position);
            rightSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_party_lords_slots);
            rightSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_party_lords_troops);
            rightSettingsColumn->addLayout(curRow);


        curRow = new QHBoxLayout();
        curRow->addItem(new QSpacerItem(15, 1));
        curRow->addWidget(replace_party_towns);
        rightSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_party_towns_position);
            rightSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_party_towns_slots);
            rightSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_party_towns_troops);
            rightSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_party_lords_territories);
            rightSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_party_player_territories);
            rightSettingsColumn->addLayout(curRow);





    curRow = new QHBoxLayout();
    curRow->addWidget(replace_quests);
    rightSettingsColumn->addLayout(curRow);


        curRow = new QHBoxLayout();
        curRow->addItem(new QSpacerItem(15, 1));
        curRow->addWidget(replace_quest_slots);
        rightSettingsColumn->addLayout(curRow);




    curRow = new QHBoxLayout();
    curRow->addWidget(replace_factions);
    rightSettingsColumn->addLayout(curRow);


        curRow = new QHBoxLayout();
        curRow->addItem(new QSpacerItem(15, 1));
        curRow->addWidget(replace_faction_relation);
        rightSettingsColumn->addLayout(curRow);

        curRow = new QHBoxLayout();
        curRow->addItem(new QSpacerItem(15, 1));
        curRow->addWidget(replace_faction_slots);
        rightSettingsColumn->addLayout(curRow);



    curRow = new QHBoxLayout();
    curRow->addWidget(replace_party_templates);
    rightSettingsColumn->addLayout(curRow);


        curRow = new QHBoxLayout();
        curRow->addItem(new QSpacerItem(15, 1));
        curRow->addWidget(replace_party_template_slots);
        rightSettingsColumn->addLayout(curRow);











    curRow = new QHBoxLayout();
    curRow->addWidget(replace_troops);
    leftSettingsColumn->addLayout(curRow);


        curRow = new QHBoxLayout();
        curRow->addItem(new QSpacerItem(15, 1));
        curRow->addWidget(replace_troop_player);
        leftSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_troop_player_specs);
            leftSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_troop_player_slots);
            leftSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_troop_player_equipment);
            leftSettingsColumn->addLayout(curRow);



        curRow = new QHBoxLayout();
        curRow->addItem(new QSpacerItem(15, 1));
        curRow->addWidget(replace_troop_lords);
        leftSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_troop_lord_specs);
            leftSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_troop_lord_slots);
            leftSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_troop_lord_equipment);
            leftSettingsColumn->addLayout(curRow);


        curRow = new QHBoxLayout();
        curRow->addItem(new QSpacerItem(15, 1));
        curRow->addWidget(replace_troop_ladies);
        leftSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_troop_lady_specs);
            leftSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_troop_lady_slots);
            leftSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_troop_lady_equipment);
            leftSettingsColumn->addLayout(curRow);



        curRow = new QHBoxLayout();
        curRow->addItem(new QSpacerItem(15, 1));
        curRow->addWidget(replace_troop_heroes);
        leftSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_troop_heroes_specs);
            leftSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_troop_heroes_slots);
            leftSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_troop_heroes_equipment);
            leftSettingsColumn->addLayout(curRow);



        curRow = new QHBoxLayout();
        curRow->addItem(new QSpacerItem(15, 1));
        curRow->addWidget(replace_troop_nonheroes);
        leftSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_troop_nonheroes_specs);
            leftSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_troop_nonheroes_slots);
            leftSettingsColumn->addLayout(curRow);

            curRow = new QHBoxLayout();
            curRow->addItem(new QSpacerItem(30, 1));
            curRow->addWidget(replace_troop_nonheroes_equipment);
            leftSettingsColumn->addLayout(curRow);


    QHBoxLayout* OKCANCELLayout = new QHBoxLayout();


    QPushButton* Convert = new QPushButton("Convert");
    QPushButton* Cancel = new QPushButton("Cancel");


    optionsRow1->addLayout(rightSettingsColumn);
    optionsRow1->addLayout(leftSettingsColumn);

    OKCANCELLayout->addWidget(Convert);
    OKCANCELLayout->addWidget(Cancel);


    globalVerticalLayout->addLayout(descriptionRow);
    globalVerticalLayout->addLayout(listsRow);
    globalVerticalLayout->addLayout(optionsRow1);
    globalVerticalLayout->addLayout(OKCANCELLayout);


    QObject::connect(Convert, SIGNAL(clicked()),
                     this, SLOT(ConvertSave()));

    QObject::connect(Cancel, SIGNAL(clicked()),
                     convertWindow, SLOT(close()));



    convertWindow->show();

}

void MainWindow::CloseConvertWindow()
{
    browseWindowOpen = false;
}

void MainWindow::ConvertSave()
{
    SetSettings();
    if (modAToConvertIndex == modBToConvertIndex)
    {
        if (tabSaves->count() > 1)
            QMessageBox::warning(convertWindow, "Fail", "Saves to convert must be different.");
        else
            QMessageBox::warning(convertWindow, "Fail", "You need two saves in order to process conversion.");

        return;
    }

    if (tabSaves->count() < 2)
    {
        QMessageBox::warning(convertWindow, "Fail", "You need two saves in order to process conversion.");
        return;
    }

    InfoLog("Ids : " << modAToConvertIndex << " , " << modBToConvertIndex);
    InfoLog("converting : " << (saves[modAToConvertIndex]->name) << " to " << (saves[modBToConvertIndex]->name));

    QProgressDialog progress("Converting...", "Abort Copy", 0, 100, convertWindow);
    progress.setWindowModality(Qt::WindowModal);
    progress.setCancelButton(0);
    progress.show();

    saves[modBToConvertIndex]->savegame->progressBar = &progress;

    saves[modBToConvertIndex]->savegame->ConvertFrom(saves[modAToConvertIndex],ConversionSettings);
    progress.close();
    QMessageBox::information(convertWindow, "Success", "Conversion is done. Add any changes you wish (or please don't if it wasn't your intention) to " + QString::fromStdString(saves[modBToConvertIndex]->name) + " and save it with a different name. (sg00-08.sav)");

    treeWidgets[modBToConvertIndex]->clear();
    AddTreeChild(treeWidgets[modBToConvertIndex],saves[modBToConvertIndex]);
    convertWindow->close();
    tabSaves->setCurrentIndex(modBToConvertIndex);
}

void MainWindow::ChangeModAIndex(int newId)
{
    modAToConvertIndex = newId;
}

void MainWindow::ChangeModBIndex(int newId)
{
    modBToConvertIndex = newId;
}

void MainWindow::GetSettings()
{
    replace_parties->setChecked(ConversionSettings.replace_parties);
        replace_party_player->setChecked(ConversionSettings.replace_party_player);
            replace_party_player_position->setChecked(ConversionSettings.replace_party_player_position);
            replace_party_player_slots->setChecked(ConversionSettings.replace_party_player_slots);
            replace_party_player_troops->setChecked(ConversionSettings.replace_party_player_troops);
        replace_party_lords->setChecked(ConversionSettings.replace_party_lords);
            replace_party_lords_position->setChecked(ConversionSettings.replace_party_lords_position);
            replace_party_lords_slots->setChecked(ConversionSettings.replace_party_lords_slots);
            replace_party_lords_troops->setChecked(ConversionSettings.replace_party_lords_troops);
        replace_party_towns->setChecked(ConversionSettings.replace_party_towns);
            replace_party_towns_position->setChecked(ConversionSettings.replace_party_towns_position);
            replace_party_towns_slots->setChecked(ConversionSettings.replace_party_towns_slots);
            replace_party_towns_troops->setChecked(ConversionSettings.replace_party_towns_troops);
            replace_party_lords_territories->setChecked(ConversionSettings.replace_party_lords_territories);
            replace_party_player_territories->setChecked(ConversionSettings.replace_party_player_territories);


    replace_quests->setChecked(ConversionSettings.replace_quests);
        replace_quest_slots->setChecked(ConversionSettings.replace_quest_slots);


    replace_factions->setChecked(ConversionSettings.replace_factions);
        replace_faction_relation->setChecked(ConversionSettings.replace_faction_relation);
        replace_faction_slots->setChecked(ConversionSettings.replace_faction_slots);

    replace_party_templates->setChecked(ConversionSettings.replace_party_templates);
        replace_party_template_slots->setChecked(ConversionSettings.replace_party_template_slots);

    replace_troops->setChecked(ConversionSettings.replace_troops);
        replace_troop_player->setChecked(ConversionSettings.replace_troop_player);
            replace_troop_player_specs->setChecked(ConversionSettings.replace_troop_player_specs);
            replace_troop_player_slots->setChecked(ConversionSettings.replace_troop_player_slots);
            replace_troop_player_equipment->setChecked(ConversionSettings.replace_troop_player_equipment);
        replace_troop_lords->setChecked(ConversionSettings.replace_troop_lords);
            replace_troop_lord_specs->setChecked(ConversionSettings.replace_troop_lord_specs);
            replace_troop_lord_slots->setChecked(ConversionSettings.replace_troop_lord_slots);
            replace_troop_lord_equipment->setChecked(ConversionSettings.replace_troop_lord_equipment);
        replace_troop_ladies->setChecked(ConversionSettings.replace_troop_ladies);
            replace_troop_lady_specs->setChecked(ConversionSettings.replace_troop_lady_specs);
            replace_troop_lady_slots->setChecked(ConversionSettings.replace_troop_lady_slots);
            replace_troop_lady_equipment->setChecked(ConversionSettings.replace_troop_lady_equipment);
        replace_troop_heroes->setChecked(ConversionSettings.replace_troop_heroes);
            replace_troop_heroes_specs->setChecked(ConversionSettings.replace_troop_heroes_specs);
            replace_troop_heroes_slots->setChecked(ConversionSettings.replace_troop_heroes_slots);
            replace_troop_heroes_equipment->setChecked(ConversionSettings.replace_troop_heroes_equipment);
        replace_troop_nonheroes->setChecked(ConversionSettings.replace_troop_nonheroes);
            replace_troop_nonheroes_specs->setChecked(ConversionSettings.replace_troop_nonheroes_specs);
            replace_troop_nonheroes_slots->setChecked(ConversionSettings.replace_troop_nonheroes_slots);
            replace_troop_nonheroes_equipment->setChecked(ConversionSettings.replace_troop_nonheroes_equipment);
}

void MainWindow::SetSettings()
{
    ConversionSettings.replace_parties = replace_parties->isChecked();
        ConversionSettings.replace_party_player = replace_party_player->isChecked();
            ConversionSettings.replace_party_player_position = replace_party_player_position->isChecked();
            ConversionSettings.replace_party_player_slots = replace_party_player_slots->isChecked();
            ConversionSettings.replace_party_player_troops = replace_party_player_troops->isChecked();
        ConversionSettings.replace_party_lords = replace_party_lords->isChecked();
            ConversionSettings.replace_party_lords_position = replace_party_lords_position->isChecked();
            ConversionSettings.replace_party_lords_slots = replace_party_lords_slots->isChecked();
            ConversionSettings.replace_party_lords_troops = replace_party_lords_troops->isChecked();
        ConversionSettings.replace_party_towns = replace_party_towns->isChecked();
            ConversionSettings.replace_party_towns_position = replace_party_towns_position->isChecked();
            ConversionSettings.replace_party_towns_slots = replace_party_towns_slots->isChecked();
            ConversionSettings.replace_party_towns_troops = replace_party_towns_troops->isChecked();
            ConversionSettings.replace_party_lords_territories = replace_party_lords_territories->isChecked();
            ConversionSettings.replace_party_player_territories= replace_party_player_territories->isChecked();


    ConversionSettings.replace_quests = replace_quests->isChecked();
        ConversionSettings.replace_quest_slots = replace_quest_slots->isChecked();


    ConversionSettings.replace_factions = replace_factions->isChecked();
        ConversionSettings.replace_faction_relation = replace_faction_relation->isChecked();
        ConversionSettings.replace_faction_slots = replace_faction_slots->isChecked();

    ConversionSettings.replace_party_templates = replace_party_templates->isChecked();
        ConversionSettings.replace_party_template_slots = replace_party_template_slots->isChecked();

    ConversionSettings.replace_troops = replace_troops->isChecked();
        ConversionSettings.replace_troop_player = replace_troop_player->isChecked();
            ConversionSettings.replace_troop_player_specs = replace_troop_player_specs->isChecked();
            ConversionSettings.replace_troop_player_slots = replace_troop_player_slots->isChecked();
            ConversionSettings.replace_troop_player_equipment = replace_troop_player_equipment->isChecked();
        ConversionSettings.replace_troop_lords = replace_troop_lords->isChecked();
            ConversionSettings.replace_troop_lord_specs = replace_troop_lord_specs->isChecked();
            ConversionSettings.replace_troop_lord_slots = replace_troop_lord_slots->isChecked();
            ConversionSettings.replace_troop_lord_equipment = replace_troop_lord_equipment->isChecked();
        ConversionSettings.replace_troop_ladies = replace_troop_ladies->isChecked();
            ConversionSettings.replace_troop_lady_specs = replace_troop_lady_specs->isChecked();
            ConversionSettings.replace_troop_lady_slots = replace_troop_lady_slots->isChecked();
            ConversionSettings.replace_troop_lady_equipment = replace_troop_lady_equipment->isChecked();
        ConversionSettings.replace_troop_heroes = replace_troop_heroes->isChecked();
            ConversionSettings.replace_troop_heroes_specs = replace_troop_heroes_specs->isChecked();
            ConversionSettings.replace_troop_heroes_slots = replace_troop_heroes_slots->isChecked();
            ConversionSettings.replace_troop_heroes_equipment = replace_troop_heroes_equipment->isChecked();
        ConversionSettings.replace_troop_nonheroes = replace_troop_nonheroes->isChecked();
            ConversionSettings.replace_troop_nonheroes_specs = replace_troop_nonheroes_specs->isChecked();
            ConversionSettings.replace_troop_nonheroes_slots = replace_troop_nonheroes_slots->isChecked();
            ConversionSettings.replace_troop_nonheroes_equipment = replace_troop_nonheroes_equipment->isChecked();
}

void MainWindow::keyPressEvent( QKeyEvent * event )
{

    if( event->key() == Qt::Key_F3 )
    {
        saves[1]->savegame->watchedValues.clear();
        saves[1]->savegame->conditionalValueSave.clear();
        saves[1]->savegame->saveGameFile.str("");
        saves[1]->modInfos->str_int_Party_Ids.clear();
        saves[1]->modInfos->int_str_Party_Ids.clear();
        saves[1]->modInfos->str_int_PartyRecord_Ids.clear();
        saves[1]->modInfos->int_str_PartyRecord_Ids.clear();
        saves[1]->modInfos->int_str_Party_Id_Ids.clear();
    }
    if( event->key() == Qt::Key_F4 )
    {
        saves[1]->savegame->SaveGameData.clear();
    }
    if( event->key() == Qt::Key_F5 )
    {
        delete(saves[1]->savegame);
    }
    if( event->key() == Qt::Key_F6 )
    {
        delete(saves[1]->modInfos);
    }
    if( event->key() == Qt::Key_F7 )
    {
        delete(saves[1]);
    }
}
