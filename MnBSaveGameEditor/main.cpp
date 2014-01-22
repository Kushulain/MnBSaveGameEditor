#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <stdlib.h>
#include <stdint.h>
#include <QApplication>
#include <QPushButton>

#include "CommonFunctions.h"
#include "SaveGame.h"
#include "ModInfos.h"

#include "mainwindow.h"





using namespace std;
using namespace CommonFunctions;

/*
struct IdVector
{
    unsigned int id;
    vector<IdVector> subIds;
};*/

enum structureColumns
{
    Type =		0,
    Count =     1,
    Field =     2,
    Condition = 3,
    Info =      4,
    MergeBehaviour =      5
};



map<string,vector<SaveGameStructureItem> > structureBlocks;
SlotsInfos slotsInfos;


string saveGameBodyName = "game";

map<string,streamsize> typeSize;
map<string,int64_t> watchedValues;
map<string,string> conditionalValueSave;

ifstream saveGameFile;

 vector<string> modINIValues;


bool testVar = false;
SaveGame* saveGame;
ModInfos* modInfos;

void LoadStructure(string filePath);
void LoadSlotInfos(string filePath);
void LoadSaveGame(string filePath);
void InitializeVariables();



void InitializeVariables()
{
	typeSize["int32"] = 4;
	typeSize["int64"] = 8;
	typeSize["uint32"] = 4;
	typeSize["uint64"] = 8;
	typeSize["bool"] = 1;
	typeSize["float"] = 4;
	typeSize["char"] = 1;

	watchedValues["playerPartyFlags"] = 1048576;
	watchedValues["tf_hero"] = 16;
	watchedValues["hero"] = 16;
	//watchedValues["dont_load_regular_troop_inventories"] = 1;

                            //VALUE TO SAVE                      //VARIABLENAME                 //SAVE CONDTITION
	conditionalValueSave["game/party_records/party/num_stacks"] = "player_party.num_stacks : flags & playerPartyFlags";

	conditionalValueSave["game/party_records/party/flags"] = "currentPartyFlags : true";
	conditionalValueSave["game/troops/flags"] = "flags : true"; //optimisation
	conditionalValueSave["game/troops/renamed"] = "renamed : true"; //optimisation
	conditionalValueSave["game/party_records/valid"] = "valid : true"; //optimisation
	conditionalValueSave["game/map_event_records/valid"] = "valid : true"; //optimisation
	//conditionalValueSave["game/party_records/party/party_stack/troop_id"] = "playerTroopsIds# : currentPartyFlags & playerPartyFlags";

	modINIValues.push_back("dont_load_regular_troop_inventories");
}

struct teststruct
{
    string str;
};

int main(int argc, char *argv[])
{
    /*
    cout << "size of vector<SGI> : " << sizeof(vector<SaveGameItem>) << endl;
    cout << "size of SGI : " << sizeof(SaveGameItem) << endl;
    cout << "size of bool : " << sizeof(bool) << endl;
    cout << "size of char : " << sizeof(char) << endl;
    cout << "size of str : " << sizeof(string) << endl;
    cout << "size of int : " << sizeof(int) << endl;
    cout << "size of SaveGameStructureItem* : " << sizeof(SaveGameStructureItem*) << endl;*/

    //ResetLogFile();
    QApplication app(argc, argv);


    InitializeVariables();
    InfoLog("");
    InfoLog("====LOADING STRUCTURE FILES :");
    InfoLog("");
    LoadStructure("SaveGameStructure.txt");

    InfoLog("");
    InfoLog("====LOADING SLOTNAMES FILES :");
    InfoLog("");
    LoadSlotInfos("SlotsNames.txt");


    MainWindow* window = new MainWindow(&structureBlocks,&typeSize,&conditionalValueSave,&watchedValues,&modINIValues, &slotsInfos);
    window->show();
/*
    QPushButton bouton("Load");
    bouton.show();*/

    //if (bouton.tr)



    /*
	InfoLog("");
	InfoLog("====LOADING MODULE FILES :");
	InfoLog("");
    modInfos = new ModInfos("J:/Steam/SteamApps/common/MountBlade Warband/Modules/ACOK 1.0",&modINIValues,&watchedValues);*/

    /*

	InfoLog("");
	InfoLog("====LOADING SAVEGAME FILES :");
	InfoLog("");
    LoadSaveGame("sg00.sav");


    cout << "Done.";*/

    return app.exec();
}


void LoadStructure(string filePath)
{
	ifstream structureFile(filePath.c_str());

   if(structureFile)
   {
      string line;
      string newBlockName = saveGameBodyName;

      while(getline(structureFile, line))
      {

        //SKIP LINE
        if (line.find("Type") == 0 )
        {
            continue;
        }

        size_t commentAddress = line.find("//");

        if (commentAddress != line.npos)
            line.resize(commentAddress);
        if (line.size() == 0)
            continue;

        //Is new block name
        if (line.find("\t") == line.npos || line.find(" ") == line.npos) //there are no "space" nor "tab" within the line
        {
            DebugLog("Loading new structure block : " << line);
            newBlockName = line;
            structureBlocks[newBlockName] = vector<SaveGameStructureItem>();
            continue;
        }

        SaveGameStructureItem data;
        data.condition = "";
        data.type = "";
        data.name = "";
        data.info = "";
        data.dataCount = "";

        string input = "";
        int column = 0;

        for (unsigned int i = 0; i < line.size(); i++)
        {
            if (line[i] == ' ' && column != Condition)
				continue;

            if (line.find("\t",i) == i || i == (line.size()-1))
			{
				if (column == Type)
				{
					data.type = input;
					//cout << "Type : " << input << ";";
				}
				if (column == Count)
				{
					data.dataCount = input;
					//cout << "Count : " << input << ";";
				}
				if (column == Field)
				{
					data.name = input;
					//cout << "Field : " << input << ";";
				}
				if (column == Condition)
				{
					data.condition = input;
                    //cout << "Condition : " << input << ";";
				}
				if (column == Info)
				{
					data.info = input;
                    //cout << "Info : " << input << ";" << endl;
				}

                if (column == MergeBehaviour)
                {
                    input += line[i];

                    CleanString(input);
                    if (input == "DONTREPLACE")
                        data.mergeBehaviour = DONTREPLACE;
                    else if (input == "REPLACE")
                        data.mergeBehaviour = REPLACE;
                    else if (input == "CLEAR")
                        data.mergeBehaviour = CLEAR;

                    else if (input == "DONTREPLACE_EXCEPTPLAYERS")
                        data.mergeBehaviour = DONTREPLACE_EXCEPTPLAYERS;
                    else if (input == "REPLACE_EXCEPTPLAYERS")
                        data.mergeBehaviour = REPLACE_EXCEPTPLAYERS;

                    else if (input == "REPLACE_BY_FACTION_ID")
                        data.mergeBehaviour = REPLACE_BY_FACTION_ID;
                    else if (input == "REPLACE_BY_GLOBALVAR_ID")
                        data.mergeBehaviour = REPLACE_BY_GLOBALVAR_ID;
                    else if (input == "REPLACE_BY_PARTY_ID")
                        data.mergeBehaviour = REPLACE_BY_PARTY_ID;
                    else if (input == "REPLACE_BY_PARTY_TEMPLATE_ID")
                        data.mergeBehaviour = REPLACE_BY_PARTY_TEMPLATE_ID;
                    else if (input == "REPLACE_BY_PARTY_RECORD_ID")
                        data.mergeBehaviour = REPLACE_BY_PARTY_RECORD_ID;
                    else if (input == "REPLACE_BY_QUEST_ID")
                        data.mergeBehaviour = REPLACE_BY_QUEST_ID;
                    else if (input == "REPLACE_BY_SCENE_ID")
                        data.mergeBehaviour = REPLACE_BY_SCENE_ID;
                    else if (input == "REPLACE_BY_TROOP_ID")
                        data.mergeBehaviour = REPLACE_BY_TROOP_ID;
                    else if (input == "REPLACE_BY_ITEM_ID")
                        data.mergeBehaviour = REPLACE_BY_ITEM_ID;

                    //cout << "merge : " << data.mergeBehaviour << ";" << endl;
                }
				input = "";
				column++;
				continue;
			}

			input.push_back(line[i]);
        }
        structureBlocks[newBlockName].push_back(data);

      }
   }
   else
   {
        ErrorLog("Unable to find structure file : " << filePath);
   }
   InfoLog("number of structure blocks loaded : " << structureBlocks.size());


}

void LoadSlotInfos(string filePath)
{
    slotsInfos.itemSlotsMaxId = 0;
    slotsInfos.factionSlotsMaxId = 0;
    slotsInfos.partySlotsMaxId = 0;
    slotsInfos.partyTemplateSlotsMaxId = 0;
    slotsInfos.questSlotsMaxId = 0;
    slotsInfos.sceneSlotsMaxId = 0;
    slotsInfos.troopSlotsMaxId = 0;
    ifstream slotFile(filePath.c_str());

    if(slotFile)
    {
        string line;
        string behave;
        string name;
        string value;

        while(getline(slotFile, line))
        {
            for (int i=line.size()-1; i >= 0; i--)
              if (line[i] == '\t')
                  line[i] = ' ';

            size_t commentAddress = line.find("#");
            if (commentAddress != line.npos)
              line.resize(commentAddress);

            if (line.size() == 0)
              continue;


            ///////////////////////////////////////////////////////////////////////////// split string in three
            behave = line.substr(0,line.find(" "));
            name = line.substr(line.find(" "), line.find("=") - line.find(" "));
            value = line.substr(line.find("=")+1,line.size() - (line.find("=")+1));

            CleanString(behave);
            CleanString(name);
            CleanString(value);

            if (behave.size() == 0 || name.size() == 0 || value.size() == 0)
              continue;

            //cout << name << "," << behave << "," << value << "." <<  endl;


            ///////////////////////////////////////////////////////////////////////////// id string to int
            unsigned int id = 0;
            unsigned int slotCount = 0;
            if (value.find("-") != value.npos)
            {
              id = atoi(value.substr(0,value.find("-")).c_str());
              slotCount = atoi(value.substr(value.find("-")+1,value.size()-(value.find("-")+1)).c_str());
            }
            else
            {
              id = atoi(value.c_str());
              slotCount = id;
            }
            slotCount += 1;


            int mergeBehave = 0;
            ///////////////////////////////////////////////////////////////////////////// behave string to int
            if (behave == "DONTREPLACE")
                mergeBehave = DONTREPLACE;
            else if (behave == "REPLACE")
                mergeBehave = REPLACE;
            else if (behave == "CLEAR")
                mergeBehave = CLEAR;

            else if (behave == "DONTREPLACE_EXCEPTPLAYERS")
                mergeBehave = DONTREPLACE_EXCEPTPLAYERS;
            else if (behave == "REPLACE_EXCEPTPLAYERS")
                mergeBehave = REPLACE_EXCEPTPLAYERS;

            else if (behave == "REPLACE_BY_FACTION_ID")
                mergeBehave = REPLACE_BY_FACTION_ID;
            else if (behave == "REPLACE_BY_GLOBALVAR_ID")
                mergeBehave = REPLACE_BY_GLOBALVAR_ID;
            else if (behave == "REPLACE_BY_PARTY_ID")
                mergeBehave = REPLACE_BY_PARTY_ID;
            else if (behave == "REPLACE_BY_PARTY_TEMPLATE_ID")
                 mergeBehave = REPLACE_BY_PARTY_TEMPLATE_ID;
            else if (behave == "REPLACE_BY_PARTY_RECORD_ID")
                mergeBehave = REPLACE_BY_PARTY_RECORD_ID;
            else if (behave == "REPLACE_BY_QUEST_ID")
                mergeBehave = REPLACE_BY_QUEST_ID;
            else if (behave == "REPLACE_BY_SCENE_ID")
                mergeBehave = REPLACE_BY_SCENE_ID;
            else if (behave == "REPLACE_BY_TROOP_ID")
                mergeBehave = REPLACE_BY_TROOP_ID;
            else if (behave == "REPLACE_BY_ITEM_ID")
                mergeBehave = REPLACE_BY_ITEM_ID;


            for (unsigned int i=id; i < slotCount; i++)
            {
                if (name.find("slot_item") != name.npos)
                {
                    slotsInfos.itemSlots[i].name = name;
                    slotsInfos.itemSlots[i].mergeBehaviour = mergeBehave;
                    slotsInfos.itemSlotsMaxId = max(slotsInfos.itemSlotsMaxId,i);
                }
                else if  (name.find("slot_faction") != name.npos)
                {
                    slotsInfos.factionSlots[i].name = name;
                    slotsInfos.factionSlots[i].mergeBehaviour = mergeBehave;
                    slotsInfos.factionSlotsMaxId = max(slotsInfos.factionSlotsMaxId,i);
                }
                else if  (name.find("slot_scene") != name.npos)
                {
                    slotsInfos.sceneSlots[i].name = name;
                    slotsInfos.sceneSlots[i].mergeBehaviour = mergeBehave;
                    slotsInfos.sceneSlotsMaxId = max(slotsInfos.sceneSlotsMaxId,i);
                }
                else if  (name.find("slot_troop") != name.npos ||
                          name.find("slot_lord") != name.npos ||
                          name.find("slot_lady") != name.npos)
                {
                    slotsInfos.troopSlots[i].name = name;
                    slotsInfos.troopSlots[i].mergeBehaviour = mergeBehave;
                    slotsInfos.troopSlotsMaxId = max(slotsInfos.troopSlotsMaxId,i);
                }
                else if  (name.find("slot_quest") != name.npos)
                {
                    slotsInfos.questSlots[i].name = name;
                    slotsInfos.questSlots[i].mergeBehaviour = mergeBehave;
                    slotsInfos.questSlotsMaxId = max(slotsInfos.questSlotsMaxId,i);
                }
                else if  (name.find("slot_party_template") != name.npos)                    //////MUST BE BEFORE SLOT_PARTY
                {
                    slotsInfos.partyTemplateSlots[i].name = name;
                    slotsInfos.partyTemplateSlots[i].mergeBehaviour = mergeBehave;
                    slotsInfos.partyTemplateSlotsMaxId = max(slotsInfos.partyTemplateSlotsMaxId,i);
                }
                else if  (name.find("slot_party") != name.npos ||
                          name.find("slot_town") != name.npos ||
                          name.find("slot_center") != name.npos ||
                          name.find("slot_village") != name.npos)
                {
                    slotsInfos.partySlots[i].name = name;
                    slotsInfos.partySlots[i].mergeBehaviour = mergeBehave;
                    slotsInfos.partySlotsMaxId = max(slotsInfos.partySlotsMaxId,i);
                }
                else
                {
                    ErrorLog("unrecognized slot name,behave,value. : " << name << "," << behave << "," << value << ".");
                }
            }
        }
   }
   else
   {
        ErrorLog("Unable to find slot file : " << filePath);
   }
   InfoLog("Slot File succefully loaded. faction:" << slotsInfos.factionSlotsMaxId << " item:" << slotsInfos.itemSlotsMaxId << " party:" << slotsInfos.partySlotsMaxId << " partyTemplate:" << slotsInfos.partyTemplateSlotsMaxId << " troops:" << slotsInfos.troopSlotsMaxId);

}
