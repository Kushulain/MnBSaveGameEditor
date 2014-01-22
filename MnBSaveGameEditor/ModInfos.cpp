#include "ModInfos.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <stdlib.h>
#include "CommonFunctions.h"


using namespace CommonFunctions;

ModInfos::ModInfos(string directoryPath, vector<string>* modINIValues, map<string,int64_t>* watchedValues)
{
    InitializeVariables();
    string curFilePath = directoryPath;
    loadedSuccefully = true;

    curFilePath += "/troops.txt";
    if (!LoadTroops(curFilePath))
        loadedSuccefully = false;

    curFilePath = directoryPath;
    curFilePath += "/module.ini";
    if (!LoadModuleINI(curFilePath,modINIValues, watchedValues))
        loadedSuccefully = false;

    curFilePath = directoryPath;
    curFilePath += "/variables.txt";
    if (!LoadGlobalVariables(curFilePath))
        loadedSuccefully = false;

    curFilePath = directoryPath;
    curFilePath += "/quests.txt";
    if (!LoadQuestIds(curFilePath))
        loadedSuccefully = false;

    curFilePath = directoryPath;
    curFilePath += "/party_templates.txt";
    if (!LoadPartyTemplateIds(curFilePath))
        loadedSuccefully = false;

    curFilePath = directoryPath;
    curFilePath += "/scenes.txt";
    if (!LoadSceneIds(curFilePath))
        loadedSuccefully = false;

    curFilePath = directoryPath;
    curFilePath += "/item_kinds1.txt";
    if (!LoadItemIds(curFilePath))
        loadedSuccefully = false;

    curFilePath = directoryPath;
    curFilePath += "/factions.txt";
    if (!LoadFactionIds(curFilePath))
        loadedSuccefully = false;

}



ModInfos::~ModInfos()
{
    //dtor
}


void ModInfos::InitializeVariables()
{
    troopStructure[0].push_back("IDName");
    troopStructure[0].push_back("name");
    troopStructure[0].push_back("pluralName");
    troopStructure[0].push_back("unknown");
    troopStructure[0].push_back("flags");
}


bool ModInfos::LoadTroops(string filePath)
{
    ifstream bufferFile(filePath.c_str());
    unsigned int row = 0;
    troop bufferTroop;

    if (bufferFile)
    {
        string line;

        getline(bufferFile, line);
        getline(bufferFile, line);

        while(getline(bufferFile, line))
        {
            size_t commentAddress = line.find("//");

            if (commentAddress != line.npos)
                line.resize(commentAddress);

            //SKIP LINE
            if (line.size() == 0 )
            {
                bufferTroop.numberId = troops.size();
                troops.push_back(bufferTroop);
                str_int_Troops_Ids[bufferTroop.infos["IDName"]] = troops.size()-1;
                row = 0;
                bufferTroop.infos.clear();
                continue;
            }

            string input = "";
            int column = 0;

            for (unsigned int i = 0; i < line.size(); i++)
            {
                if (line[i] == ' ')
                {
                    if (input.size() > 0)
                    {
                        if (row < 6 && column < troopStructure[row].size())
                        {
                            string infoName = troopStructure[row][column];

                            if (infoName == " " || infoName == "unknown")
                            {
                                bufferTroop.infos["unknown"] = input;
                            }
                            else
                            {
                                bufferTroop.infos[infoName] = input;
                            }
                        }
                        else
                        {
                            break;
                        }
                        input.clear();
                        column++;
                    }
                    continue;
                }

            input.push_back(line[i]);
            }
            row++;
        }
        if (bufferTroop.infos.size() > 0)
        {
            bufferTroop.numberId = troops.size();
            troops.push_back(bufferTroop);
            str_int_Troops_Ids[bufferTroop.infos["IDName"]] = troops.size()-1;
        }
    }
    else
    {
        ErrorLog("Unable to find troop file : " << filePath);
        return false;
    }
    InfoLog("number of troops loaded : " << troops.size());
    return true;
}

bool ModInfos::LoadModuleINI(string filePath,vector<string>* modINIValues, map<string,int64_t>* watchedValues )
{
    ifstream INIFile(filePath.c_str());

    if (INIFile)
    {
        string line;
        while(getline(INIFile, line))
        {
            size_t commentAddress = line.find("#");

            if (commentAddress != line.npos)
                line.resize(commentAddress);


            CleanString(line);

            unsigned int equalPos = line.find("=");

            //SKIP LINE
            if (line.size() == 0 || equalPos == line.npos)
            {
                continue;
            }

            string key = line.substr(0,equalPos);
            string value = line.substr(equalPos+1,line.npos);


            for (unsigned int i=0; i < modINIValues->size(); i++)
            {
               if ((*modINIValues)[i] == key)
               {
                    (*watchedValues)[key] = atoll(value);
                    //cout << "key : " << key << endl;
                   // cout << "value : " << atoll(&value[0]) << endl;
               }
            }


        }
    }
    else
    {
        ErrorLog("Unable to find INI file : " << filePath);
        return false;
    }
    InfoLog("INI File Loaded.");
    return true;
}


bool ModInfos::LoadGlobalVariables(string filePath)
{
    ifstream variablesFile(filePath.c_str());

    if (variablesFile)
    {
        string line;
        while(getline(variablesFile, line))
        {
            int_str_globalVariables_Ids.push_back(line);
            str_int_globalVariables_Ids[line] = int_str_globalVariables_Ids.size()-1;
        }
    }
    else
    {
        ErrorLog("Unable to find variables.txt file : " << filePath);
        return false;
    }
    InfoLog("Variables.txt file loaded : " << int_str_globalVariables_Ids.size());
    return true;

}

bool ModInfos::LoadQuestIds(string filePath)
{
    ifstream bufFile(filePath.c_str());
    string questId;
    size_t namePos;


    if (bufFile)
    {
        string line;
        while(getline(bufFile, line))
        {
            namePos = line.find("qst_");
            if (namePos != line.npos)
            {
                questId = line.substr(namePos,line.find(" ",namePos)-namePos);
                int_str_Quest_Ids.push_back(questId);
                str_int_Quest_Ids[questId] = int_str_Quest_Ids.size()-1;
                //cout << "\"" << questId << "\" at " << int_str_Quest_Ids.size()-1 << endl;
            }
        }
    }
    else
    {
        ErrorLog("Unable to find quests.txt file : " << filePath);
        return false;
    }
    InfoLog("quests.txt file loaded : " << int_str_Quest_Ids.size());
    return true;

}


bool ModInfos::LoadPartyTemplateIds(string filePath)
{
    ifstream bufFile(filePath.c_str());
    string pty_temp;
    size_t namePos;

    if (bufFile)
    {
        string line;
        while(getline(bufFile, line))
        {
            namePos = line.find("pt_");
            if (namePos != line.npos)
            {
                pty_temp = line.substr(namePos,line.find(" ",namePos)-namePos);
                int_str_PartyTemplate_Ids.push_back(pty_temp);
                str_int_PartyTemplate_Ids[pty_temp] = int_str_PartyTemplate_Ids.size()-1;
                //cout << "\"" << pty_temp << "\" at " << int_str_PartyTemplate_Ids.size()-1 << endl;
            }
        }
    }
    else
    {
        ErrorLog("Unable to find party_templates.txt file : " << filePath);
        return false;
    }
    InfoLog("party_templates.txt file loaded : " << int_str_PartyTemplate_Ids.size());
    return true;

}


bool ModInfos::LoadSceneIds(string filePath)
{
    ifstream bufFile(filePath.c_str());
    string sceneId;
    size_t namePos;

    if (bufFile)
    {
        string line;
        while(getline(bufFile, line))
        {
            namePos = line.find("scn_");
            if (namePos != line.npos)
            {
                sceneId = line.substr(namePos,line.find(" ",namePos)-namePos);
                int_str_Scene_Ids.push_back(sceneId);
                str_int_Scene_Ids[sceneId] = int_str_Scene_Ids.size()-1;
                //cout << "\"" << sceneId << "\" at " << int_str_Scene_Ids.size()-1 << endl;
            }
        }
    }
    else
    {
        ErrorLog("Unable to find scenes.txt file : " << filePath);
        return false;
    }
    InfoLog("scenes.txt file loaded : " << int_str_Scene_Ids.size());
    return true;

}


bool ModInfos::LoadItemIds(string filePath)
{
    ifstream bufFile(filePath.c_str());
    string itemId;
    size_t namePos;

    if (bufFile)
    {
        string line;
        while(getline(bufFile, line))
        {
            namePos = line.find("itm_");
            if (namePos != line.npos)
            {
                itemId = line.substr(namePos,line.find(" ",namePos)-namePos);
                int_str_Item_Ids.push_back(itemId);
                str_int_Item_Ids[itemId] = int_str_Item_Ids.size()-1;
                //cout << "\"" << itemId << "\" at " << int_str_Item_Ids.size()-1 << endl;
            }
        }
    }
    else
    {
        ErrorLog("Unable to find item_kinds1.txt file : " << filePath);
        return false;
    }
    InfoLog("item_kinds1.txt file loaded : " << int_str_Item_Ids.size());
    return true;

}


bool ModInfos::LoadFactionIds(string filePath)
{
    ifstream bufFile(filePath.c_str());
    string itemId;
    size_t namePos;

    if (bufFile)
    {
        string line;
        while(getline(bufFile, line))
        {
            namePos = line.find("fac_");
            if (namePos != line.npos)
            {
                itemId = line.substr(namePos,line.find(" ",namePos)-namePos);
                int_str_Faction_Ids.push_back(itemId);
                str_int_Faction_Ids[itemId] = int_str_Faction_Ids.size()-1;
                //cout << "\"" << itemId << "\" at " << int_str_Faction_Ids.size()-1 << endl;
            }
        }
    }
    else
    {
        ErrorLog("Unable to find factions.txt file : " << filePath);
        return false;
    }
    InfoLog("factions.txt file loaded : " << int_str_Faction_Ids.size());
    return true;

}

int64_t ModInfos::GetOthersFactionId(int64_t cur_id, ModInfos* Other)
{
    if (cur_id <= 0)
        return cur_id;

    if (cur_id >= int_str_Faction_Ids.size())
        return cur_id;

    string IDString = int_str_Faction_Ids[cur_id];
    map<string,int64_t>::const_iterator itr = Other->str_int_Faction_Ids.find(IDString);

    if (itr != Other->str_int_Faction_Ids.end())
        return itr->second;
    else
        return -666;
}

int64_t ModInfos::GetOthersTroopId(int64_t cur_id, ModInfos* Other)
{
    if (cur_id <= 2) //some slots can use first numbers for specific behaviour
        return cur_id;

    if (cur_id >= troops.size())
        return cur_id;

    string IDString = troops[cur_id].infos["IDName"];
    map<string,int64_t>::const_iterator itr = Other->str_int_Troops_Ids.find(IDString);

    if (itr != Other->str_int_Troops_Ids.end())
    {
        return itr->second;
    }
    else
    {
        return -666;
    }
}

int64_t ModInfos::GetOthersQuestId(int64_t cur_id, ModInfos* Other)
{
    if (cur_id <= 0)
        return cur_id;

    if (cur_id >= int_str_Quest_Ids.size())
        return cur_id;

    string IDString = int_str_Quest_Ids[cur_id];
    map<string,int64_t>::const_iterator itr = Other->str_int_Quest_Ids.find(IDString);

    if (itr != Other->str_int_Quest_Ids.end())
        return itr->second;
    else
        return -666;
}

int64_t ModInfos::GetOthersPartyId(int64_t cur_id, ModInfos* Other)
{
    if (cur_id <= 2) //some slots can use first numbers for specific behaviour
        return cur_id;

    if (int_str_Party_Ids.find(cur_id) == int_str_Party_Ids.end())
        return -666;

    string IDString = int_str_Party_Ids[cur_id];
    map<string,int64_t>::const_iterator itr = Other->str_int_Party_Ids.find(IDString);

    if (itr != Other->str_int_Party_Ids.end())
        return itr->second;
    else
        return -666;
}

int64_t ModInfos::GetOthersPartyRecordId(int64_t cur_id, ModInfos* Other)
{
    if (cur_id <= 0)
        return cur_id;

    if (cur_id >= int_str_PartyRecord_Ids.size())
        return cur_id;

    string IDString = int_str_PartyRecord_Ids[cur_id];
    map<string,int64_t>::const_iterator itr = Other->str_int_PartyRecord_Ids.find(IDString);

    if (itr != Other->str_int_PartyRecord_Ids.end())
        return itr->second;
    else
        return -666;
}

int64_t ModInfos::GetOthersPartyTemplateId(int64_t cur_id, ModInfos* Other)
{
    if (cur_id <= 0)
        return cur_id;

    if (cur_id >= int_str_PartyTemplate_Ids.size())
        return cur_id;

    string IDString = int_str_PartyTemplate_Ids[cur_id];
    map<string,int64_t>::const_iterator itr = Other->str_int_PartyTemplate_Ids.find(IDString);

    if (itr != Other->str_int_PartyTemplate_Ids.end())
        return itr->second;
    else
        return -666;
}

int64_t ModInfos::GetOthersItemId(int64_t cur_id, ModInfos* Other)
{
    if (cur_id <= 0)
        return cur_id;

    if (cur_id >= int_str_Item_Ids.size())
        return cur_id;

    string IDString = int_str_Item_Ids[cur_id];
    map<string,int64_t>::const_iterator itr = Other->str_int_Item_Ids.find(IDString);

    if (itr != Other->str_int_Item_Ids.end())
        return itr->second;
    else
        return -666;
}

int64_t ModInfos::GetOthersSceneId(int64_t cur_id, ModInfos* Other)
{
    if (cur_id <= 0)
        return cur_id;

    if (cur_id >= int_str_Scene_Ids.size())
        return cur_id;

    string IDString = int_str_Scene_Ids[cur_id];
    map<string,int64_t>::const_iterator itr = Other->str_int_Scene_Ids.find(IDString);

    if (itr != Other->str_int_Scene_Ids.end())
        return itr->second;
    else
        return -666;
}

