#ifndef SAVEGAME_H
#define SAVEGAME_H

#include "CommonFunctions.h"
#include "ModInfos.h"
#include <stdint.h>
#include <QProgressDialog>
#include <QCoreApplication>
#include "SaveGameInfos.h"

//class SaveGameInfos;

enum MergeBehaviourEnum
{
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
    REPLACE_BY_ITEM_ID = 13                 //brown
};

struct SaveGameStructureItem
{
    string type; //int32 uint string ...
    string dataCount; //a number 1 , 5, etc or a text relative to a row's description
    string name;
    string condition;
    string info;
    int mergeBehaviour;
};

struct UserSettings
{
    bool replace_parties;
        bool replace_party_player;
            bool replace_party_player_position;
            bool replace_party_player_slots;
            bool replace_party_player_troops;
        bool replace_party_lords;
            bool replace_party_lords_position;
            bool replace_party_lords_slots;
            bool replace_party_lords_troops;
        bool replace_party_towns;
            bool replace_party_towns_position;
            bool replace_party_towns_slots;
            bool replace_party_towns_troops;
            bool replace_party_lords_territories;
            bool replace_party_player_territories;


    bool replace_quests;
        bool replace_quest_slots;


    bool replace_factions;
        bool replace_faction_relation;
        bool replace_faction_slots;

    bool replace_party_templates;
        bool replace_party_template_slots;

    bool replace_troops;
        bool replace_troop_player;
            bool replace_troop_player_specs;
            bool replace_troop_player_slots;
            bool replace_troop_player_equipment;
        bool replace_troop_lords;
            bool replace_troop_lord_specs;
            bool replace_troop_lord_slots;
            bool replace_troop_lord_equipment;
        bool replace_troop_ladies;
            bool replace_troop_lady_specs;
            bool replace_troop_lady_slots;
            bool replace_troop_lady_equipment;
        bool replace_troop_heroes;
            bool replace_troop_heroes_specs;
            bool replace_troop_heroes_slots;
            bool replace_troop_heroes_equipment;
        bool replace_troop_nonheroes;
            bool replace_troop_nonheroes_specs;
            bool replace_troop_nonheroes_slots;
            bool replace_troop_nonheroes_equipment;

        UserSettings()
        {
            replace_parties = true;
                replace_party_player = true;
                    replace_party_player_position = true;
                    replace_party_player_slots = true;
                    replace_party_player_troops = true;
                replace_party_lords = true;
                    replace_party_lords_position = true;
                    replace_party_lords_slots = true;
                    replace_party_lords_troops = true;
                replace_party_towns = true;
                    replace_party_towns_position = false;
                    replace_party_towns_slots = true;
                    replace_party_towns_troops = true;
                    replace_party_lords_territories = true;
                    replace_party_player_territories = true;


            replace_quests = true;
                replace_quest_slots = true;


            replace_factions = true;
                replace_faction_relation = true;
                replace_faction_slots = true;

            replace_party_templates = true;
                replace_party_template_slots = true;

            replace_troops = true;
                replace_troop_player = true;
                    replace_troop_player_specs = true;
                    replace_troop_player_slots = true;
                    replace_troop_player_equipment = true;
                replace_troop_lords = true;
                    replace_troop_lord_specs = true;
                    replace_troop_lord_slots = true;
                    replace_troop_lord_equipment = false;
                replace_troop_ladies = true;
                    replace_troop_lady_specs = true;
                    replace_troop_lady_slots = true;
                    replace_troop_lady_equipment = false;
                replace_troop_heroes = true;
                    replace_troop_heroes_specs = true;
                    replace_troop_heroes_slots = true;
                    replace_troop_heroes_equipment = true;
                replace_troop_nonheroes = false;
                    replace_troop_nonheroes_specs = false;
                    replace_troop_nonheroes_slots = false;
                    replace_troop_nonheroes_equipment = false;
        }
};


template <typename T >
class List : public list< T >
{
private:
    typename List<T>::iterator itr;
public:
    int lastId;
    int m_size;


    List<T>() : list<T>()
    {
        //DebugLog(" common consttructor ");
        lastId = -1;
        itr = list<T>::end();
        m_size = 0;
    }

    List<T>(const List<T>& SGIs) : list<T>()
    {
        //DebugLog(" copy const consttructor "); <- push_back SGI
        clear();
        insert(0,SGIs.begin(),SGIs.end());
    }


    List<T>(List<T>&& SGIs)
    {
        //DebugLog(" copy rvalue consttructor ");
        clear();
        insert(0,SGIs.begin(),SGIs.end());
    }

    List<T>& operator=(List<T>& SGIs)
    {
        //DebugLog(" copy = ref consttructor ");
        clear();
        insert(0,SGIs.begin(),SGIs.end());
        return *this;
    }

    List<T>& operator=(const List<T>& SGIs)
    {
        //DebugLog(" copy = const consttructor ");
        clear();
        insert(0,SGIs.begin(),SGIs.end());
        return *this;
    }

    List<T>& operator=(List<T>&& SGIs)
    {
        //DebugLog(" copy = rvalue consttructor ");
        clear();
        insert(0,SGIs.begin(),SGIs.end());
        return *this;
    }

    T& operator[](int id)
    {
        if (id > lastId)
        {
            while (id != lastId)
            {
                lastId++;
                itr++;
            }
        }
        else
        {
            while (id != lastId)
            {
                lastId--;
                itr--;
            }
        }
        return *itr;
    }

    void erase(int first, int last)
    {
        (*this)[first];
        typename List<T>::iterator firstItr = itr;
        (*this)[last];
        typename List<T>::iterator lastItr = itr;

        list<T>::erase(firstItr,lastItr);
        lastId = first;
        m_size -= (last - first);
    }

    void erase(int cur)
    {
        (*this)[cur];
        typename List<T>::iterator curItr = itr;

        itr = list<T>::erase(curItr);
        lastId = cur;
        m_size--;
    }

    void clear()
    {
        list<T>::clear();
        m_size = 0;
        lastId = -1;
        itr = list<T>::end();
    }

    void insert(int id, const T &val)
    {
        (*this)[id];
        list<T>::insert(itr,val);
        lastId = id + 1;
        m_size++;
    }

    int size()
    {
        return m_size;
    }

    template <class InputIterator>
    void insert(int id, InputIterator _First, InputIterator _Last)
    {
        (*this)[id];
        list<T>::insert(itr,_First,_Last);
        m_size += distance(_First,_Last);
        itr = list<T>::end();
        lastId = -1;
    }

    void push_back(const T &val)
    {
        m_size++;
        list<T>::push_back(val);
    }

    void pop_back()
    {
        m_size--;
        list<T>::pop_back();
        lastId = -1;
        itr = list<T>::end();
    }
};

struct SaveGameItem
{
    string* displayName;
    SaveGameStructureItem* infos;
    int finalBehaviour;
    bool isFake; //does the exporter have to export this value ?
    //string type; //int32 uint string ...
    //string dataCount; //a number 1 , 5, etc or a text relative to a row's description
    //string condition;
    //string info;
    string value; //raw binary data from the .sav
    List<SaveGameItem> subItems;

    /*
    SaveGameItem(SaveGameItem&& SGI) :
        finalBehaviour(move(SGI.finalBehaviour)),
        value(move(SGI.value)),
        isFake(move(SGI.isFake)),
        subItems(move(SGI.subItems))
    {
        finalBehaviour = 13;
        displayName = SGI.displayName;
        infos = SGI.infos;
    }

    SaveGameItem(const SaveGameItem& SGI)
    {
        finalBehaviour = 5;
        infos = SGI.infos;
        value = SGI.value;
        displayName = SGI.displayName;
        isFake = SGI.isFake;
        subItems = SGI.subItems;
    }

    SaveGameItem& operator=(const SaveGameItem& SGI)
    {
        finalBehaviour = 10;// SGI.finalBehaviour;
        infos = SGI.infos;
        value = SGI.value;
        displayName = SGI.displayName;
        isFake = SGI.isFake;
        subItems = SGI.subItems;
        return *this;
    }

    SaveGameItem& operator=(SaveGameItem&& SGI)
    {
        finalBehaviour = SGI.finalBehaviour;
        infos = SGI.infos;
        value = SGI.value;
        displayName = SGI.displayName;
        isFake = SGI.isFake;
        subItems.clear();
        if (SGI.subItems.size() > 0)
            subItems = SGI.subItems;

        return *this;
    }*/


/*
    void operator=(const SaveGameItem& SGI)
    {
        finalBehaviour = finalBehaviour;
        infos = SGI.infos;
        value = SGI.value;
        displayName = SGI.displayName;
        isFake = SGI.isFake;
        subItems.clear();
        if (SGI.subItems.size() > 0)
            subItems = SGI.subItems;
    }*/

/*

    void operator=(SaveGameItem& SGI)
    {
        infos = SGI.infos;
        value = SGI.value;
        displayName = SGI.displayName;
        isFake = SGI.isFake;
        subItems.clear();
        if (SGI.subItems.size() > 0)
            subItems = SGI.subItems;
    }

    void operator=(const SaveGameItem& SGI)
    {
        infos = SGI.infos;
        value = SGI.value;
        displayName = SGI.displayName;
        isFake = SGI.isFake;
        subItems.clear();
        if (SGI.subItems.size() > 0)
            subItems = SGI.subItems;
    }*/

/*
    SaveGameItem(SaveGameStructureItem& item)
    {
        type = item.type;
        dataCount = item.dataCount;
        name = item.name;
        condition = item.condition;
        info = item.info;
    }
*/
    SaveGameItem()
    {
        finalBehaviour = DONTREPLACE;
    }
    ~SaveGameItem()
    {
        subItems.clear();
       // delete(displayName);
    }
};

struct Slot
{
    string name;
    int mergeBehaviour;
};

struct SlotsInfos
{
    map<unsigned int, Slot> troopSlots;
    map<unsigned int, Slot> factionSlots;
    map<unsigned int, Slot> partySlots;
    map<unsigned int, Slot> questSlots;
    map<unsigned int, Slot> itemSlots;
    map<unsigned int, Slot> sceneSlots;
    map<unsigned int, Slot> partyTemplateSlots;

    unsigned int troopSlotsMaxId;
    unsigned int factionSlotsMaxId;
    unsigned int partySlotsMaxId;
    unsigned int questSlotsMaxId;
    unsigned int itemSlotsMaxId;
    unsigned int sceneSlotsMaxId;
    unsigned int partyTemplateSlotsMaxId;
};

enum slotsTypes
{
    isUnrecognizedSlot = 0,
    isTroopSlot = 1,
    isFactionSlot = 2,
    isPartySlot = 3,
    isQuestSlot = 4,
    isItemSlot = 5,
    isSceneSlot = 6,
    isPartyTemplateSlot = 7,
};

class SaveGameInfos;

class SaveGame
{
    public:
        SaveGame(string filePath,
                 map<string,vector<SaveGameStructureItem> > SGStructure,
                 map<string,streamsize> aTypeSize,
                 map<string,string> aConditionalValueSave,
                 map<string,int64_t> aWatchedValues,
                 ModInfos* aModInfos,
                 QProgressDialog* aProgressBar,
                 SlotsInfos* aSlotsInfos);

        virtual ~SaveGame();

        void ReadAsBlock(string& blockType, string& blockName, unsigned int blockNumber);
        void StructureItemToSGItem(SaveGameItem& SGData, SaveGameStructureItem& SGSI);
        SaveGameItem* GetItemAtPath(vector<int>* aPath);
        SaveGameItem* GetItemAtPath(vector<unsigned int>* aPath);
        void InsertSGIAtMarker(SaveGameItem& SGI, vector<int>* aPath);
        void InsertSGIAtMarker(List<SaveGameItem>& SGIs, vector<int>* aPath);
        void RemoveSGIAtMarker(vector<int>* aPath);
        void RemoveSGIAtMarker(vector<int>* aPath, int count);
        List<SaveGameItem>* GetCurrentVectorTree();
        string IsConditionTrue(string condition, uint32_t start);
        string Evaluate(string& value);
        string LookForSGVariable(string& address);
        string LookForLocalSGVariable(string& variable);
        bool ExportData(string filePath);
        void ExportTree(List<SaveGameItem>& curTree);

        void ConvertFrom(SaveGameInfos* originalMod, UserSettings Settings);
        void ConvertItemList(List<SaveGameItem>& curTree,vector<int>& marker, int mergeBehaviour = REPLACE);
        int GetNextSGI(string& name, vector<int>& marker);
        int GetNextBeginSGI(string& name, vector<int>& marker);
        unsigned int HandleFakeItem(List<SaveGameItem>& curTree, unsigned int id, vector<int>& marker,  int mergeBehaviour = REPLACE);
        vector<string> GetFakeSGIBeginData(string val);
        void MergeSGI(SaveGameItem* originalSGI, SaveGameItem* newSGI, int mergeBehave);
        int CombineBehaviours(int behaveParent, int behaveCurrent);
        void ShiftCurIdList(unsigned int position, int shiftAmount);
        int GetSlotBehave(int id, int slotType);
        void CleanPartyStack(List<SaveGameItem>& list);


        bool loadedSuccefully;

        ModInfos* modInfos;
        SaveGameStructureItem* fakeSGSI;

        map<string,vector<SaveGameStructureItem> > structureBlocks;
        List<SaveGameItem> SaveGameData;

        vector<string> currentBlockNamePath;
        vector<unsigned int> currentBlockIdPath;

        string saveGameBodyName;

        map<string,streamsize> typeSize;
        map<string,string> conditionalValueSave;
        map<string,int64_t> watchedValues;
        vector<unsigned int> playerTroopsIDs;
        SlotsInfos slotsInfos;

        istringstream saveGameFile;
        uint64_t fileLength;

        ofstream saveGameExportFile;

        int curPercentage;

        uint64_t lastTT;
        stringstream toLog;

        /* Optimisations: this store position of some block for better look up while converting*/
        List<SaveGameItem>* troopsList;
        vector<unsigned int> troopsIDInList;

        List<SaveGameItem>* partyRecordsList;
        vector<unsigned int> partyRecordsIDInList;

        List<SaveGameItem>* partyTemplateList;
        vector<unsigned int> partyTemplateIDInList;

        List<SaveGameItem>* questsList;
        vector<unsigned int> questsIDInList;

        List<SaveGameItem>* scenesList;
        vector<unsigned int> scenesIDInList;

        List<SaveGameItem>* itemsList;
        vector<unsigned int> itemsIDInList;

        List<SaveGameItem>* factionsList;
        vector<unsigned int> factionsIDInList;

        SaveGameInfos* originalSaveInfos;

        QProgressDialog* progressBar;


        vector<unsigned int>* curIDList;

        UserSettings userSettings;

        string* sumPointerToEmptyStr;

        bool browsingPlayers;
        bool browsingLords;
        bool browsingLadies;
        bool browsingTowns;
        bool browsingHeroes;
        bool browsingNonHeroes;

        bool browsingParty;
        bool browsingParty_Template;
        bool browsingFactions;
        bool browsingQuests;
        bool browsingTroops;
        bool browsingItem;
        bool browsingSite;

        uint64_t inputsCount;
        uint64_t curInputs;
        string* lastNumStacks;

    protected:
    private:
};

#endif // SAVEGAME_H
