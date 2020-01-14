// header files
#include <stdio.h>
#include "ConfigAccess.h"
#include "MetaDataAccess.h"
#include "simtimer.h"
#include "SimUtils.h"

/***********************************************************
    Used threads and memory allocation should be working correctly
 ***********************************************************/

/*
Function name: main
Algorithm: driver function to test config file upload operation
Precondition: none
Postcondition: returns 0 on success
Exceptions: none
Note: demonstrate development and use of config file upload
      operations and function.
*/
int main(int argc, char **argv)
{
    int configAccessResult;
    int mdAccessResult;
    char configFileName[MAX_STR_LEN];
    char mdFileName[MAX_STR_LEN];
    ConfigDataType *configDataPtr;
    OpCodeType *mdDataPtr;

    printf("\nSimulator Program\n");
    printf("===================\n\n");

    printf("Uploading Configuration Files\n");

    if (argc < 2)
    {
        printf("ERROR: Program requires file name for config file");
        printf(" as command line argument\n");
        printf("Program terinated\n");
        return 1;
    }

    copyString(configFileName, argv[1]);
    configAccessResult = getConfigData(configFileName, &configDataPtr);

    if (configAccessResult != NO_ERR)
    {
        displayConfigError(configAccessResult);
        clearConfigData(&configDataPtr);
        printf("\n");
        return 0;
    }

    printf("\nUploading Metadata Files\n");

    copyString(mdFileName, configDataPtr->metaDataFileName);
    mdAccessResult = getOpCodes(mdFileName, &mdDataPtr);

    if (mdAccessResult != NO_ERR)
    {
        displayMetaDataError(mdAccessResult);
        clearConfigData(&configDataPtr);
        mdDataPtr = clearMetaDataList(mdDataPtr); // returns null
        printf("\n");
        return 0;
    }
    printf("\n");
    runSim(mdDataPtr, configDataPtr);
    clearConfigData(&configDataPtr);
    mdDataPtr = clearMetaDataList(mdDataPtr); // returns null
    printf("\n");
    return 0;
}