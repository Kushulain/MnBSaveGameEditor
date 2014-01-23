#ifndef MODINFOS_H
#define MODINFOS_H


#include "CommonFunctions.h"
#include <stdint.h>

struct troop
{
    int32_t numberId;
    map<string,string> infos;
};


class ModInfos
{
    public:
        ModInfos(string directoryPath,vector<string>* modINIValues, map<string,int64_t>* watchedValues);
        virtual ~ModInfos();

        void InitializeVariables();
        bool LoadTroops(string filePath);
        bool LoadModuleINI(string filePath,vector<string>* modINIValues, map<string,int64_t>* watchedValues);
        bool LoadGlobalVariables(string filePath);
        bool LoadQuestIds(string filePath);
        bool LoadPartyTemplateIds(string filePath);
        bool LoadSceneIds(string filePath);
        bool LoadItemIds(string filePath);
        bool LoadFactionIds(string filePath);

        int64_t GetOthersFactionId(int64_t cur_id, ModInfos* Other);
        int64_t GetOthersTroopId(int64_t cur_id, ModInfos* Other);
        int64_t GetOthersQuestId(int64_t cur_id, ModInfos* Other);
        int64_t GetOthersPartyId(int64_t cur_id, ModInfos* Other);
        int64_t GetOthersPartyRecordId(int64_t cur_id, ModInfos* Other);
        int64_t GetOthersPartyTemplateId(int64_t cur_id, ModInfos* Other);
        int64_t GetOthersItemId(int64_t cur_id, ModInfos* Other);
        int64_t GetOthersSceneId(int64_t cur_id, ModInfos* Other);
        int64_t GetOthersGVarId(int64_t cur_id, ModInfos* Other);

        vector<string> troopStructure[6];


        //////////////////////////////////////////////////////////////////ID -> NAME loaded with files
        map<string,int64_t> str_int_globalVariables_Ids;
        vector<string> int_str_globalVariables_Ids;

        map<string,int64_t> str_int_Quest_Ids;
        vector<string> int_str_Quest_Ids;

        map<string,int64_t> str_int_PartyTemplate_Ids;
        vector<string> int_str_PartyTemplate_Ids;

        map<string,int64_t> str_int_Scene_Ids;
        vector<string> int_str_Scene_Ids;

        map<string,int64_t> str_int_Item_Ids;
        vector<string> int_str_Item_Ids;


        map<string,int64_t> str_int_Faction_Ids;
        vector<string> int_str_Faction_Ids;

        vector<troop> troops;
        map<string,int64_t> str_int_Troops_Ids;

        //////////////////////////////////////////////////////////////////ID -> NAME loaded during SG Parsing
        map<string,int64_t> str_int_Party_Ids;
        map<int64_t,string> int_str_Party_Ids;   //<- parties are not continuous

        map<string,int64_t> str_int_PartyRecord_Ids;
        map<int64_t,string> int_str_PartyRecord_Ids;

        map<int64_t,string> int_str_Party_Id_Ids;

        bool loadedSuccefully;

        ifstream bufferFile;

    protected:
    private:
};

#endif // MODINFOS_H
