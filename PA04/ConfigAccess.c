// header files
#include "ConfigAccess.h"

/*
Function name:  getConfigData
Algorithm:      opens file, acquires configuration data,
                returns pointer to data structure
Precondition:   for correct operation, file is available, is formatted
                correctly, and has all configuration lines and data although
                the configuration lines are not required to be in a specific
                order
Postcondition:  in correct operation, returns pointer to correct
                configuration data structure
Exceptions:     correct and appropriately (without program failure)
                responds to and reports filea ccess failure,
                incorrectly formatted lead or end descriptors,
                incorrectly formatted prompt, data out of range, and
                incomplete file conditions
Notes: none
 */
int getConfigData(char *fileName, ConfigDataType **configData)
{
    const int NUM_DATA_LINES = 9;
    const char READ_ONLY_FLAG[] = "r";
    int lineCtr = 0;

    ConfigDataType *tempData;
    FILE *fileAccessPtr;
    char dataBuffer[MAX_STR_LEN];
    int dataLineCode;   // line leader number code
    int intData;
    double doubleData;

    // initilialize config data pointer in case of return error
    *configData = NULL;

    fileAccessPtr = fopen(fileName, READ_ONLY_FLAG);

    if(fileAccessPtr == NULL)
    {
        return CFG_FILE_ACCESS_ERR;
    }

    // Check for start and of sim config string
    if( (getLineTo(fileAccessPtr, MAX_STR_LEN, COLON, dataBuffer,
                    IGNORE_LEADING_WS) != NO_ERR)
                        || (compareString(dataBuffer,
                            "Start Simulator Configuration File") != STR_EQ) )
    {
        fclose(fileAccessPtr);
        return CFG_CORRUPT_DESCRIPTOR_ERR;
    }

    tempData = (ConfigDataType *) malloc(sizeof(ConfigDataType));

    while(lineCtr < NUM_DATA_LINES)
    {
        // get line leader (e.g., file path)
        if(getLineTo(fileAccessPtr, MAX_STR_LEN, COLON,
                                    dataBuffer, IGNORE_LEADING_WS) != NO_ERR )
        {
            free(tempData);
            fclose(fileAccessPtr);
            return INCOMPLETE_FILE_ERR;
        }
        // get data line by number in enum
        dataLineCode = getDataLineCode(dataBuffer);

        // check data line found
        if(dataLineCode != CFG_CORRUPT_PROMPT_ERR)
        {
            // check for double value
            if(dataLineCode == CFG_VERSION_CODE)
            {
                fscanf(fileAccessPtr, "%lf", &doubleData);
            }
            // check for string values
            else if( (dataLineCode == CFG_MD_FILE_NAME_CODE)
                            || (dataLineCode == CFG_LOG_FILE_NAME_CODE)
                            || (dataLineCode == CFG_CPU_SCHED_CODE)
                            || (dataLineCode == CFG_LOG_TO_CODE) )
            {
                fscanf(fileAccessPtr, "%s", dataBuffer);
            }
            // otherwise assume integer value
            else
            {
                fscanf(fileAccessPtr, "%d", &intData);
            }

            if(valueInRange(dataLineCode, intData, doubleData, dataBuffer)
                                                                        == True)
            {
                switch(dataLineCode)
                {
                    case CFG_VERSION_CODE:
                        tempData->version = doubleData;
                        break;

                    case CFG_MD_FILE_NAME_CODE:
                        copyString(tempData->metaDataFileName, dataBuffer);
                        break;

                    case CFG_CPU_SCHED_CODE:
                        tempData->cpuSchedCode = getCpuSchedCode(dataBuffer);
                        break;

                    case CFG_QUANT_CYCLES_CODE:
                        tempData->quantumCycles = intData;
                        break;

                    case CFG_MEM_AVAILABLE_CODE:
                        tempData->memAvailable = intData;
                        break;

                    case CFG_PROC_CYCLES_CODE:
                        tempData->procCycleRate = intData;
                        break;

                    case CFG_IO_CYCLES_CODE:
                        tempData->ioCycleRate = intData;
                        break;

                    case CFG_LOG_TO_CODE:
                        tempData->logToCode = getLogToCode(dataBuffer);
                        break;

                    case CFG_LOG_FILE_NAME_CODE:
                        copyString(tempData->logToFileName, dataBuffer);
                        break;
                }
            }
            // data value not in range
            else
            {
                free(tempData);
                fclose(fileAccessPtr);
                return CFG_DATA_OUT_OF_RANGE_ERR;
            }
        }
        // data line not found
        else
        {
            free(tempData);
            fclose(fileAccessPtr);
            return CFG_CORRUPT_PROMPT_ERR;
        }

        lineCtr++;
    }

    // Check for end of sim config string
    if( (getLineTo(fileAccessPtr, MAX_STR_LEN, PERIOD, dataBuffer,
                        IGNORE_LEADING_WS) != NO_ERR)
                             || (compareString(dataBuffer,
                                "End Simulator Configuration File") != STR_EQ) )
    {
        free(tempData);
        fclose(fileAccessPtr);
        return CFG_CORRUPT_DESCRIPTOR_ERR;
    }

    *configData = tempData;

    fclose(fileAccessPtr);
    return NO_ERR;
}

/*
Function name:  getDataLineCode
Algorithm:      tests string for one of known leader string, returns line number
                if string is correct, returns CFG_CORRUPT_DATA_ERR if string
                is not found
Precondition:   dataBuffer is valid C-Style string
Postcondition:  returns line number of data item in terms of a constant
                (e.g., CFG_VERSION_CODE, CFG_CPU_SCHED_CODE, etc.)
Exceptions:     returns CFG_CORRUPT_FILE_ERR if string is not identified
Notes: none
 */
int getDataLineCode( char *dataBuffer )
{
    // return appropriate code depending on prompt string provided

    // check for version/phase string
        // func: compareString
    if( compareString( dataBuffer, "Version/Phase" ) == STR_EQ )
    {
        // return configuration version code
        return CFG_VERSION_CODE;
    }

    // check for file path string
        // func: compareString
    if( compareString( dataBuffer, "File Path" ) == STR_EQ )
    {
        // return meta data file path code
        return CFG_MD_FILE_NAME_CODE;
    }

    // check for cpu scheduling code string
        // func: compareString
    if( compareString( dataBuffer, "CPU Scheduling Code" ) == STR_EQ )
    {
        // return cpu schedule code code
        return CFG_CPU_SCHED_CODE;
    }

    // check for quantum time string
        // func: compareString
    if( compareString( dataBuffer, "Quantum Time (cycles)" ) == STR_EQ )
    {
        // return quantum cycles code
        return CFG_QUANT_CYCLES_CODE;
    }

    // check for memory available string
        // func: compareString
    if( compareString( dataBuffer, "Memory Available (KB)" ) == STR_EQ )
    {
        // return memory available code
        return CFG_MEM_AVAILABLE_CODE;
    }

    // check for processor cycle time string
        // func: compareString
    if( compareString( dataBuffer, "Processor Cycle Time (msec)" ) == STR_EQ )
    {
        // return processor cycle code
        return CFG_PROC_CYCLES_CODE;
    }

    // check for I/O cycle time string
        // func: compareString
    if( compareString( dataBuffer, "I/O Cycle Time (msec)" ) == STR_EQ )
    {
        // return I/O cycles code
        return CFG_IO_CYCLES_CODE;
    }

    // check for log to string
        // func: compareString
    if( compareString( dataBuffer, "Log To" ) == STR_EQ )
    {
        // return log to code
        return CFG_LOG_TO_CODE;
    }

    // check for log file path string
        // func: compareString
    if( compareString( dataBuffer, "Log File Path" ) == STR_EQ )
    {
        // return log to file name code
        return CFG_LOG_FILE_NAME_CODE;
    }

    // at this point, assume failed string access, return corrupt prompt error
    return CFG_CORRUPT_PROMPT_ERR;
}

/*
Function name:  getCpuSchedCode
Algorithm:      converts string data (e.g., "SJF-N", "SRTF-P") to constant
                code number to be stored as integer
Precondition:   codeStr is a C-Style string with one of the specified cpu
                scheduling operations
Postcondition:  returns code representing scheduling actions
Exceptions:     defaults to FCFS-N code
Notes: none
 */
ConfigDataCodes getCpuSchedCode( char *codeStr )
{
    // initialize function/variables

        // set up temporary string for testing
            //func: getStringLength, malloc
        int strLen = getStringLength( codeStr );
        char *tempStr = (char *)malloc( strLen + 1 );

        // set default return to FCFS-N
        int returnVal = CPU_SCHED_FCFS_N_CODE;

    // set string to lower case for testing
        //func: setStrToLowerCase
    setStrToLowerCase( tempStr, codeStr );

    // check for SJF-N
        // function: compareString
    if( compareString( tempStr, "sjf-n" ) == STR_EQ )
    {
        // set return to SJF-N code
        returnVal = CPU_SCHED_SJF_N_CODE;
    }

    // check for SRTF-P
        // func: compareString
    if( compareString( tempStr, "srtf-p" ) == STR_EQ )
    {
        // set return to SRTF-P code
        return CPU_SCHED_SRTF_P_CODE;
    }

    // check for FCFS-P
        // func: compareString
    if( compareString( tempStr, "fcfs-p" ) == STR_EQ )
    {
        // set return to FCFS-P code
        return  CPU_SCHED_FCFS_P_CODE;
    }

    // check for RR-P
        // func: compareString
    if( compareString( tempStr, "rr-p" ) == STR_EQ )
    {
        // set return to RR-P code
        return CPU_SCHED_RR_P_CODE;
    }

    // free temp str memory
        // func: free
    free( tempStr );

    // return code found
    return returnVal;   // temp stub return
}

/*
Function name:  valueInRange
Algorithm:      tests one of three values (int, double, string) for being
                in specified range, depending on data code
                (i.e., specified config value)
Precondition:   one of the three data values is valid
Postcondition:  return True if data is within specified parameters,
                False otherwise
Exceptions:     metadata or logfile names are ignored and return True
Notes: none
 */
Boolean valueInRange(int lineCode, int intVal, double doubleVal, char *stringVal)
{
    Boolean result = True;
    char *tempStr;
    int strLen;

    switch(lineCode)
    {
        case CFG_VERSION_CODE:
            if(doubleVal < 0.00 || doubleVal > 10.00)
            {
                result = False;
            }
            break;

        case CFG_CPU_SCHED_CODE:
            strLen = getStringLength(stringVal);
            tempStr = (char *)malloc(strLen + 1);
            setStrToLowerCase(tempStr, stringVal);

            if( (compareString(tempStr, "none") != STR_EQ)
                    && (compareString(tempStr, "fcfs-n") != STR_EQ)
                    && (compareString(tempStr, "sjf-n") != STR_EQ)
                    && (compareString(tempStr, "srtf-p") != STR_EQ)
                    && (compareString(tempStr, "fcfs-p") != STR_EQ)
                    && (compareString(tempStr, "rr-p") != STR_EQ) )
            {
                result = False;
            }
            free(tempStr);
            break;

        case CFG_QUANT_CYCLES_CODE:
            if(intVal < 0 || intVal > 100)
            {
                result = False;
            }
            break;

        case CFG_MEM_AVAILABLE_CODE:
            if(intVal < 0 || intVal > 102400)
            {
                result = False;
            }
            break;

        case CFG_PROC_CYCLES_CODE:
            if(intVal < 1 || intVal > 1000)
            {
                result = False;
            }
            break;

        case CFG_IO_CYCLES_CODE:
            if(intVal < 1 || intVal > 10000)
            {
                result = False;
            }
            break;

        case CFG_LOG_TO_CODE:
            strLen = getStringLength(stringVal);
            tempStr = (char *)malloc(strLen + 1);
            setStrToLowerCase(tempStr, stringVal);

            if( (compareString(tempStr, "both") != STR_EQ)
                    && (compareString(tempStr, "monitor") != STR_EQ)
                    && (compareString(tempStr, "file") != STR_EQ) )
            {
                result = False;
            }
            free(tempStr);
            break;
    }
    return result;
}

/*
Function name:  getLogToCode
Algorithm:      converts string data (e.g., "file", "Monitor") to constant
                code number to be stored as integer
Precondition:   codeStr is a C-Style string with one of the specified
                log to operations
Postcondition:  returns code representing log to actions
Exceptions:     defaults to monitor code
Notes: none
 */
ConfigDataCodes getLogToCode( char *logToStr )
{
    // intialize func/variables

        // create temp str
            // func: getStringLength, malloc
        int strLen = getStringLength( logToStr );
        char *tempStr = (char *)malloc( strLen + 1 );

        // set default return value to log to monitor
        int returnVal = LOGTO_MONITOR_CODE;

    // set temp string to lower case
        // fucntion: setStrToLowerCase
    setStrToLowerCase( tempStr, logToStr );

    // check for BOTH
        // func: compareString
    if( compareString( tempStr, "both") == STR_EQ )
    {
        // set return value to both code
        returnVal = LOGTO_BOTH_CODE;
    }

    // check for FILE
        // func: compareString
    if( compareString( tempStr, "file") == STR_EQ )
    {
        // set return value to file code
        returnVal = LOGTO_FILE_CODE;
    }

    // free temp string memory
        //func: free
    free( tempStr );

    // return code found
    return returnVal;
}

/*
Function name:  displayConfigData
Algorithm:      diagnostic function to show config data output
Precondition:   parameter has pointer to allocated data set
Postcondition:  configuration data is displayed to the screen
Exceptions:     none
Notes: none
 */
void displayConfigData( ConfigDataType *configData )
{
    // intialize func/var
    char displayString[ STD_STR_LEN ];

    // print lines of display
        // func: printf, configCodeToString (translates coded items)
    printf( "\nConfig File Display\n" );
    printf( "\n==================\n" );
    printf("Version                 : %3.2f\n", configData->version );
    printf("Program file name       : %s\n", configData->metaDataFileName );
    configCodeToString( configData->cpuSchedCode, displayString );
    printf("CPU schedule selection  : %s\n", displayString );
    printf("Quantum time            : %d\n", configData->quantumCycles );
    printf("Memory available (KB)   : %d\n", configData->memAvailable );
    printf("Process cycle rate      : %d\n", configData->procCycleRate );
    printf("I/O cycle rate          : %d\n", configData->ioCycleRate );
    configCodeToString( configData->logToCode, displayString );
    printf("Log to selection        : %s\n", displayString );
    printf("Log file name           : %s\n", configData->logToFileName );
}

/*
Function name:  configCodeToString
Algorithm:      utility function to support display of CPU scheduling
                or Log To code strings
Precondition:   code variable holds constant value from ConfigDataCodes
                for item (e.g., CPU_SCHED_SRTF_P, etc.)
Postcondition:  string parameter hodls correct string associated
                with the given constant
Exceptions:     none
Notes: none
 */
void configCodeToString( int code, char *outString )
{
    // Define array with eight items, and short (10) lengths
    char displayStrings[ 8 ][ 10 ] = {  "SJF-N", "SRTF-P", "FCFS-P",
                                        "RR-P", "FCFS-N", "Monitor",
                                        "File", "Both" };
    // copy string to return paramter
        //func: copyString
    copyString( outString, displayStrings[ code ] );
}

/*
Function name:  displayConfigError
Algorithm:      utility function to support display of error code strings
Precondition:   parameter holds constant value from error code list for
                item (e.g., CFG_FILE_ACCESS_ERR, etc.)
Postcondition:  correct error string (related to integer error code)
                is displayed
Exceptions:     none
Notes: none
 */
void displayConfigError( int errCode )
{
    // Define array with seven items, and short (40) lengths
    // Includes three potential errors from StringManipErrors
    char displayStrings[ 7 ][ 40 ] =
                                 { "No Error",
                                   "Incomplete File Error",
                                   "Input Buffer Overrun",
                                   "Configuration File Access Error",
                                   "Corrupt Configuration Descriptor Error",
                                   "Data Out Of Range Configuration Error",
                                   "Corrupt Configuration Prompt Error" };

    // print error message
        // func: printf
    printf( "\nFATAL ERROR: %s, Program aborted\n", displayStrings[ errCode ] );
}

/*
Function name:  clearConfigData
Algorithm:      frees all allocated memory for config data
Precondition:   pointer to config data linked list passed in as paramter
Postcondition:  config data memory is freed, pointer is set to null
Exceptions:     none
Notes: none
 */
void clearConfigData( ConfigDataType **configDataPtr )
{
    // check that config data poitner is not null
    if( *configDataPtr != NULL )
    {
        // free data structure memory
            // func: free
        free( *configDataPtr );
    }

    // set conrfig data pointer to null (returned as paramter)
    *configDataPtr = NULL;
}
