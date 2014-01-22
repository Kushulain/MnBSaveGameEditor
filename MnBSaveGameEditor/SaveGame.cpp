#include "SaveGame.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <stdlib.h>
#include "CommonFunctions.h"
#include <ctime>
#include <stdio.h>
/*
#include <chrono>
#include <thread>
#include <windows.h>*/

using namespace CommonFunctions;
//using namespace std::chrono;

SaveGame::SaveGame( string filePath,
                    map<string,vector<SaveGameStructureItem> > SGStructure,
                    map<string,streamsize> aTypeSize,
                    map<string,string> aConditionalValueSave,
                    map<string,int64_t> aWatchedValues,
                    ModInfos* aModInfos,
                    QProgressDialog* aProgressBar,
                    SlotsInfos* aSlotsInfos)
{
    saveGameBodyName = "game";

    typeSize = aTypeSize;
    conditionalValueSave = aConditionalValueSave;
    watchedValues = aWatchedValues;
    structureBlocks = SGStructure;
    progressBar = aProgressBar;
    slotsInfos = *aSlotsInfos;

    browsingPlayers = false;
    browsingLords = false;
    browsingLadies = false;
    browsingTowns = false;
    browsingHeroes = false;
    browsingNonHeroes = false;

    browsingParty = false;
    browsingParty_Template = false;
    browsingFactions = false;
    browsingQuests = false;
    browsingTroops = false;
    browsingItem = false;
    browsingSite = false;

    inputsCount = 0;
    curInputs = 0;

    modInfos = aModInfos;

    DebugLog("Loading Savegame at Path : " << filePath);

	/*saveGameFile = fopen(filePath.c_str(),  ios_base::in | ios_base::binary );
    saveGameFile.seekg (0, saveGameFile.end);
    fileLength = saveGameFile.tellg();
    saveGameFile.seekg (0, saveGameFile.beg);*/

	FILE* saveGameFileBuffer = fopen(filePath.c_str(), "rb" );


    fseek (saveGameFileBuffer, 0, SEEK_END);
    fileLength = ftell(saveGameFileBuffer);
    fseek (saveGameFileBuffer, 0, SEEK_SET);

    string buffer;
    buffer.resize(fileLength);
    fread(&buffer[0],1,fileLength,saveGameFileBuffer);
    fseek (saveGameFileBuffer, 0, SEEK_SET);
    //istringstream test(buffer, ios_base::in | ios_base::binary);

    saveGameFile.str(buffer);

    curPercentage = 0;

    LogLevel = 2;

    lastTT = 0;
    fakeSGSI = new SaveGameStructureItem();
    sumPointerToEmptyStr = new string("");


/*
    system_clock::time_point m_oStart;
    m_oStart = high_resolution_clock::now();

    saveGameFile.read(&buffer[0],1000000);

    uint64_t diff1 = duration_cast<nanoseconds>(high_resolution_clock::now() - m_oStart ).count();
    cout << "temps : " << diff1 << endl;*/

	if(saveGameFile)
	{
        loadedSuccefully = true;
        ReadAsBlock(saveGameBodyName, saveGameBodyName, 1);
	}
	else
	{
        loadedSuccefully = false;
        ErrorLog("Unable to find savegame file : " << filePath);
    }
    saveGameFile.str("");
    cout << "Input Count : " << inputsCount << endl;
}

SaveGame::~SaveGame()
{
    //dtor
}



void SaveGame::ReadAsBlock(string& blockType, string& blockName, unsigned int blockNumber)
{

        List<SaveGameItem>* curTreeVector = GetCurrentVectorTree();

        currentBlockNamePath.push_back(blockName);

        unsigned int dataCount;
        string path;
        SaveGameItem SGI;

        if      (blockType == "faction")
            factionsList = curTreeVector;
        else if (blockType == "site")
            scenesList = curTreeVector;
        else if (blockType == "party_template")
            partyTemplateList = curTreeVector;
        else if (blockType == "party_record")
            partyRecordsList = curTreeVector;
        else if (blockType == "troop")
            troopsList = curTreeVector;
        else if (blockType == "item_kind")
            itemsList = curTreeVector;
        else if (blockType == "quest")
            questsList = curTreeVector;

		for (unsigned int curBlockId=0; curBlockId < blockNumber; curBlockId++)
		{
            if      (blockType == "faction")
                factionsIDInList.push_back(curTreeVector->size());
            else if (blockType == "site")
                scenesIDInList.push_back(curTreeVector->size());
            else if (blockType == "party_template")
                partyTemplateIDInList.push_back(curTreeVector->size());
            else if (blockType == "party_record")
                partyRecordsIDInList.push_back(curTreeVector->size());
            else if (blockType == "troop")
                troopsIDInList.push_back(curTreeVector->size());
            else if (blockType == "item_kind")
                itemsIDInList.push_back(curTreeVector->size());
            else if (blockType == "quest")
                questsIDInList.push_back(curTreeVector->size());



            if (blockNumber > 1)
            {
                string ID = "";
                ID = Int64ToString(curBlockId);


                if (blockType == "troop")
                    ID += "(" + modInfos->troops[curBlockId].infos["IDName"] + " " + modInfos->troops[curBlockId].infos["name"] + ")";
                else if (blockType == "item_kind")
                    ID += "(" + modInfos->int_str_Item_Ids[curBlockId] + ")";
                else if (blockType == "quest")
                    ID += "(" + modInfos->int_str_Quest_Ids[curBlockId] + ")";
                else if (blockType == "party_template")
                    ID += "(" + modInfos->int_str_PartyTemplate_Ids[curBlockId] + ")";
                else if (blockType == "site")
                    ID += "(" + modInfos->int_str_Scene_Ids[curBlockId] + ")";
                else if (blockType == "faction")
                    ID += "(" + modInfos->int_str_Faction_Ids[curBlockId] + ")";

/*
                cout << "ID : " << ID << endl;

                if (curBlockId == 675 && blockType == "troop")
                    LogLevel = 3;*/

                SaveGameItem fakeHeader;
                fakeHeader.value = "faketree_begin+" + blockType + "+" + ID + "+" + " ";
                fakeHeader.infos = fakeSGSI;
                fakeHeader.displayName = &blockType;
                fakeHeader.isFake = true;
                curTreeVector->push_back(fakeHeader);
            }

			for (unsigned int curStructureItemId=0; curStructureItemId < structureBlocks[blockType].size(); curStructureItemId++)
			{
				SaveGameStructureItem& itemStructure = structureBlocks[blockType][curStructureItemId];
                bool createdAFakeSGI = false;
                bool createEmptyString = false;

				if (itemStructure.dataCount.find("num_") != itemStructure.dataCount.npos)
                {
					dataCount = atoi(&Evaluate(itemStructure.dataCount)[0]);                                   //PREVIOUSLY REFERENCED COUNT
                    //DebugLog("found count raw : " << Evaluate(itemStructure.dataCount));
                    //DebugLog("found count : " << dataCount);

                    if (itemStructure.type == "char")
                    {
                        if (dataCount == 0)
                        {
                            dataCount = 1;
                            createEmptyString = true;
                        }
                    }
				}
				else
				{
					dataCount = atoi(&itemStructure.dataCount[0]);                                  //FIX COUNT
				}


				if (typeSize.count(itemStructure.type))
				{
                    //StructureItemToSGItem(SGI,itemStructure);


                    SGI.infos = &itemStructure;
                    streamsize valueSize = typeSize[itemStructure.type];



                    ostringstream log;

                    for (unsigned int k = 0; k < currentBlockNamePath.size(); k++)
                    {
                        log << currentBlockNamePath[k] << "/" ;
                    }
                    log << itemStructure.name;
                    path = log.str();


                    unsigned int slotType = 0;
                    if (dataCount > 1 && path.find("slots"))
                    {
                        if (path == "game/troops/slots")
                            slotType = isTroopSlot;
                        else if (path == "game/party_records/party/slots")
                            slotType = isPartySlot;
                        else if (path == "game/quests/slots")
                            slotType = isQuestSlot;
                        else if (path == "game/sites/slots")
                            slotType = isSceneSlot;
                        else if (path == "game/party_templates/slots")
                            slotType = isPartyTemplateSlot;
                        else if (path == "game/factions/slots")
                            slotType = isFactionSlot;
                        else if (path == "game/item_kind/slots")
                            slotType = isItemSlot;
                    }

                    int numCreated = 0;
					for (unsigned int curDataId=0; curDataId < dataCount; curDataId++)
                    {

                        if (dataCount == 1)
                        {
                            if (path.find("player_party_stack_additional_info") != path.npos)
                            {
                                watchedValues["cur_stack_troop_flags"] = ToInt64(modInfos->troops[playerTroopsIDs[curBlockId]].infos["flags"]);
                                watchedValues["cur_stack_no"] = curBlockId;
                            }
                        }

                        //cout << "test : " << clock() << " - " << lastClock << endl;

                        if (itemStructure.condition != "")
                        {
                            string conditionResults = IsConditionTrue(itemStructure.condition,0);
                            DebugLog( itemStructure.condition << " Evaluated to " << conditionResults);
                            if (atoi(conditionResults.c_str()) == 0)
                                continue;
                        }

                        if (curDataId == 0 && (dataCount > 1 || itemStructure.dataCount.find("num_") != itemStructure.dataCount.npos) && itemStructure.type != "char")
                        {
                            createdAFakeSGI = true;
                            string ID = itemStructure.name;

                            SaveGameItem fakeHeader;
                            fakeHeader.value = "faketree_begin+" + itemStructure.type + "+" + ID + "+" + "(" + itemStructure.dataCount + ")";
                            fakeHeader.infos = fakeSGSI;
                            fakeHeader.displayName = &itemStructure.name;
                            fakeHeader.isFake = true;
                            curTreeVector->push_back(fakeHeader);
                        }

                        //TODO probleme actuel : crash sur la fakebegintree "num_chars", que l'on veut supprimer au final, pour ne garder juste que la string, meme si size = 0
                        if (itemStructure.type == "char")
                        {
                            if (dataCount > 800000)
                            {
                                WarningLog("long string, continue ?? : " << path << "/" << itemStructure.name);
                                getchar();
                            }
                            if (createEmptyString)
                                dataCount = 0;

                            SGI.value.resize(valueSize*dataCount);
                            saveGameFile.read(&SGI.value[0],valueSize*dataCount);
                        }
                        else
                        {
                            SGI.value.resize(valueSize);
                            saveGameFile.read(&SGI.value[0],valueSize);
                        }

                        if (itemStructure.name.find("num_") != itemStructure.name.npos)
                        {
                            DebugLog("Found a _num : " << itemStructure.name << " value : " << *reinterpret_cast<int32_t*>(&(SGI.value[0])));
                            watchedValues[itemStructure.name] =  *reinterpret_cast<int32_t*>(&(SGI.value[0]));// reinterpret_cast<uint32_t>(SaveGameData[SaveGameData.size()-1].value, sizeof(SaveGameData[SaveGameData.size()-1].value));
                        }


                        if (itemStructure.name == "num_chars")// && *reinterpret_cast<int32_t*>(&(SGI.value[0])) > 0)
                            continue;

                        SGI.displayName = &itemStructure.name;
/*
                        if (dataCount > 1)
                            SGI.displayName = SGI.displayName;// new string( + UInt64ToString(curDataId));
*/
                        if (itemStructure.name == "global_variables" && curDataId < modInfos->int_str_globalVariables_Ids.size())
                            SGI.displayName = &modInfos->int_str_globalVariables_Ids[curDataId];

                        /*
                            isUnrecognizedSlot = 0,
                            isTroopSlot = 1,
                            isFactionSlot = 2,
                            isPartySlot = 3,
                            isQuestSlot = 4,
                            isItemSlot = 5,
                            isSceneSlot = 6,
                            isPartyTemplateSlot = 7,
                        */

                        if (slotType != isUnrecognizedSlot)
                        {
                            if (slotType == isTroopSlot && curDataId <= slotsInfos.troopSlotsMaxId)
                            {
                                map<unsigned int,Slot>::iterator itr = slotsInfos.troopSlots.find(curDataId);
                                if (itr != slotsInfos.troopSlots.end())
                                    SGI.displayName = &itr->second.name;// + UInt64ToString(curDataId);
                            }
                            else if (slotType == isFactionSlot && curDataId <= slotsInfos.factionSlotsMaxId)
                            {
                                map<unsigned int,Slot>::iterator itr = slotsInfos.factionSlots.find(curDataId);
                                if (itr != slotsInfos.factionSlots.end())
                                    SGI.displayName = &itr->second.name;// + UInt64ToString(curDataId);
                            }
                            else if (slotType == isPartySlot && curDataId <= slotsInfos.partySlotsMaxId)
                            {
                                map<unsigned int,Slot>::iterator itr = slotsInfos.partySlots.find(curDataId);
                                if (itr != slotsInfos.partySlots.end())
                                    SGI.displayName = &itr->second.name;// + UInt64ToString(curDataId);
                            }
                            else if (slotType == isQuestSlot && curDataId <= slotsInfos.questSlotsMaxId)
                            {
                                map<unsigned int,Slot>::iterator itr = slotsInfos.questSlots.find(curDataId);
                                if (itr != slotsInfos.questSlots.end())
                                    SGI.displayName = &itr->second.name;// + UInt64ToString(curDataId);
                            }
                            else if (slotType == isItemSlot && curDataId <= slotsInfos.itemSlotsMaxId)
                            {
                                map<unsigned int,Slot>::iterator itr = slotsInfos.itemSlots.find(curDataId);
                                if (itr != slotsInfos.itemSlots.end())
                                    SGI.displayName = &itr->second.name;// + UInt64ToString(curDataId);
                            }
                            else if (slotType == isSceneSlot && curDataId <= slotsInfos.sceneSlotsMaxId)
                            {
                                map<unsigned int,Slot>::iterator itr = slotsInfos.sceneSlots.find(curDataId);
                                if (itr != slotsInfos.sceneSlots.end())
                                    SGI.displayName = &itr->second.name;// + UInt64ToString(curDataId);
                            }
                            else if (slotType == isPartyTemplateSlot && curDataId <= slotsInfos.partyTemplateSlotsMaxId)
                            {
                                map<unsigned int,Slot>::iterator itr = slotsInfos.partyTemplateSlots.find(curDataId);
                                if (itr != slotsInfos.partyTemplateSlots.end())
                                    SGI.displayName = &itr->second.name;// + UInt64ToString(curDataId);
                            }
                        }

                        SGI.isFake = false;

                        curTreeVector->push_back(SGI);
                        inputsCount++;
                        numCreated++;

                        if (LogLevel == 3)
                            getchar();

                        if (dataCount == 1)
                        {
                            if (conditionalValueSave.find(path) != conditionalValueSave.end())
                            {
                                int address = conditionalValueSave[path].find(":");
                                string varNameAndCondition = conditionalValueSave[path];

                                string conditionResults = IsConditionTrue(varNameAndCondition,address+1);
                                if (atoi(conditionResults.c_str()) != 0)
                                {
                                    string newVarName = varNameAndCondition;
                                    newVarName.resize(address);
                                    CleanString(newVarName);
                                    if (itemStructure.type == "int32" || itemStructure.type == "uint32")
                                    {
                                        watchedValues[newVarName] = *reinterpret_cast<int32_t*>(&(SGI.value[0]));
                                        DebugLog(" added value : \"" << *reinterpret_cast<int32_t*>(&(SGI.value[0])) << "\" at watchedValueMap[] : \"" << newVarName << "\"");
                                    }
                                    else if (itemStructure.type == "int64" || itemStructure.type == "uint64")
                                    {
                                        watchedValues[newVarName] = *reinterpret_cast<int64_t*>(&(SGI.value[0]));
                                        DebugLog(" added value : \"" << *reinterpret_cast<int64_t*>(&(SGI.value[0])) << "\" at watchedValueMap[] : \"" << newVarName << "\"");
                                    }
                                    else
                                    {
                                        watchedValues[newVarName] = *reinterpret_cast<char*>(&(SGI.value[0]));
                                        DebugLog(" added value : \"" << *reinterpret_cast<char*>(&(SGI.value[0])) << "\" at watchedValueMap[] : \"" << newVarName << "\"");
                                    }
                                }
                            }

                            if (path == "game/party_records/party/stacks/troop_id" && ToInt64(IsConditionTrue("currentPartyFlags & playerPartyFlags",0)) != 0 )
                            {
                                playerTroopsIDs.push_back(*reinterpret_cast<int32_t*>(&(SGI.value[0])));
                            }
                            else if (path == "game/party_records/id")
                            {
                                watchedValues["curPartyId"] = *reinterpret_cast<int32_t*>(&(SGI.value[0]));
                            }
                            else if (path == "game/party_records/raw_id")
                            {
                                watchedValues["curPartyRawId"] = *reinterpret_cast<int32_t*>(&(SGI.value[0]));
                            }

                        }
                        else if (path.find("game/party_records") != path.npos)
                        {
                            if (path == "game/party_records/party/name/text")
                            {
                                modInfos->str_int_Party_Ids[SGI.value] = watchedValues["curPartyId"];
                                modInfos->int_str_Party_Ids[watchedValues["curPartyId"]] = SGI.value;

                                modInfos->str_int_PartyRecord_Ids[SGI.value] = watchedValues["curPartyRawId"];
                                modInfos->int_str_PartyRecord_Ids[watchedValues["curPartyRawId"]] = SGI.value;
                            }
                            else if (path == "game/party_records/party/id/text")
                            {
                                modInfos->int_str_Party_Id_Ids[watchedValues["curPartyRawId"]] = SGI.value;
                            }
                        }

/*
                        log << " " << (curDataId+1) << "/" << dataCount;
                        path = log.str();
*/
                        if (((float)(saveGameFile.tellg()) / (float)(fileLength)) * 100.0f > (float)(curPercentage))
                        {
                            progressBar->setValue(curPercentage);
                            progressBar->setLabelText(QString::fromStdString(path));
                            cout << curPercentage << "% : " << path << " time : " << (float)clock()/CLOCKS_PER_SEC << endl;
                            //cout << toLog << endl;
                            curPercentage++;
                        }

/*
                        if (path.find("party_records/party") != path.npos)
                        {
                            LogLevel = 3;
                        }
                        else
                        {
                            LogLevel = 2;
                        }*/

                        if (LogLevel > 2)
                        {
                            if (itemStructure.type == "int32")
                                DebugLog(itemStructure.type << ", " << itemStructure.name << " : " << *reinterpret_cast<int*>(&SGI.value[0]) << ", "  << "str : " << SGI.value << ", " << string_to_hex(SGI.value));
                            if (itemStructure.type == "int64")
                                DebugLog(itemStructure.type << ", " << itemStructure.name << " : " << *reinterpret_cast<int64_t*>(&SGI.value[0]) << ", "  << "str : " << SGI.value <<  ", "  << string_to_hex(SGI.value)  );
                            if (itemStructure.type == "uint32")
                                DebugLog(itemStructure.type << ", " << itemStructure.name << " : " << *reinterpret_cast<uint32_t*>(&SGI.value[0]) << ", "  << "str : " << SGI.value << ", "  << string_to_hex(SGI.value) );
                            if (itemStructure.type == "uint64")
                                DebugLog(itemStructure.type << ", " << itemStructure.name << " : " << *reinterpret_cast<uint64_t*>(&SGI.value[0]) << ", "  << "str : " << SGI.value <<  ", "  << string_to_hex(SGI.value)  );
                            if (itemStructure.type == "bool")
                                DebugLog(itemStructure.type << ", " << itemStructure.name << " : " << *reinterpret_cast<bool*>(&SGI.value[0]) << ", "  << "str : " << SGI.value <<  ", "  << string_to_hex(SGI.value) );
                            if (itemStructure.type == "float")
                                DebugLog(itemStructure.type << ", " << itemStructure.name << " : " << *reinterpret_cast<float*>(&SGI.value[0]) << ", "  << "str : " << SGI.value << ", " << string_to_hex(SGI.value) );
                            if (itemStructure.type == "char")
                                DebugLog(itemStructure.type << ", " << itemStructure.name << " : " << SGI.value << ", "  << "str : " << SGI.value << ", " << string_to_hex(SGI.value) );
                        }

                        if (itemStructure.type == "char")
                            break;
                        /*
                        if (LogLevel == 123)
                            getchar();*/

					}

                    if (!createdAFakeSGI && (numCreated == 0 || itemStructure.dataCount.find("num_") != itemStructure.dataCount.npos) && itemStructure.type != "char" && itemStructure.name != "num_chars")
                    {
                        createdAFakeSGI = true;
                        string ID = itemStructure.name;

                        SaveGameItem fakeHeader;

                        if (itemStructure.dataCount.find("num_") != itemStructure.dataCount.npos)
                            fakeHeader.value = "faketree_begin+" + itemStructure.type + "+" + ID + "+" + "(" + itemStructure.dataCount + ")";
                        else
                            fakeHeader.value = "faketree_begin+" + itemStructure.type + "+" + ID + "+" + "(" + Int64ToString(numCreated) + ")";

                        fakeHeader.infos = fakeSGSI;
                        fakeHeader.displayName = &itemStructure.name;
                        fakeHeader.isFake = true;
                        curTreeVector->push_back(fakeHeader);

                    }

                    if (createdAFakeSGI)
                    {
                        string ID = itemStructure.name;

                        SaveGameItem fakeHeader;
                        fakeHeader.value = "faketree_end";
                        fakeHeader.displayName = sumPointerToEmptyStr;
                        fakeHeader.infos = fakeSGSI;
                        fakeHeader.isFake = true;
                        curTreeVector->push_back(fakeHeader);
                    }

				}
				else if (structureBlocks.count(itemStructure.type))
				{
                    if (itemStructure.condition != "")
                    {
                        string conditionResults = IsConditionTrue(itemStructure.condition,0);
                        DebugLog( itemStructure.condition << " Evaluated to " << conditionResults);
                        if (atoi(conditionResults.c_str()) == 0)
                        {
                            DebugLog("refused");
                            continue;
                        }
                    }

                    SaveGameItem fakeBlockTitle;
                    fakeBlockTitle.value = "(" + UInt64ToString(dataCount) + ")";
                    fakeBlockTitle.infos = &itemStructure;
                    fakeBlockTitle.displayName = &itemStructure.name;
                    fakeBlockTitle.isFake = true;

                    //SaveGameItem* fakeBlockTitle = new SaveGameItem(itemStructure);


                    curTreeVector->push_back(fakeBlockTitle);

				    //currentBlockIdPath[currentBlockIdPath.size()-1] = GetCurrentVectorTree().size() - 1;
				    currentBlockIdPath.push_back(curTreeVector->size() - 1);
				    ReadAsBlock(itemStructure.type, itemStructure.name,dataCount);
                    currentBlockIdPath.pop_back();
				}
			}

            if (blockNumber > 1)
            {
                SaveGameItem fakeHeader;
                fakeHeader.value = "faketree_end";
                fakeHeader.infos = fakeSGSI;
                fakeHeader.displayName = sumPointerToEmptyStr;
                fakeHeader.isFake = true;
                curTreeVector->push_back(fakeHeader);
            }
		}

        if      (blockType == "faction")
            factionsIDInList.push_back(curTreeVector->size());
        else if (blockType == "site")
            scenesIDInList.push_back(curTreeVector->size());
        else if (blockType == "party_template")
            partyTemplateIDInList.push_back(curTreeVector->size());
        else if (blockType == "party_record")
            partyRecordsIDInList.push_back(curTreeVector->size());
        else if (blockType == "troop")
            troopsIDInList.push_back(curTreeVector->size());
        else if (blockType == "item_kind")
            itemsIDInList.push_back(curTreeVector->size());
        else if (blockType == "quest")
            questsIDInList.push_back(curTreeVector->size());

		currentBlockNamePath.pop_back();
}

/*
void SaveGame::StructureItemToSGItem(SaveGameItem& SGData, SaveGameStructureItem& SGSI)
{
    SGData.type = SGSI.type;
    SGData.dataCount = SGSI.dataCount;
    SGData.name = SGSI.name;
    SGData.condition = SGSI.condition;
    SGData.info = SGSI.info;
}*/

SaveGameItem* SaveGame::GetItemAtPath(vector<int>* aPath)
{

    List<SaveGameItem>* curVectorTree = &SaveGameData;
    vector<int> path = *aPath;

    for (unsigned int i=0; i < (path.size()-1); i++)
            curVectorTree = &(*curVectorTree)[path[i]].subItems;

    return &(*curVectorTree)[path[path.size()-1]];
}

SaveGameItem* SaveGame::GetItemAtPath(vector<unsigned int>* aPath)
{

    List<SaveGameItem>* curVectorTree = &SaveGameData;
    vector<unsigned int> path = *aPath;

    for (unsigned int i=0; i < (path.size()-1); i++)
            curVectorTree = &(*curVectorTree)[path[i]].subItems;

    return &(*curVectorTree)[path[path.size()-1]];
}

void SaveGame::InsertSGIAtMarker(SaveGameItem& SGI, vector<int>* aPath)
{

    List<SaveGameItem>* curVectorTree = &SaveGameData;
    vector<int> path = *aPath;

    for (unsigned int i=0; i < (path.size()-1); i++)
            curVectorTree = &(*curVectorTree)[path[i]].subItems;

    curVectorTree->insert(path[path.size()-1],SGI);
}

void SaveGame::InsertSGIAtMarker(List<SaveGameItem>& SGIs, vector<int>* aPath)
{

    List<SaveGameItem>* curVectorTree = &SaveGameData;
    vector<int> path = *aPath;

    for (unsigned int i=0; i < (path.size()-1); i++)
            curVectorTree = &(*curVectorTree)[path[i]].subItems;

    curVectorTree->insert(path[path.size()-1],SGIs.begin(),SGIs.end());
}

void SaveGame::RemoveSGIAtMarker(vector<int>* aPath)
{

    List<SaveGameItem>* curVectorTree = &SaveGameData;
    vector<int> path = *aPath;

    for (unsigned int i=0; i < (path.size()-1); i++)
            curVectorTree = &(*curVectorTree)[path[i]].subItems;

    curVectorTree->erase(path[path.size()-1]);
}

void SaveGame::RemoveSGIAtMarker(vector<int>* aPath, int count)
{

    List<SaveGameItem>* curVectorTree = &SaveGameData;
    vector<int> path = *aPath;

    for (unsigned int i=0; i < (path.size()-1); i++)
            curVectorTree = &(*curVectorTree)[path[i]].subItems;

    curVectorTree->erase(path[path.size()-1], path[path.size()-1] + count);
}

List<SaveGameItem>* SaveGame::GetCurrentVectorTree()
{

    List<SaveGameItem>* curVectorTree = &SaveGameData;

    for (unsigned int i=0; i < currentBlockIdPath.size(); i++)
            curVectorTree = &(*curVectorTree)[currentBlockIdPath[i]].subItems;

    return curVectorTree;
}

string SaveGame::IsConditionTrue(string condition, uint32_t start)
{
    vector<string> values;
    int bracketsIgnoreLevel = 0; // delegated
    string curValue = "";
    string buffer = "                ";


    for (uint32_t i=start; i<condition.size(); i++)
    {
        if (condition[i] == ')')
        {
            bracketsIgnoreLevel -= 1;
            if (bracketsIgnoreLevel < 0)
                break;
            continue;
        }

        if (condition[i] == '(')
        {
            if (bracketsIgnoreLevel == 0)
            {
                if (curValue.size() > 0)
                {
                    if (curValue != "")
                        values.push_back(Evaluate(curValue));
                    curValue = "";
                }
                values.push_back(IsConditionTrue(condition, i+1));
            }
            bracketsIgnoreLevel += 1;
        }

        if (bracketsIgnoreLevel > 0)
            continue;


        if (condition[i] == ' ')
        {
            if (curValue != "")
                values.push_back(Evaluate(curValue));

            curValue = "";
            continue;
        }

        curValue.push_back(condition[i]);
    }

    if (curValue.size() > 0)
        values.push_back(Evaluate(curValue));


    bool hasUnaryOperator = false;
    do {
        hasUnaryOperator = false;
        for (unsigned int i=0; i<values.size(); i++)
        {
            buffer = "";
            if (values[i] == "+")
            {
                buffer = Int64ToString( ToInt64(values[i-1]) + ToInt64(values[i+1]));
                values[i-1].assign(Evaluate(buffer));
                values.erase(values.begin()+i+1);
                values.erase(values.begin()+i);
                hasUnaryOperator = true;
                break;
            }
            if (values[i] == "-")
            {
                buffer = Int64ToString( ToInt64(values[i-1]) - ToInt64(values[i+1]));
                values[i-1].assign(Evaluate(buffer));
                values.erase(values.begin()+i+1);
                values.erase(values.begin()+i);
                hasUnaryOperator = true;
                break;
            }
            if (values[i] == "/")
            {
                buffer = Int64ToString( ToInt64(values[i-1]) / ToInt64(values[i+1]));
                values[i-1].assign(Evaluate(buffer));
                values.erase(values.begin()+i+1);
                values.erase(values.begin()+i);
                hasUnaryOperator = true;
                break;
            }
            if (values[i] == "*")
            {
                buffer = Int64ToString( ToInt64(values[i-1]) * ToInt64(values[i+1]));
                values[i-1].assign(Evaluate(buffer));
                values.erase(values.begin()+i+1);
                values.erase(values.begin()+i);
                hasUnaryOperator = true;
                break;
            }
            if (values[i] == "&")
            {
                DebugLog(values[i-1] << " & " << values[i+1] << " : " << ( ToInt64(values[i-1]) & ToInt64(values[i+1])));
                buffer = Int64ToString( ToInt64(values[i-1]) & ToInt64(values[i+1]));
                values[i-1].assign(Evaluate(buffer));
                values.erase(values.begin()+i+1);
                values.erase(values.begin()+i);
                hasUnaryOperator = true;
                break;
            }
            if (values[i] == "|")
            {
                buffer = Int64ToString( ToInt64(values[i-1]) | ToInt64(values[i+1]));
                values[i-1].assign(Evaluate(buffer));
                values.erase(values.begin()+i+1);
                values.erase(values.begin()+i);
                hasUnaryOperator = true;
                break;
            }
        }
    } while (hasUnaryOperator);


    bool hasComparisonOperator = false;
    do {
        hasComparisonOperator = false;
        for (unsigned int i=0; i<values.size(); i++)
        {
            buffer = "";
            if (values[i] == "<")
            {
                buffer = Int64ToString( ToInt64(values[i-1]) < ToInt64(values[i+1]));
                values[i-1].assign(Evaluate(buffer));
                values.erase(values.begin()+i+1);
                values.erase(values.begin()+i);
                hasComparisonOperator = true;
                break;
            }
            if (values[i] == ">")
            {
                buffer = Int64ToString( ToInt64(values[i-1]) > ToInt64(values[i+1]));
                values[i-1].assign(Evaluate(buffer));
                values.erase(values.begin()+i+1);
                values.erase(values.begin()+i);
                hasComparisonOperator = true;
                break;
            }
            if (values[i] == "<=")
            {
                buffer = Int64ToString( ToInt64(values[i-1]) <= ToInt64(values[i+1]));
                values[i-1].assign(Evaluate(buffer));
                values.erase(values.begin()+i+1);
                values.erase(values.begin()+i);
                hasComparisonOperator = true;
                break;
            }
            if (values[i] == ">=")
            {
                buffer = Int64ToString( ToInt64(values[i-1]) >= ToInt64(values[i+1]));
                values[i-1].assign(Evaluate(buffer));
                values.erase(values.begin()+i+1);
                values.erase(values.begin()+i);
                hasComparisonOperator = true;
                break;
            }
            if (values[i] == "!")
            {
                buffer = Int64ToString(ToInt64(values[i+1]) > 0 ? 0 : 1);
                values[i].assign(Evaluate(buffer));
                values.erase(values.begin()+i+1);
                hasComparisonOperator = true;
                break;
            }
        }
    } while (hasComparisonOperator);


    bool hasBooleanEqualOperator = false;
    do {
        hasBooleanEqualOperator = false;
        for (unsigned int i=0; i<values.size(); i++)
        {
            buffer = "";
            if (values[i] == "==")
            {
                DebugLog(values[i-1] << " == " << values[i+1] << " : " << ( ToInt64(values[i-1]) == ToInt64(values[i+1])));
                buffer = Int64ToString( ToInt64(values[i-1]) == ToInt64(values[i+1]));
                values[i-1].assign(Evaluate(buffer));
                values.erase(values.begin()+i+1);
                values.erase(values.begin()+i);
                hasBooleanEqualOperator = true;
                break;
            }
        }
    } while (hasBooleanEqualOperator);

    bool hasBooleanOperator = false;
    do {
        hasBooleanOperator = false;
        for (unsigned int i=0; i<values.size(); i++)
        {
            buffer = "";
            if (values[i] == "!=")
            {
                buffer = Int64ToString( ToInt64(values[i-1]) != ToInt64(values[i+1]));
                values[i-1].assign(Evaluate(buffer));
                values.erase(values.begin()+i+1);
                values.erase(values.begin()+i);
                hasBooleanOperator = true;
                break;
            }
            if (values[i] == "||")
            {
                DebugLog(values[i-1] << " || " << values[i+1] << " : " << ( ToInt64(values[i-1]) || ToInt64(values[i+1])));
                buffer = Int64ToString( ToInt64(values[i-1]) || ToInt64(values[i+1]));
                values[i-1].assign(Evaluate(buffer));
                values.erase(values.begin()+i+1);
                values.erase(values.begin()+i);
                hasBooleanOperator = true;
                break;
            }
            if (values[i] == "&&")
            {
                DebugLog(values[i-1] << " && " << values[i+1] << " : " << ( ToInt64(values[i-1]) && ToInt64(values[i+1])));
                buffer = Int64ToString( ToInt64(values[i-1]) && ToInt64(values[i+1]));
                values[i-1].assign(Evaluate(buffer));
                values.erase(values.begin()+i+1);
                values.erase(values.begin()+i);
                hasBooleanOperator = true;
                break;
            }
        }
    } while (hasBooleanOperator);

    if (values.size() > 1)
    {
        ostringstream log;
        log << "Can't evaluate completely : ";
        for (unsigned int i=0; i < values.size(); i++)
        {
            log << values[i] << " ";
        }
        ErrorLog(log.str());
    }


    return values[0];
}


string SaveGame::Evaluate(string& value)
{
///////////////////////////////////////////////////////////////////REMOVE SPACES chars
    CleanString(value);

///////////////////////////////////////////////////////////////////TRUE / FALSE
    if (value == "true")
        return "1";
    if (value == "false")
        return "0";
    if (value.size() == 0)
        return "0";


///////////////////////////////////////////////////////////////////IS DIGIT
    bool isDigit = true;
    for (unsigned int i=0; i< value.size(); i++)
        if (!isdigit(value[i]))
            isDigit = false;

    if (isDigit)
    {
        return value;
    }

///////////////////////////////////////////////////////////////////OPERATOR
    if (value == "==" ||
        value == "!=" ||
        value == "&&" ||
        value == "||" ||
        value == "<" ||
        value == ">" ||
        value == ">=" ||
        value == "<=" ||
        value == "+" ||
        value == "-" ||
        value == "*" ||
        value == "/" ||
        value == "|" ||
        value == "&" ||
        value == "!"  )
    {
        return value;
    }


///////////////////////////////////////////////////////////////////VARIABLES

///////////////////////////////////////////////////////////////////VARIOUS GLOBAL VARIABLE

    if (watchedValues.find(value) != watchedValues.end())
    {
        return Int64ToString(watchedValues[value]);
    }

///////////////////////////////////////////////////////////////////GAMESAVE PATH VARIABLE


    bool hasDot = false;
    for (unsigned int i=0; i< value.size(); i++)
        if (value[i] == '.')
            hasDot = true;

    if (hasDot)
    {
        return LookForSGVariable(value);
    }

    return LookForLocalSGVariable(value);

}

string SaveGame::LookForSGVariable(string& address)
{
    vector<string> addressList;
    string buffer = "";

    for (unsigned int i=0; i< address.size(); i++)
    {
        if (address[i] == '.')
        {
            addressList.push_back(buffer);
            buffer = "";
            continue;
        }
        buffer.push_back(address[i]);
    }
    if (buffer.size() > 0)
        addressList.push_back(buffer);

    List<SaveGameItem>* curVectorTree = &SaveGameData;


    for (unsigned int i=0; i < addressList.size(); i++)
    {
        //DebugLog("n" << i << " : " << addressList[i] << " length : " << curVectorTree->size());
        for (unsigned int j=0; j < curVectorTree->size(); j++)
        {
            //DebugLog("compare : " << addressList[i] << " to " << (*curVectorTree)[j].infos->name);
            if (addressList[i] == (*curVectorTree)[j].infos->name)
            {
                //DebugLog("FOUND : " << addressList[i] << " to " << (*curVectorTree)[j].infos->name);

                if (i == (addressList.size()-1))
                {
                    if ((*curVectorTree)[j].infos->type == "uint32")
                    {
                        DebugLog( "Found Path Variable uint32 " << address << " with value : " << UInt64ToString(*reinterpret_cast<uint32_t*>(&(*curVectorTree)[j].value[0])))
                        return UInt64ToString(*reinterpret_cast<uint32_t*>(&(*curVectorTree)[j].value[0]));
                    }
                    if ((*curVectorTree)[j].infos->type == "uint64")
                    {
                        DebugLog( "Found Path Variable uint64 " << address << " with value : " << UInt64ToString(*reinterpret_cast<uint64_t*>(&(*curVectorTree)[j].value[0])))
                        return UInt64ToString(*reinterpret_cast<uint64_t*>(&(*curVectorTree)[j].value[0]));
                    }

                    if ((*curVectorTree)[j].infos->type == "int32")
                    {
                        DebugLog( "Found Path Variable int32 " << address << " with value : " << Int64ToString(*reinterpret_cast<int32_t*>(&(*curVectorTree)[j].value[0])))
                        return Int64ToString(*reinterpret_cast<int32_t*>(&(*curVectorTree)[j].value[0]));
                    }
                    if ((*curVectorTree)[j].infos->type == "int64")
                    {
                        DebugLog( "Found Path Variable int64 " << address << " with value : " << Int64ToString(*reinterpret_cast<int64_t*>(&(*curVectorTree)[j].value[0])))
                        return Int64ToString(*reinterpret_cast<int64_t*>(&(*curVectorTree)[j].value[0]));
                    }
                    if ((*curVectorTree)[j].infos->type == "bool")
                    {
                        DebugLog( "Found Path Variable int64 " << address << " with value : " << Int64ToString(*reinterpret_cast<char*>(&(*curVectorTree)[j].value[0])))
                        return Int64ToString(*reinterpret_cast<char*>(&(*curVectorTree)[j].value[0]));
                    }
                    if ((*curVectorTree)[j].infos->type == "char")
                    {
                        DebugLog( "Found Path Variable int64 " << address << " with value : " << (*curVectorTree)[j].value);
                        return (*curVectorTree)[j].value;
                    }
                }

                curVectorTree = &(*curVectorTree)[j].subItems;
                break;
            }
        }
    }

    ErrorLog("Could not find variable at path : " << address << ", lets assume it's 1");
    return "1";
}


string SaveGame::LookForLocalSGVariable(string& variable)
{
    List<SaveGameItem>* curVectorTree = GetCurrentVectorTree();
    string results = "";
    for (unsigned int j=0; j < curVectorTree->size(); j++)
    {
        if (variable == (*curVectorTree)[j].infos->name)
        {
            if ((*curVectorTree)[j].infos->type == "uint32")
            {
                results = UInt64ToString(*reinterpret_cast<uint32_t*>(&(*curVectorTree)[j].value[0]));
            }
            if ((*curVectorTree)[j].infos->type == "uint64")
            {
                results = UInt64ToString(*reinterpret_cast<uint64_t*>(&(*curVectorTree)[j].value[0]));
            }

            if ((*curVectorTree)[j].infos->type == "int32")
            {
                results = Int64ToString(*reinterpret_cast<int32_t*>(&(*curVectorTree)[j].value[0]));
            }
            if ((*curVectorTree)[j].infos->type == "int64")
            {
                results = Int64ToString(*reinterpret_cast<int64_t*>(&(*curVectorTree)[j].value[0]));
            }
            if ((*curVectorTree)[j].infos->type == "bool")
            {
                results = Int64ToString(*reinterpret_cast<char*>(&(*curVectorTree)[j].value[0]));
            }
        }
    }

   // cout << " lookup : " << variable << endl;

    if (results != "")
    {
        DebugLog( "Found Local Variable " << variable << " with value : " << results)
        return results;
    }
    ErrorLog("Could not find variable : " << variable << ", lets assume it's 1");
    return "1";
}


bool SaveGame::ExportData(string filePath)
{
    curPercentage = 0;
    saveGameExportFile.open(filePath.c_str(), ofstream::out | ofstream::trunc | ofstream::binary);
    if (saveGameExportFile)
        ExportTree(SaveGameData);
    else
        return false;

    cout << "import file size : " << saveGameFile.tellg() << " export size : " << saveGameExportFile.tellp() << endl;


    saveGameExportFile.close();
    return true;
}

void SaveGame::ExportTree(List<SaveGameItem>& curTree)
{
    if (((float)(saveGameExportFile.tellp()) / (float)(fileLength)) * 100.0f > (float)(curPercentage))
    {
        progressBar->setValue(curPercentage);
        progressBar->setLabelText(QString::fromStdString("Exporting..."));
        progressBar->show();
        curPercentage++;
        QCoreApplication::processEvents();
    }

    for (unsigned int i=0; i<curTree.size();i++)
    {
        if (curTree[i].isFake == false)
        {

            if (curTree[i].infos->name == "text")
            {
                int32_t out = curTree[i].value.size();
                string push;
                push.resize(typeSize["int32"]);
                memcpy( &push[ 0 ], &out, typeSize["int32"]);
                DataStringToOStream(saveGameExportFile,push);
            }
            DataStringToOStream(saveGameExportFile,curTree[i].value);
        }
        if (curTree[i].subItems.size() > 0)
            ExportTree(curTree[i].subItems);
    }
}

void SaveGame::ConvertFrom(SaveGameInfos* originalMod, UserSettings Settings)
{
    LogLevel = 2;
    userSettings = Settings;
    originalSaveInfos = originalMod;
    vector<int> marker;
    cout << "converting ..." << endl;
    marker.push_back(-1);
    curInputs = 0;
    ConvertItemList(originalMod->savegame->SaveGameData,marker);
}

void SaveGame::ConvertItemList(List<SaveGameItem>& curTree,vector<int>& marker, int mergeBehaviour)
{
    vector<unsigned int>* originalIDList;
    bool specialBrowsing = false;




    if (mergeBehaviour == DONTREPLACE)
    {
        return;
    }

    if (mergeBehaviour == REPLACE_BY_FACTION_ID)
    {
        originalIDList = &originalSaveInfos->savegame->factionsIDInList;
        curIDList = &factionsIDInList;
        specialBrowsing = true;
        browsingFactions = true;
    }
    else if (mergeBehaviour == REPLACE_BY_PARTY_RECORD_ID)
    {
        originalIDList = &originalSaveInfos->savegame->partyRecordsIDInList;
        curIDList = &partyRecordsIDInList;
        specialBrowsing = true;
        browsingParty = true;
    }
    else if (mergeBehaviour == REPLACE_BY_PARTY_TEMPLATE_ID)
    {
        originalIDList = &originalSaveInfos->savegame->partyTemplateIDInList;
        curIDList = &partyTemplateIDInList;
        specialBrowsing = true;
        browsingParty_Template = true;
    }
    else if (mergeBehaviour == REPLACE_BY_QUEST_ID)
    {
        originalIDList = &originalSaveInfos->savegame->questsIDInList;
        curIDList = &questsIDInList;
        specialBrowsing = true;
        browsingQuests = true;
    }
    else if (mergeBehaviour == REPLACE_BY_SCENE_ID)
    {
        originalIDList = &originalSaveInfos->savegame->scenesIDInList;
        curIDList = &scenesIDInList;
        specialBrowsing = true;
        browsingSite = true;
    }
    else if (mergeBehaviour == REPLACE_BY_TROOP_ID)
    {
        originalIDList = &originalSaveInfos->savegame->troopsIDInList;
        curIDList = &troopsIDInList;
        specialBrowsing = true;
        browsingTroops = true;
    }
    else if (mergeBehaviour == REPLACE_BY_ITEM_ID)
    {
        originalIDList = &originalSaveInfos->savegame->itemsIDInList;
        curIDList = &itemsIDInList;
        specialBrowsing = true;
        browsingItem = true;
    }
    else
    {
        originalIDList = new vector<unsigned int>();
        originalIDList->push_back(0);
        originalIDList->push_back(curTree.size());
        specialBrowsing = false;
    }

    //cout << "newBlock ..." << curTree.size() << endl;
    for (unsigned int k=0; k<(originalIDList->size()-1);k++)
    {
        int64_t newId = 0;
        if (mergeBehaviour == REPLACE_BY_FACTION_ID)
        {
            browsingPlayers = false;
            if (originalSaveInfos->modInfos->int_str_Faction_Ids[k] == "fac_player_faction" ||
                originalSaveInfos->modInfos->int_str_Faction_Ids[k] == "fac_player_supporters_faction")
                browsingPlayers = true;

            newId = originalSaveInfos->modInfos->GetOthersFactionId(k,modInfos);
            if (newId != k)
                cout << "Faction " << k << " is now " << (newId == -666 ? "missing" : Int64ToString(newId)) << endl;
            if (newId == -666)
                continue;
        }
        else if (mergeBehaviour == REPLACE_BY_PARTY_RECORD_ID)
        {
            browsingPlayers = false;
            browsingLords = false;
            browsingTowns = false;

            if (k == 0)
            {
                browsingPlayers = true;
            }
            else if (originalSaveInfos->modInfos->int_str_Party_Id_Ids[k] == "pt_kingdom_hero_party")
            {
                browsingLords = true;
            }
            else if (originalSaveInfos->modInfos->int_str_Party_Id_Ids[k].find("p_town") != originalSaveInfos->modInfos->int_str_Party_Id_Ids[k].npos ||
                originalSaveInfos->modInfos->int_str_Party_Id_Ids[k].find("p_castle") != originalSaveInfos->modInfos->int_str_Party_Id_Ids[k].npos ||
                originalSaveInfos->modInfos->int_str_Party_Id_Ids[k].find("p_village") != originalSaveInfos->modInfos->int_str_Party_Id_Ids[k].npos)
            {
                browsingTowns = true;
            }
            else //If random party (most likely non-Unique) -> ignore
            {
                continue;
            }

            if (browsingPlayers && !userSettings.replace_party_player)
                continue;
            if (browsingLords && !userSettings.replace_party_lords)
                continue;
            if (browsingTowns && !userSettings.replace_party_towns)
                continue;

            newId = originalSaveInfos->modInfos->GetOthersPartyRecordId(k,modInfos);
            if (newId != k)
                cout << "Party " << k << " is now " << (newId == -666 ? "missing" : Int64ToString(newId)) << endl;
            if (newId == -666)
                continue;
        }
        else if (mergeBehaviour == REPLACE_BY_PARTY_TEMPLATE_ID)
        {
            browsingPlayers = false;
            newId = originalSaveInfos->modInfos->GetOthersPartyTemplateId(k,modInfos);
            if (newId != k)
                cout << "Party_Template " << k << " is now " << (newId == -666 ? "missing" : Int64ToString(newId)) << endl;
            if (newId == -666)
                continue;
        }
        else if (mergeBehaviour == REPLACE_BY_QUEST_ID)
        {
            browsingPlayers = false;

            newId = originalSaveInfos->modInfos->GetOthersQuestId(k,modInfos);
            if (newId != k)
                cout << "Quest " << k << " is now " << (newId == -666 ? "missing" : Int64ToString(newId)) << endl;
            if (newId == -666)
                continue;
        }
        else if (mergeBehaviour == REPLACE_BY_SCENE_ID)
        {
            browsingPlayers = false;

            newId = originalSaveInfos->modInfos->GetOthersSceneId(k,modInfos);
            if (newId != k)
                cout << "Site " << k << " is now " << (newId == -666 ? "missing" : Int64ToString(newId)) << endl;
            if (newId == -666)
                continue;
        }
        else if (mergeBehaviour == REPLACE_BY_TROOP_ID)
        {
            browsingPlayers = false;
            browsingLords = false;
            browsingLadies = false;
            browsingHeroes = false;
            browsingNonHeroes = false;

            if (k == 0)
            {
                browsingPlayers = true;
            }
            else if (originalSaveInfos->modInfos->troops[k].infos["IDName"].find("trp_kingdom") != originalSaveInfos->modInfos->troops[k].infos["IDName"].npos ||
                     originalSaveInfos->modInfos->troops[k].infos["IDName"].find("trp_knight") != originalSaveInfos->modInfos->troops[k].infos["IDName"].npos)
            {
                browsingLords = true;
            }
            else if (originalSaveInfos->modInfos->troops[k].infos["IDName"].find("_lady_") != originalSaveInfos->modInfos->troops[k].infos["IDName"].npos)
            {
                browsingLadies = true;
            }
            else if (originalSaveInfos->modInfos->troops[k].infos["IDName"].find("trp_npc") != originalSaveInfos->modInfos->troops[k].infos["IDName"].npos)
            {
                browsingHeroes = true;
            }
            else
            {
                browsingNonHeroes = true;
            }

            if (browsingPlayers && !userSettings.replace_troop_player)
                continue;
            if (browsingLords && !userSettings.replace_troop_lords)
                continue;
            if (browsingLadies && !userSettings.replace_troop_ladies)
                continue;
            if (browsingHeroes && !userSettings.replace_troop_heroes)
                continue;
            if (browsingNonHeroes && !userSettings.replace_troop_nonheroes)
                continue;

            newId = originalSaveInfos->modInfos->GetOthersTroopId(k,modInfos);
            if (newId != k)
                cout << "Troop " << k << ":" << originalSaveInfos->modInfos->troops[k].infos["IDName"] <<  " is now " << (newId == -666 ? "missing" : Int64ToString(newId)) << endl;
            if (newId == -666)
                continue;
        }
        else if (mergeBehaviour == REPLACE_BY_ITEM_ID)
        {
            browsingPlayers = false;

            newId = originalSaveInfos->modInfos->GetOthersItemId(k,modInfos);
            if (newId != k)
                cout << "Item " << k << " is now " << (newId == -666 ? "missing" : Int64ToString(newId)) << endl;
            if (newId == -666)
                continue;
        }
        else
        {
            newId = 0;
        }

        if (specialBrowsing)
        {
            if (newId < (curIDList->size()-1))
                marker[marker.size()-1] = (*curIDList)[newId]-1;
            else
                continue;
        }

        if (specialBrowsing && LogLevel > 2)
        {
            DebugLog("curIDList size : " << curIDList->size() << " with firsts : " << (*curIDList)[0] << "," << (*curIDList)[1] << "," << (*curIDList)[2] << "," << (*curIDList)[3] << "," << (*curIDList)[4] << ".");
            DebugLog("special browsing : from " << (*originalIDList)[k] << " to " << (*originalIDList)[k+1] << " with marker at " << marker[marker.size()-1] << "/" << endl);
            vector<int> path = marker;
            path[path.size()-1] += 1;

            DebugLog("original save block name is : " << curTree[(*originalIDList)[k]].value);
            DebugLog("new save block name is : " << GetItemAtPath(&path)->value);
        }
        for (unsigned int i=(*originalIDList)[k]; i < (*originalIDList)[k+1];i++)
        {

            if (progressBar->value() < ((curInputs*100.0) / inputsCount))
            {
                progressBar->setValue((curInputs*100.0) / inputsCount);
                progressBar->show();
                QCoreApplication::processEvents();
            }
            if (curTree[i].isFake)
            {
                i = HandleFakeItem(curTree, i, marker, mergeBehaviour);
            }
            else
            {
                curInputs++;
                int nextSGIId = GetNextSGI(curTree[i].infos->name,marker);

                if (nextSGIId != -1)
                {
                    marker[marker.size()-1] = nextSGIId;
                    SaveGameItem* newSGI = GetItemAtPath(&marker);

                    MergeSGI(&(curTree[i]),newSGI,CombineBehaviours(mergeBehaviour,newSGI->infos->mergeBehaviour));

                    if (newSGI->infos->name.find("num_") != newSGI->infos->name.npos)
                        watchedValues[newSGI->infos->name] =  *reinterpret_cast<int32_t*>(&(newSGI->value[0]));
                    if (curTree[i].infos->name.find("num_") != curTree[i].infos->name.npos)
                        originalSaveInfos->savegame->watchedValues[curTree[i].infos->name] =  *reinterpret_cast<int32_t*>(&(curTree[i].value[0]));
                }
                else
                {
                    //WarningLog("A input is missing in new save : " << curTree[i].displayName);
                }
            }

        }
    }



    if (!specialBrowsing)
    {
        delete(originalIDList);
    }
    else
    {
        browsingPlayers = false;
        browsingTowns = false;
        browsingLords = false;
        browsingLadies = false;
        browsingHeroes = false;
        browsingNonHeroes = false;

        if (mergeBehaviour == REPLACE_BY_FACTION_ID)
        {
            browsingFactions = false;
        }
        else if (mergeBehaviour == REPLACE_BY_PARTY_RECORD_ID)
        {
            browsingParty = false;
        }
        else if (mergeBehaviour == REPLACE_BY_PARTY_TEMPLATE_ID)
        {
            browsingParty_Template = false;
        }
        else if (mergeBehaviour == REPLACE_BY_QUEST_ID)
        {
            browsingQuests = false;
        }
        else if (mergeBehaviour == REPLACE_BY_SCENE_ID)
        {
            browsingSite = false;
        }
        else if (mergeBehaviour == REPLACE_BY_TROOP_ID)
        {
            browsingTroops = false;
        }
        else if (mergeBehaviour == REPLACE_BY_ITEM_ID)
        {
            browsingItem = false;
        }

    }
}

void SaveGame::ShiftCurIdList(unsigned int position, int shiftAmount)
{
    if (shiftAmount < 0)
        position += shiftAmount;

    for (unsigned int i=0; i < curIDList->size(); i++)
    {
        if ((*curIDList)[i] >= position)
            (*curIDList)[i] += shiftAmount;
    }
}

int SaveGame::CombineBehaviours(int behaveParent, int behaveCurrent)
{
    if (behaveParent == CLEAR || behaveCurrent == CLEAR)
        return CLEAR;
    if (behaveParent == DONTREPLACE || behaveCurrent == DONTREPLACE)
        return DONTREPLACE;

    if (behaveParent == DONTREPLACE_EXCEPTPLAYERS && !browsingPlayers)
        return DONTREPLACE;

    if (behaveParent == REPLACE_EXCEPTPLAYERS && browsingPlayers)
        return DONTREPLACE;

    return behaveCurrent;
}

int SaveGame::GetNextSGI(string& name, vector<int>& marker)
{
    List<SaveGameItem>* curVectorTree = &SaveGameData;

    for (unsigned int i=0; i < (marker.size()-1); i++)
            curVectorTree = &(*curVectorTree)[marker[i]].subItems;

    for (unsigned int i=(marker[marker.size()-1]+1); i< curVectorTree->size(); i++)
    {
        if ((*curVectorTree)[i].infos->name == name)
        {
            return i;
        }
        else if ((*curVectorTree)[i].infos->condition.size() < 2 && !(*curVectorTree)[i].isFake)
        {
            //WarningLog("A input is missing in original save : " << (*curVectorTree)[i].displayName);
            break;
        }
    }

    return -1;
}


int SaveGame::GetNextBeginSGI(string& name, vector<int>& marker)
{

    List<SaveGameItem>* curVectorTree = &SaveGameData;

    for (unsigned int i=0; i < (marker.size()-1); i++)
            curVectorTree = &(*curVectorTree)[marker[i]].subItems;

    for (unsigned int i=(marker[marker.size()-1]+1); i< curVectorTree->size(); i++)
    {
        if (*(*curVectorTree)[i].displayName == name)
        {
            return i;
        }
        else if ((*curVectorTree)[i].infos->condition.size() < 2 && !(*curVectorTree)[i].isFake)
        {
            //WarningLog("A input is missing in original save : " << (*curVectorTree)[i].displayName);
            break;
        }
    }

    return -1;
}


unsigned int SaveGame::HandleFakeItem(List<SaveGameItem>& curTree, unsigned int id, vector<int>& marker, int mergeBehaviour)
{
    string val = curTree[id].value;

    if (val.find("faketree_end") != val.npos)
    {
        marker[marker.size()-1] += 1;
        return id;
    }
    else if (val.find("faketree_begin") != val.npos)
    {
        int nextSGIId = GetNextBeginSGI(*curTree[id].displayName,marker);
        bool hasPushed = false;
        unsigned int position = 0;
        int shift = 0;

        vector<string> originalValues = GetFakeSGIBeginData(val);
        //unsigned int originalCount = atoi(&originalSaveInfos->savegame->Evaluate(originalValues[2])[0]);
        unsigned int newCount = atoi(&Evaluate(originalValues[2])[0]);

        if (originalValues[2].size() == 0)
        {
            DebugLog(" tree is a block sample : " << curTree[id].value);
            marker[marker.size()-1] += 1;
            return id;
        }
        else                //this is the begin of type list
        {
            int settingsBehave = REPLACE;
            int slotType = -1;

            if (originalValues[1] == "slots")
            {
                slotType = isUnrecognizedSlot;

                if (browsingFactions)
                    slotType = isFactionSlot;
                if (browsingTroops)
                    slotType = isTroopSlot;
                if (browsingParty)
                    slotType = isPartySlot;
                if (browsingQuests)
                    slotType = isQuestSlot;
                if (browsingSite)
                    slotType = isSceneSlot;
                if (browsingParty_Template)
                    slotType = isPartyTemplateSlot;

                if (!userSettings.replace_faction_slots && browsingFactions)
                {
                    settingsBehave = DONTREPLACE;
                }
                if ((!userSettings.replace_party_player_slots && browsingParty && browsingPlayers) ||
                    (!userSettings.replace_party_lords_slots && browsingParty && browsingLords) ||
                    (!userSettings.replace_party_towns_slots && browsingParty && browsingTowns))
                {
                    settingsBehave = DONTREPLACE;
                }
                if (!userSettings.replace_quest_slots && browsingQuests)
                {
                    settingsBehave = DONTREPLACE;
                }
                if (!userSettings.replace_party_template_slots && browsingParty_Template)
                {
                    settingsBehave = DONTREPLACE;
                }
                if ((!userSettings.replace_troop_player_slots && browsingPlayers && browsingTroops) ||
                    (!userSettings.replace_troop_lord_slots && browsingLords && browsingTroops) ||
                    (!userSettings.replace_troop_lady_slots && browsingLadies && browsingTroops) ||
                    (!userSettings.replace_troop_heroes_slots && browsingHeroes && browsingTroops) ||
                    (!userSettings.replace_troop_nonheroes_slots && browsingNonHeroes && browsingTroops))
                {
                    settingsBehave = DONTREPLACE;
                }

            }
            else if (originalValues[1] == "relations")
            {
                if (!userSettings.replace_faction_relation && browsingFactions)
                {
                    settingsBehave = DONTREPLACE;
                }
            }

            DebugLog( " tree is a list : " << curTree[id].value);
            if (nextSGIId == -1)    //if no fake tree begin : create one
            {
                DebugLog(" no faketre_begin in newSave creating one ..." );
                marker[marker.size()-1] += 1;
                InsertSGIAtMarker(curTree[id],&marker);
                GetItemAtPath(&marker)->value = "faketree_end";
                InsertSGIAtMarker(curTree[id],&marker);
                if (!hasPushed)
                {
                    position = marker[marker.size()-1];
                    hasPushed = true;
                }
                shift += 2;
            }
            else
            {
                marker[marker.size()-1] = nextSGIId;
            }
            DebugLog( " count is : " << newCount);

            List<SaveGameItem> SGIToAdd;
            SaveGameItem* curSGI;

            curInputs += newCount;
            for (unsigned int i=0; i < newCount; i++)
            {
                if (curTree[id+i+1].infos->type != originalValues[0] || CombineBehaviours(settingsBehave,curTree[id+i+1].infos->mergeBehaviour) == DONTREPLACE)
                {
                    int faketreeend = id + 1;
                    while (curTree[faketreeend].value.find("faketree_end") == curTree[faketreeend].value.npos)
                        faketreeend++;

                    marker[marker.size()-1] += newCount + 1 - i;
                    return faketreeend; //original count depleted or behav == DONTREPLACE
                }

                nextSGIId = GetNextSGI(originalValues[1],marker);

                if (nextSGIId == -1)    //if no SGI of this type, then we create one
                {
                    SGIToAdd.push_back(curTree[id+i+1]);
                    curSGI = &(SGIToAdd[SGIToAdd.size()-1]);

                    if (curSGI->subItems.size() > 0)
                        ErrorLog("SGI with childs is copied.");

                    if (!hasPushed)
                    {
                        position = marker[marker.size()-1];
                        hasPushed = true;
                    }
                    shift++;
                }
                else
                {
                    marker[marker.size()-1] = nextSGIId;
                    curSGI = GetItemAtPath(&marker);
                }

                if (slotType > 0)
                {
                    MergeSGI(&(curTree[id+i+1]),curSGI,GetSlotBehave(i,slotType));
                }
                else
                {
                    MergeSGI(&(curTree[id+i+1]),curSGI,curTree[id+i+1].infos->mergeBehaviour);
                }

            }



            marker[marker.size()-1] += 1; //to faketree_end
            DebugLog( " ended 1 : " << newCount << " size to inser "  << SGIToAdd.size());
            if (SGIToAdd.size() > 0)
            {
                InsertSGIAtMarker(SGIToAdd,&marker);
                marker[marker.size()-1] += SGIToAdd.size();

            }


            int countToRemove = 0;
            while (GetItemAtPath(&marker)->value.find("faketree_end") == GetItemAtPath(&marker)->value.npos)  //the case newCount > oldCount we erase old inputs till reach faketree_end COSTY
            {
                marker[marker.size()-1] += 1;
                countToRemove++;

                if (!hasPushed)
                {
                    position = marker[marker.size()-1];
                    hasPushed = true;
                }
                shift--;
            }
            DebugLog( " ended 2 : " << newCount << " size to remove "  << countToRemove);
            marker[marker.size()-1] -= countToRemove;

            if (countToRemove > 0)
            {

/*
                vector<int> pathToParent = marker;
                pathToParent.resize(pathToParent.size()-1);

                if (slotType == isTroopSlot)
                    getchar();

*/
                RemoveSGIAtMarker(&marker,countToRemove);
/*
                SaveGameItem* parentSGI = GetItemAtPath(&pathToParent);


                int eraseWithChilds = 0;
                for (int i=marker[marker.size()-1];i<(marker[marker.size()-1]+countToRemove);i++)
                {
                    if (parentSGI->subItems[i].subItems.size() > 0)
                    {
                        eraseWithChilds++;
                    }
                }

                cout << "erase with childs : " << eraseWithChilds << endl;

                //parentSGI->subItems.erase(parentSGI->subItems.begin()+marker[marker.size()-1],parentSGI->subItems.begin()+marker[marker.size()-1]+countToRemove);

                eraseWithChilds = 0;
                for (int i=marker[marker.size()-1];i<(marker[marker.size()-1]+countToRemove);i++)
                {
                    if (parentSGI->subItems[i].subItems.size() > 0)
                    {
                        eraseWithChilds++;
                    }
                }
                cout << "erase with childs 2 : " << eraseWithChilds << endl;

               // parentSGI->subItems.reserve(parentSGI->subItems.size());
                //parentSGI->subItems.shrink_to_fit();
                if (slotType == isTroopSlot)
                {
                    cout << "done size is now : " << GetItemAtPath(&pathToParent)->subItems.size() <<  endl;
                    getchar();
                }*/
            }

            DebugLog( " ended 3 : " << newCount << " size to remove "  << countToRemove);

            if (mergeBehaviour >= REPLACE_BY_FACTION_ID && mergeBehaviour <= REPLACE_BY_ITEM_ID)
                ShiftCurIdList(position,shift);

            int faketreeend = id + newCount + 1;
            DebugLog( " ended 4 : " << newCount << " size to remove "  << countToRemove);

            while (curTree[faketreeend].value.find("faketree_end") == curTree[faketreeend].value.npos)  //the case newCount > oldCount we erase old inputs till reach faketree_end COSTY
            {
                faketreeend++;
            }
            DebugLog( " ended 3 : " << newCount << " faketreeend "  << faketreeend);

            return faketreeend;
        }

    }
    else if (structureBlocks.count(curTree[id].infos->type))    //new block of blocks
    {
        bool hasPushed = false;
        unsigned int position = 0;
        int shift = 0;

        int nextSGIId = GetNextSGI(curTree[id].infos->name,marker);
        int blockCount = atoi(&Evaluate(curTree[id].infos->dataCount)[0]);

        if (nextSGIId == -1)    //if missing
        {

            if (blockCount > 0 && curTree[id].infos->condition.size() < 2) //and should exist now but make sure it doesn't requires special condition
            {
                marker[marker.size()-1] += 1;   //we create it, with copy of the original
                nextSGIId = marker[marker.size()-1];
                InsertSGIAtMarker(curTree[id],&marker);
                if (!hasPushed)
                {
                    position = marker[marker.size()-1];
                    hasPushed = true;
                }
                shift++;
            }
            else
            {
                return (id);
            }
        }
        else //it exists
        {
            marker[marker.size()-1] = nextSGIId;

            if (blockCount > 0)
            {
                if ((curTree[id].infos->type == "player_party_stack" || (curTree[id].infos->type == "party_stack" && browsingPlayers)) &&
                    CombineBehaviours(mergeBehaviour,curTree[id].infos->mergeBehaviour) == REPLACE && (userSettings.replace_party_player_troops && userSettings.replace_parties && userSettings.replace_party_player))
                {
                    WarningLog("forced copy of " << curTree[id].infos->name);
                    GetItemAtPath(&marker)->subItems = curTree[id].subItems;
                }
                else if ((curTree[id].infos->type == "party_stack" && browsingLords) &&
                         CombineBehaviours(mergeBehaviour,curTree[id].infos->mergeBehaviour) == REPLACE && (userSettings.replace_party_lords_troops && userSettings.replace_party_lords && userSettings.replace_parties))
                {
                    WarningLog("forced copy of " << curTree[id].infos->name);
                    GetItemAtPath(&marker)->subItems = curTree[id].subItems;
                    CleanPartyStack(GetItemAtPath(&marker)->subItems);
                }
                else if ((curTree[id].infos->type == "party_stack" && browsingTowns) &&
                         CombineBehaviours(mergeBehaviour,curTree[id].infos->mergeBehaviour) == REPLACE && (userSettings.replace_party_towns_troops && userSettings.replace_party_towns && userSettings.replace_parties))
                {
                    WarningLog("forced copy of " << curTree[id].infos->name);
                    GetItemAtPath(&marker)->subItems = curTree[id].subItems;
                    CleanPartyStack(GetItemAtPath(&marker)->subItems);

                }
            }
            else
            {
                RemoveSGIAtMarker(&marker);
                marker[marker.size()-1] -= 1;
                if (!hasPushed)
                {
                    position = marker[marker.size()-1];
                    hasPushed = true;
                }
                shift--;
                if (mergeBehaviour >= REPLACE_BY_FACTION_ID && mergeBehaviour <= REPLACE_BY_ITEM_ID && shift != 0)
                    ShiftCurIdList(position,shift);

                return (id);
            }
        }

        if (mergeBehaviour >= REPLACE_BY_FACTION_ID && mergeBehaviour <= REPLACE_BY_ITEM_ID && shift != 0)
            ShiftCurIdList(position,shift);

        if (curTree[id].infos->type == "quest" && !userSettings.replace_quests)
            return id;
        if (curTree[id].infos->type == "faction" && !userSettings.replace_factions)
            return id;
        if (curTree[id].infos->type == "party_record" && !userSettings.replace_parties)
            return id;
        if (curTree[id].infos->type == "party_template" && !userSettings.replace_party_templates)
            return id;
        if (curTree[id].infos->type == "troop" && !userSettings.replace_troops)
            return id;
        if (curTree[id].infos->type == "party_stack" && !userSettings.replace_party_player_troops && browsingPlayers)
            return id;
        if (curTree[id].infos->type == "party_stack" && !userSettings.replace_party_lords_troops && browsingLords)
            return id;
        if (curTree[id].infos->type == "party_stack" && !userSettings.replace_party_towns_troops && browsingTowns)
            return id;
        if (curTree[id].infos->type == "player_party_stack" && (!userSettings.replace_party_player_troops || !userSettings.replace_parties || !userSettings.replace_party_player))
            return id;
        if (curTree[id].infos->name == "equipped_items")
        {
            if (!userSettings.replace_troop_player_equipment && browsingPlayers)
                return id;
            else if (!userSettings.replace_troop_lord_equipment && browsingLords)
                return id;
            else if (!userSettings.replace_troop_lady_equipment && browsingLadies)
                return id;
            else if (!userSettings.replace_troop_heroes_equipment && browsingHeroes)
                return id;
            else if (!userSettings.replace_troop_nonheroes_equipment && browsingNonHeroes)
                return id;
        }


        GetItemAtPath(&marker)->finalBehaviour = CombineBehaviours(mergeBehaviour,curTree[id].infos->mergeBehaviour);
        marker.push_back(-1);
        ConvertItemList(curTree[id].subItems,marker,CombineBehaviours(mergeBehaviour,curTree[id].infos->mergeBehaviour));
        marker.pop_back();

    }

    return id;
}

vector<string> SaveGame::GetFakeSGIBeginData(string val)
{
    vector<string> results;

    val = val.substr(val.find("+")+1,val.npos);
    results.push_back(val.substr(0,val.find("+"))); //type

    val = val.substr(val.find("+")+1,val.npos);
    results.push_back(val.substr(0,val.find("+"))); //name

    val = val.substr(val.find("+")+1,val.npos);     //count
    string value = val.substr(0,val.npos);

    for (int i=value.size()-1; i >= 0; i--)
        if (value[i] == '(' || value[i] == ')' || value[i] == ' ')
            value.erase(value.begin()+i);

    results.push_back(value);
    return results;
}

void SaveGame::MergeSGI(SaveGameItem* originalSGI, SaveGameItem* newSGI, int mergeBehave)
{
    if (browsingParty)
        DebugLog("replace " << *originalSGI->displayName << " to " << *newSGI->displayName << " with behave : " << mergeBehave)
    //getchar();

    if (originalSGI->isFake || newSGI->isFake)
        ErrorLog("Not supposed to replace Fake SGI ? : " << *originalSGI->displayName << " value : " << originalSGI->value);

    if (((!userSettings.replace_party_player_position && browsingPlayers && browsingParty) ||
        (!userSettings.replace_party_lords_position && browsingLords && browsingParty) ||
        (!userSettings.replace_party_towns_position && browsingTowns && browsingParty)) &&
        (originalSGI->infos->name != "slots")) // otpimisation
    {
        if (originalSGI->infos->name == "initial_position_x" ||
                originalSGI->infos->name == "initial_position_y" ||
                originalSGI->infos->name == "target_position_x" ||
                originalSGI->infos->name == "target_position_y" ||
                originalSGI->infos->name == "position_x" ||
                originalSGI->infos->name == "position_y" ||
                originalSGI->infos->name == "position_z")
        {
            mergeBehave = DONTREPLACE;
        }
    }
    if (!userSettings.replace_party_lords_territories && browsingTowns && browsingParty)
    {
        if (*originalSGI->displayName == "slot_town_lord")
        {
            int64_t id = *reinterpret_cast<int64_t*>(&originalSGI->value[0]);
            if (id > 0 && id < originalSaveInfos->modInfos->troops.size())              // id is valid
            {
                if (originalSaveInfos->modInfos->troops[id].infos["IDName"].find("trp_kingdom") != originalSaveInfos->modInfos->troops[id].infos["IDName"].npos ||   //id is a lord
                     originalSaveInfos->modInfos->troops[id].infos["IDName"].find("trp_knight") != originalSaveInfos->modInfos->troops[id].infos["IDName"].npos)
                {
                    mergeBehave = DONTREPLACE;
                }
            }
        }
    }
    if (!userSettings.replace_party_player_territories && browsingTowns && browsingParty)
    {
        if (*originalSGI->displayName == "slot_town_lord" && (*reinterpret_cast<int64_t*>(&originalSGI->value[0]) == 0))
        {
            mergeBehave = DONTREPLACE;
        }
    }
    if (((!userSettings.replace_troop_player_specs && browsingPlayers && browsingTroops) ||
        (!userSettings.replace_troop_lord_specs && browsingLords && browsingTroops) ||
        (!userSettings.replace_troop_lady_specs && browsingLadies && browsingTroops) ||
        (!userSettings.replace_troop_heroes && browsingHeroes && browsingTroops) ||
        (!userSettings.replace_troop_nonheroes_specs && browsingNonHeroes && browsingTroops)) &&
        (originalSGI->infos->name != "slots")) // otpimisation
    {
        if (originalSGI->infos->name == "attributes" ||
                originalSGI->infos->name == "proficiencies" ||
                originalSGI->infos->name == "skills" ||
                originalSGI->infos->name == "level" ||
                originalSGI->infos->name == "gold" ||
                originalSGI->infos->name == "experience" ||
                originalSGI->infos->name == "health")
        {
            mergeBehave = DONTREPLACE;
        }
    }

    if (originalSGI->infos->name == "num_stacks")
    {
        if (browsingPlayers && (!userSettings.replace_party_player_troops || !userSettings.replace_parties || !userSettings.replace_party_player))
        {
            mergeBehave = DONTREPLACE;
        }
        else if (browsingLords && (!userSettings.replace_party_lords_troops || !userSettings.replace_parties || !userSettings.replace_party_lords))
        {
            mergeBehave = DONTREPLACE;
        }
        else if (browsingTowns && (!userSettings.replace_party_towns_troops || !userSettings.replace_parties || !userSettings.replace_party_towns))
        {
            mergeBehave = DONTREPLACE;
        }
        lastNumStacks = &newSGI->value;
    }

    if (originalSGI->infos->name == "num_slots")
    {
        if (!userSettings.replace_faction_slots && browsingFactions)
        {
            mergeBehave = DONTREPLACE;
        }
        if ((!userSettings.replace_party_player_slots && browsingParty && browsingPlayers) ||
            (!userSettings.replace_party_lords_slots && browsingParty && browsingLords) ||
            (!userSettings.replace_party_towns_slots && browsingParty && browsingTowns))
        {
            mergeBehave = DONTREPLACE;
        }
        if (!userSettings.replace_quest_slots && browsingQuests)
        {
            mergeBehave = DONTREPLACE;
        }
        if (!userSettings.replace_party_template_slots && browsingParty_Template)
        {
            mergeBehave = DONTREPLACE;
        }
        if ((!userSettings.replace_troop_player_slots && browsingPlayers && browsingTroops) ||
            (!userSettings.replace_troop_lord_slots && browsingLords && browsingTroops) ||
            (!userSettings.replace_troop_lady_slots && browsingLadies && browsingTroops) ||
            (!userSettings.replace_troop_heroes_slots && browsingHeroes && browsingTroops) ||
            (!userSettings.replace_troop_nonheroes_slots && browsingNonHeroes && browsingTroops))
        {
            mergeBehave = DONTREPLACE;
        }
    }

    if (mergeBehave == DONTREPLACE)
        return;

    if (mergeBehave == REPLACE)
    {
        newSGI->value = originalSGI->value;
        newSGI->finalBehaviour = REPLACE;
    }
    else if (mergeBehave == CLEAR)
    {
        if (newSGI->infos->type == "char")
        {
            newSGI->value = "";
        }
        else if (newSGI->infos->type == "int32" || newSGI->infos->type == "uint32")
        {
            int32_t buf = 0;
            memcpy( &newSGI->value[ 0 ], &buf, newSGI->value.size());
        }
        else if (newSGI->infos->type == "int64" || newSGI->infos->type == "uint64")
        {
            int64_t buf = 0;
            memcpy( &newSGI->value[ 0 ], &buf, newSGI->value.size());
        }
        else if (newSGI->infos->type == "bool")
        {
            newSGI->value[0] = '\0';
        }
        else if (newSGI->infos->type == "float")
        {
            float buf = 0.0f;
            memcpy( &newSGI->value[ 0 ], &buf, newSGI->value.size());
        }
        newSGI->finalBehaviour = CLEAR;
    }
    else if (mergeBehave == DONTREPLACE_EXCEPTPLAYERS)
    {
        if (browsingPlayers)
        {
            newSGI->value = originalSGI->value;
            newSGI->finalBehaviour = REPLACE;
        }
    }
    else if (mergeBehave == REPLACE_EXCEPTPLAYERS)
    {
        if (!browsingPlayers)
        {
            newSGI->value = originalSGI->value;
            newSGI->finalBehaviour = REPLACE;
        }
    }
    else if (mergeBehave == REPLACE_BY_FACTION_ID)
    {
        int64_t convertedId = 0;
        newSGI->finalBehaviour = REPLACE_BY_FACTION_ID;

        if (originalSGI->infos->type == "int32")
            convertedId = originalSaveInfos->modInfos->GetOthersFactionId(*reinterpret_cast<int32_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "int64")
            convertedId = originalSaveInfos->modInfos->GetOthersFactionId(*reinterpret_cast<int64_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "uint32")
            convertedId = originalSaveInfos->modInfos->GetOthersFactionId(*reinterpret_cast<uint32_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "uint64")
            convertedId = originalSaveInfos->modInfos->GetOthersFactionId(*reinterpret_cast<uint64_t*>(&originalSGI->value[0]),modInfos);

        if (convertedId != -666)
        {
            if (originalSGI->infos->type == "int32")
            {
                int32_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "int64")
            {
                int64_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "uint32")
            {
                uint32_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "uint64")
            {
                uint64_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
        }
    }
    else if (mergeBehave == REPLACE_BY_GLOBALVAR_ID)
    {
        //DANGEROUS SO FAR and not used, We would have to set a behaviour for each one, anyway It wouldn't happens here actually
    }
    else if (mergeBehave == REPLACE_BY_PARTY_ID)
    {
        newSGI->finalBehaviour = REPLACE_BY_PARTY_ID;
        int64_t convertedId = 0;

        if (originalSGI->infos->type == "int32")
            convertedId = originalSaveInfos->modInfos->GetOthersPartyId(*reinterpret_cast<int32_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "int64")
            convertedId = originalSaveInfos->modInfos->GetOthersPartyId(*reinterpret_cast<int64_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "uint32")
            convertedId = originalSaveInfos->modInfos->GetOthersPartyId(*reinterpret_cast<uint32_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "uint64")
            convertedId = originalSaveInfos->modInfos->GetOthersPartyId(*reinterpret_cast<uint64_t*>(&originalSGI->value[0]),modInfos);

        if (convertedId != -666)
        {
            if (originalSGI->infos->type == "int32")
            {
                int32_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "int64")
            {
                int64_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "uint32")
            {
                uint32_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "uint64")
            {
                uint64_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
        }
    }
    else if (mergeBehave == REPLACE_BY_PARTY_TEMPLATE_ID)
    {
        newSGI->finalBehaviour = REPLACE_BY_PARTY_TEMPLATE_ID;
        int64_t convertedId = 0;

        if (originalSGI->infos->type == "int32")
            convertedId = originalSaveInfos->modInfos->GetOthersPartyTemplateId(*reinterpret_cast<int32_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "int64")
            convertedId = originalSaveInfos->modInfos->GetOthersPartyTemplateId(*reinterpret_cast<int64_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "uint32")
            convertedId = originalSaveInfos->modInfos->GetOthersPartyTemplateId(*reinterpret_cast<uint32_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "uint64")
            convertedId = originalSaveInfos->modInfos->GetOthersPartyTemplateId(*reinterpret_cast<uint64_t*>(&originalSGI->value[0]),modInfos);

        if (convertedId != -666)
        {
            if (originalSGI->infos->type == "int32")
            {
                int32_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "int64")
            {
                int64_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "uint32")
            {
                uint32_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "uint64")
            {
                uint64_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
        }
    }
    else if (mergeBehave == REPLACE_BY_PARTY_RECORD_ID) //NOT SUPPOSED TO HAPPENS FOR SG's variables
    {
        newSGI->finalBehaviour = REPLACE_BY_PARTY_RECORD_ID;
        int64_t convertedId = 0;

        if (originalSGI->infos->type == "int32")
            convertedId = originalSaveInfos->modInfos->GetOthersPartyRecordId(*reinterpret_cast<int32_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "int64")
            convertedId = originalSaveInfos->modInfos->GetOthersPartyRecordId(*reinterpret_cast<int64_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "uint32")
            convertedId = originalSaveInfos->modInfos->GetOthersPartyRecordId(*reinterpret_cast<uint32_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "uint64")
            convertedId = originalSaveInfos->modInfos->GetOthersPartyRecordId(*reinterpret_cast<uint64_t*>(&originalSGI->value[0]),modInfos);

        if (convertedId != -666)
        {
            if (originalSGI->infos->type == "int32")
            {
                int32_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "int64")
            {
                int64_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "uint32")
            {
                uint32_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "uint64")
            {
                uint64_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
        }
    }
    else if (mergeBehave == REPLACE_BY_QUEST_ID) //NOT SUPPOSED TO HAPPENS FOR SG's variables
    {
        newSGI->finalBehaviour = REPLACE_BY_QUEST_ID;
        int64_t convertedId = 0;

        if (originalSGI->infos->type == "int32")
            convertedId = originalSaveInfos->modInfos->GetOthersQuestId(*reinterpret_cast<int32_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "int64")
            convertedId = originalSaveInfos->modInfos->GetOthersQuestId(*reinterpret_cast<int64_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "uint32")
            convertedId = originalSaveInfos->modInfos->GetOthersQuestId(*reinterpret_cast<uint32_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "uint64")
            convertedId = originalSaveInfos->modInfos->GetOthersQuestId(*reinterpret_cast<uint64_t*>(&originalSGI->value[0]),modInfos);

        if (convertedId != -666)
        {
            if (originalSGI->infos->type == "int32")
            {
                int32_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "int64")
            {
                int64_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "uint32")
            {
                uint32_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "uint64")
            {
                uint64_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
        }
    }
    else if (mergeBehave == REPLACE_BY_SCENE_ID) //NOT SUPPOSED TO HAPPENS FOR SG's variables (So Far)
    {
        newSGI->finalBehaviour = REPLACE_BY_SCENE_ID;
        int64_t convertedId = 0;

        if (originalSGI->infos->type == "int32")
            convertedId = originalSaveInfos->modInfos->GetOthersSceneId(*reinterpret_cast<int32_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "int64")
            convertedId = originalSaveInfos->modInfos->GetOthersSceneId(*reinterpret_cast<int64_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "uint32")
            convertedId = originalSaveInfos->modInfos->GetOthersSceneId(*reinterpret_cast<uint32_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "uint64")
            convertedId = originalSaveInfos->modInfos->GetOthersSceneId(*reinterpret_cast<uint64_t*>(&originalSGI->value[0]),modInfos);

        if (convertedId != -666)
        {
            if (originalSGI->infos->type == "int32")
            {
                int32_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "int64")
            {
                int64_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "uint32")
            {
                uint32_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "uint64")
            {
                uint64_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
        }
    }
    else if (mergeBehave == REPLACE_BY_TROOP_ID)
    {
        newSGI->finalBehaviour = REPLACE_BY_TROOP_ID;
        int64_t convertedId = 0;

        if (originalSGI->infos->type == "int32")
            convertedId = originalSaveInfos->modInfos->GetOthersTroopId(*reinterpret_cast<int32_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "int64")
            convertedId = originalSaveInfos->modInfos->GetOthersTroopId(*reinterpret_cast<int64_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "uint32")
            convertedId = originalSaveInfos->modInfos->GetOthersTroopId(*reinterpret_cast<uint32_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "uint64")
            convertedId = originalSaveInfos->modInfos->GetOthersTroopId(*reinterpret_cast<uint64_t*>(&originalSGI->value[0]),modInfos);

        if (convertedId != -666)
        {
            if (originalSGI->infos->type == "int32")
            {
                int32_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "int64")
            {
                int64_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "uint32")
            {
                uint32_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "uint64")
            {
                uint64_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
        }
    }
    else if (mergeBehave == REPLACE_BY_ITEM_ID)
    {
        newSGI->finalBehaviour = REPLACE_BY_ITEM_ID;
        int64_t convertedId = 0;

        if (originalSGI->infos->type == "int32")
            convertedId = originalSaveInfos->modInfos->GetOthersItemId(*reinterpret_cast<int32_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "int64")
            convertedId = originalSaveInfos->modInfos->GetOthersItemId(*reinterpret_cast<int64_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "uint32")
            convertedId = originalSaveInfos->modInfos->GetOthersItemId(*reinterpret_cast<uint32_t*>(&originalSGI->value[0]),modInfos);
        else if (originalSGI->infos->type == "uint64")
            convertedId = originalSaveInfos->modInfos->GetOthersItemId(*reinterpret_cast<uint64_t*>(&originalSGI->value[0]),modInfos);

        if (convertedId != -666)
        {
            if (originalSGI->infos->type == "int32")
            {
                int32_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "int64")
            {
                int64_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "uint32")
            {
                uint32_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
            else if (originalSGI->infos->type == "uint64")
            {
                uint64_t out = convertedId;
                memcpy( &newSGI->value[ 0 ], &out, originalSGI->value.size());
            }
        }
    }
}



int SaveGame::GetSlotBehave(int id, int slotType)
{
    if (slotType == isTroopSlot)
    {
        map<unsigned int,Slot>::const_iterator itr = slotsInfos.troopSlots.find((unsigned int)id);

        if (itr != slotsInfos.troopSlots.end())
            return itr->second.mergeBehaviour;
        else
            return DONTREPLACE;
    }
    if (slotType == isFactionSlot)
    {
        map<unsigned int,Slot>::const_iterator itr = slotsInfos.factionSlots.find((unsigned int)id);

        if (itr != slotsInfos.factionSlots.end())
            return itr->second.mergeBehaviour;
        else
            return DONTREPLACE;
    }
    if (slotType == isPartySlot)
    {
        map<unsigned int,Slot>::const_iterator itr = slotsInfos.partySlots.find((unsigned int)id);

        if (itr != slotsInfos.partySlots.end())
            return itr->second.mergeBehaviour;
        else
            return DONTREPLACE;
    }
    if (slotType == isQuestSlot)
    {
        map<unsigned int,Slot>::const_iterator itr = slotsInfos.questSlots.find((unsigned int)id);

        if (itr != slotsInfos.questSlots.end())
            return itr->second.mergeBehaviour;
        else
            return DONTREPLACE;
    }
    if (slotType == isItemSlot)
    {
        map<unsigned int,Slot>::const_iterator itr = slotsInfos.itemSlots.find((unsigned int)id);

        if (itr != slotsInfos.itemSlots.end())
            return itr->second.mergeBehaviour;
        else
            return DONTREPLACE;
    }
    if (slotType == isSceneSlot)
    {
        map<unsigned int,Slot>::const_iterator itr = slotsInfos.sceneSlots.find((unsigned int)id);

        if (itr != slotsInfos.sceneSlots.end())
            return itr->second.mergeBehaviour;
        else
            return DONTREPLACE;
    }
    if (slotType == isPartyTemplateSlot)
    {
        map<unsigned int,Slot>::const_iterator itr = slotsInfos.partyTemplateSlots.find((unsigned int)id);

        if (itr != slotsInfos.partyTemplateSlots.end())
            return itr->second.mergeBehaviour;
        else
            return DONTREPLACE;
    }
    return DONTREPLACE;
}

void SaveGame::CleanPartyStack(List<SaveGameItem>& list)
{
    for (unsigned int i=0; i < list.size(); i++)
    {
        if (list[i].infos->name == "troop_id" && originalSaveInfos->modInfos->GetOthersTroopId(*reinterpret_cast<int32_t*>(&(list[i].value[0])),modInfos) == -666)
        {
            WarningLog("Couldn't find the new Id of an army troop : " << originalSaveInfos->modInfos->troops[*reinterpret_cast<int32_t*>(&(list[i].value[0]))].infos["IDName"] )
            list.erase(i, i + 4);
            i--;
        }
    }
    int32_t newCount = 0;

    for (unsigned int i=0; i < list.size(); i++)
        if (list[i].infos->name == "troop_id")
            newCount++;

    memcpy( &(*lastNumStacks)[0], &newCount, 4);
}
