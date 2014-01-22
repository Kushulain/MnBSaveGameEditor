#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <stdlib.h>
#include <ctime>
#include <stdio.h>
#include "CommonFunctions.h"
#include "ModInfos.h"
#include "SaveGame.h"
#include "savegameinfos.h"
#include <QtDebug>
#include <QFileDialog>
#include <QSignalMapper>
#include <QProgressDialog>
#include <QMessageBox>
#include <QComboBox>
#include <QLabel>
#include "qtreecustom.h"
#include <QCheckBox>
#include <QtGui>




class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(
                        map<string,vector<SaveGameStructureItem> >* aSGStructure,
                        map<string,streamsize>* aTypeSize,
                        map<string,string>* aConditionalValueSave,
                        map<string,int64_t>* aWatchedValues,
                        vector<string>* amodINIValues,
                        SlotsInfos* aSlotsInfos);

    QWidget* CreateTabContent(SaveGameInfos* SGInfos);
    QStringList SGItemGetData(SaveGameItem& SGI);
    void keyPressEvent(QKeyEvent *);


    map<int,QBrush*> Colors;

    bool browseWindowOpen;
    QWidget* browseWindow;

    bool convertWindowOpen;
    QWidget* convertWindow;

    QLineEdit* ModPath;
    QLineEdit* SGPath;

    QTabWidget* tabSaves;
    vector<QTreeWidget*> treeWidgets;
    QTreeWidget* curTree;

    vector<SaveGameInfos*> saves;

    QString lastModDirectory;
    QString lastSaveDirectory;
    UserSettings ConversionSettings;

    int modAToConvertIndex;
    int modBToConvertIndex;

    map<string,vector<SaveGameStructureItem> >* SGStructure;
    map<string,streamsize>* TypeSize;
    map<string,string>* ConditionalValueSave;
    map<string,int64_t>* WatchedValues;
    vector<string>* modINIValues;
    SlotsInfos* slotsInfos;

    bool createdCheckBoxes;

    QCheckBox* replace_parties;
        QCheckBox* replace_party_player;
            QCheckBox* replace_party_player_position;
            QCheckBox* replace_party_player_slots;
            QCheckBox* replace_party_player_troops;
        QCheckBox* replace_party_lords;
            QCheckBox* replace_party_lords_position;
            QCheckBox* replace_party_lords_slots;
            QCheckBox* replace_party_lords_troops;
        QCheckBox* replace_party_towns;
            QCheckBox* replace_party_towns_position;
            QCheckBox* replace_party_towns_slots;
            QCheckBox* replace_party_towns_troops;
            QCheckBox* replace_party_lords_territories;
            QCheckBox* replace_party_player_territories;


    QCheckBox* replace_quests;
        QCheckBox* replace_quest_slots;


    QCheckBox* replace_factions;
        QCheckBox* replace_faction_relation;
        QCheckBox* replace_faction_slots;

    QCheckBox* replace_party_templates;
        QCheckBox* replace_party_template_slots;

    QCheckBox* replace_troops;
        QCheckBox* replace_troop_player;
            QCheckBox* replace_troop_player_specs;
            QCheckBox* replace_troop_player_slots;
            QCheckBox* replace_troop_player_equipment;
        QCheckBox* replace_troop_lords;
            QCheckBox* replace_troop_lord_specs;
            QCheckBox* replace_troop_lord_slots;
            QCheckBox* replace_troop_lord_equipment;
        QCheckBox* replace_troop_ladies;
            QCheckBox* replace_troop_lady_specs;
            QCheckBox* replace_troop_lady_slots;
            QCheckBox* replace_troop_lady_equipment;
        QCheckBox* replace_troop_heroes;
            QCheckBox* replace_troop_heroes_specs;
            QCheckBox* replace_troop_heroes_slots;
            QCheckBox* replace_troop_heroes_equipment;
        QCheckBox* replace_troop_nonheroes;
            QCheckBox* replace_troop_nonheroes_specs;
            QCheckBox* replace_troop_nonheroes_slots;
            QCheckBox* replace_troop_nonheroes_equipment;

signals:

public slots:
    void BrowseSGWindow();
    void CloseBrowse();

    void BrowseModDirectory();
    void BrowseSGFile();
    void LoadSave();
    void AddTreeChild(QTreeWidgetItem* parent);
    void AddTreeChild(QTreeWidget* parent, SaveGameInfos* SGInfos);
    void RemoveTreeChild(QTreeWidgetItem* parent);
    void TryEditItem(QTreeWidgetItem* item, int column = 0);
    void ChangeTree(int id);
    void ExportSave();

    void ConvertSaveWindow();
    void CloseConvertWindow();

    void ChangeModAIndex(int newId);
    void ChangeModBIndex(int newId);

    void ConvertSave();
    void GetSettings();
    void SetSettings();

};

#endif // MAINWINDOW_H
