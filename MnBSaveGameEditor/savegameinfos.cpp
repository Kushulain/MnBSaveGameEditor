#include "savegameinfos.h"

SaveGameInfos::SaveGameInfos(string modDirectory, string SaveGamePath,
                             map<string,vector<SaveGameStructureItem> > SGStructure,
                             map<string,streamsize> aTypeSize,
                             map<string,string> aConditionalValueSave,
                             map<string,int64_t> aWatchedValues,
                             vector<string>* modINIValues,
                             QProgressDialog* aProgressBar,
                             SlotsInfos* slotsInfos)
{

    map<string,int64_t> watchedValues = aWatchedValues;


    loadedSuccefully = true;

    InfoLog("");
    InfoLog("====LOADING MODULE FILES :");
    InfoLog("");
    modInfos = new ModInfos(modDirectory,modINIValues,&watchedValues);

    if (!modInfos->loadedSuccefully)
    {
        loadedSuccefully = false;
        return;
    }



    InfoLog("");
    InfoLog("====LOADING SAVEGAME FILES :");
    InfoLog("");
    savegame = new SaveGame(SaveGamePath, SGStructure, aTypeSize, aConditionalValueSave, watchedValues, modInfos, aProgressBar, slotsInfos);

    if (!savegame->loadedSuccefully)
    {
        loadedSuccefully = false;
    }
    else
    {
        string modName = modDirectory;
        modName = modName.substr(modName.find_last_of("\\/")+1, modName.npos);
        string savegameAddress = "game.header.savegame_name.text";
        savegameAddress = savegame->LookForSGVariable(savegameAddress);
        name = modName + "/" + savegameAddress;
    }

    cout << "Done." << endl;
}

SaveGameInfos::~SaveGameInfos()
{
    //
    cout << "TESSSSSSSSSSSSSST" << endl;
    delete(savegame);
    delete(modInfos);
}
