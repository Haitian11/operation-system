// Code Implementation Information ////////////////////////////////////////////
/**
  * @file ConfigAccess.c
  *
  * @brief Standard string utilities for C programs
  *
  * @details Implements all standard String utility functions
  *
  * @version 1.00
  *          Anonymous (2019 Febuary 1)
  *          Inital Development and testing
  *
  * @note Requires
  * @note Author redacted for grading integrity
  */

  #include "ConfigAccess.h"
  #include "Codes.h"

// String Constants ///////////////////////////////////////////////////////////
const char CONFIG_FILE_HEADER[] = "Start Simulator Configuration File:\n";
const char VERSION_DATA[] = "Version/Phase:";
const char META_DATA_FILE_PATH[] = "File Path:";
const char CPU_SCHEDULE[] = "CPU Scheduling Code:";
const char QUANTUM_TIME[] = "Quantum Time (cycles):";
const char MEMORY_AVALABLE[] = "Memory Available (MB):";
const char PROC_CYCLE_TIME[] = "Processor Cycle Time (msec):";
const char I_O_CYCLE_TIME[] = "I/O Cycle Time (msec):";
const char LOG_TO[] = "Log To:";
const char LOG_PATH[] = "Log File Path:";

const char CONFIG_OUTPUT_HEADER[] =
            "Config File Display\n===================\n\n";
const char VERSION_OUT[] = "Version                : ";
const char FILE_PATH_OUT[] = "Program file name      : ";
const char LOG_PATH_OUT[] = "Log file name          : ";
const char CPU_SCHEDULE_OUT[] = "CPU schedule selection : ";
const char LOG_STYLE_OUT[] = "Log to selection       : ";
const char QUANTUM_OUT[] = "Quantum time           : ";
const char MEMORY_OUT[] = "Memory Avalable        : ";
const char PROCESS_OUT[] = "Process cycle rate     : ";
const char I_O_OUT[] = "I/O cycle rate         : ";

// Other Constants /////////////////////////////////////////////////////////////
const int BASE_TEN = 10;
const int VALID_ARG_COUNT = 2;
const int NUM_DATA_LINES = 9;
// Start Program Data///////////////////////////////////////////////////////////
  void getConfigData( char* fileName, System* system )
{
    const char* READ_ONLY_POINTER = "r";

    char currentLine[ MAX_STR_LEN ];
    FILE* filePointer;

    int lineIndex = 0;
    int errorFlag = 0;

    filePointer  = fopen( fileName, READ_ONLY_POINTER );
    if (filePointer == NULL )
    {
        errorHandeler(FILE_READ_ERROR, true);
    }

    nukeString( currentLine, MAX_STR_LEN );
    getLineTo( filePointer, MAX_STR_LEN, NEWLINE_CHAR, currentLine );
    if ( compareString( currentLine, &CONFIG_FILE_HEADER ) != 0 )
    {
        errorFlag = INVALID_FILE_TYPE;
    }

    while ( lineIndex < NUM_DATA_LINES)
    {
        getDataLine( filePointer, system );
        lineIndex++;
    }
    fclose(filePointer);
} //end getConfigData

void getDataLine( FILE* filePointer, System* system )
{
    const char COLON[] = ":";
  printf("Data Line\n");
    char currentLine[ MAX_STR_LEN ];
    char curDataType[ MAX_STR_LEN ];
    char curData [ MAX_STR_LEN ];

    int colonIndex;
    int index;

    nukeString( currentLine, MAX_STR_LEN );
    nukeString( curDataType , MAX_STR_LEN );
    nukeString( curData, MAX_STR_LEN );

    getLineTo( filePointer, MAX_STR_LEN, NEWLINE_CHAR, currentLine );
    colonIndex = findSubstring( COLON, currentLine );
    if ( colonIndex == SUBSTRING_NOT_FOUND )
    {
        errorHandeler( INVALID_DATA_VALUE, true);
    }
    colonIndex++;
    getSubstring( curDataType, currentLine, 0, colonIndex );
    // The + 1 to colon index is to account for the space
    getSubstring( curData, currentLine, colonIndex + 1, getStringLength( currentLine ) - 1);

    //  This is basically a switch statement for strings
    if ( compareString( curDataType, VERSION_DATA ) == 0 )
    {
        getVersionData( curData, system );
    }
    if ( compareString( curDataType, META_DATA_FILE_PATH ) == 0 )
    {
        getMetaFilePath( curData, system );
    }
    if ( compareString( curDataType, CPU_SCHEDULE ) == 0 )
    {
        getSchedule( curData, system );
    }
    if ( compareString( curDataType, QUANTUM_TIME ) == 0 )
    {
        getQuantum( curData, system );
    }
    if ( compareString( curDataType, MEMORY_AVALABLE ) == 0 )
    {
        getMemory( curData, system );
    }
    if ( compareString( curDataType, PROC_CYCLE_TIME ) == 0 )
    {
        getProcCycleTime( curData, system );
    }
    if ( compareString( curDataType, I_O_CYCLE_TIME ) == 0 )
    {
        getIoCycleTime( curData, system );
    }
    if ( compareString( curDataType, LOG_TO ) == 0 )
    {
        getLogStyle( curData, system );
    }
    if ( compareString( curDataType, LOG_PATH ) == 0 )
    {
        getLogFilePath( curData, system );
    }
    // "switch" ends here

} //End getDataLine

void logConfigData(System* system)
{
    char tempString[MAX_STR_LEN];
    addToLog(CONFIG_OUTPUT_HEADER, system);

    copyString(VERSION_OUT, tempString);
    concatanateString(tempString, system->config.version );
    concatanateString(tempString, "\n" );
    addToLog(tempString, system);

    copyString(FILE_PATH_OUT, tempString);
    concatanateString(tempString, system->config.metaDataFileName);
    concatanateString(tempString, "\n" );
    addToLog(tempString, system);

    copyString(CPU_SCHEDULE_OUT, tempString);
    concatanateString(tempString, returnCpuScheduleCode(system) );
    concatanateString(tempString, "\n" );
    addToLog(tempString, system);

    copyString(QUANTUM_OUT, tempString);
    addToLog(tempString, system);
    nukeString(tempString, MAX_STR_LEN);
    sprintf(tempString, "%d\n", system->config.quantumCycles );
    addToLog(tempString, system);

    copyString(MEMORY_OUT, tempString);
    addToLog(tempString, system);
    nukeString(tempString, MAX_STR_LEN);
    sprintf(tempString, "%d\n", system->memory.memAvalable );
    addToLog(tempString, system);

    copyString(PROCESS_OUT, tempString);
    addToLog(tempString, system);
    nukeString(tempString, MAX_STR_LEN);
    sprintf(tempString, "%d\n", system->config.procCycleRate );
    addToLog(tempString, system);

    copyString(I_O_OUT, tempString);
    addToLog(tempString, system);
    nukeString(tempString, MAX_STR_LEN);
    sprintf(tempString, "%d\n", system->config.ioCycleRate);
    addToLog(tempString, system);

    copyString(LOG_STYLE_OUT, tempString);
    concatanateString(tempString, returnLogStyle(system) );
    concatanateString(tempString, "\n" );
    addToLog(tempString, system);

    copyString(LOG_PATH_OUT, tempString);
    concatanateString(tempString, system->log.logToFileName);
    concatanateString(tempString, "\n\n" );
    addToLog(tempString, system);
} //end logConfigData

// Get Data Functions //////////////////////////////////////////////////////////

void getVersionData( char* versionData, System* system )
{
    copyString( versionData, system->config.version );
} //End getVersionData

void getMetaFilePath( char* metaFileName, System* system )
{
    copyString( metaFileName, system->config.metaDataFileName );
} //End getMetaFilePath

void getSchedule( char* scheduleData, System* system )
{
     char cpuScheduleCode;
    if ( compareString( scheduleData, "SJF-N" ) == 0 )
    {
        cpuScheduleCode = (char)(SJF_N);
    }
    else if ( compareString( scheduleData, "SRTF-P" ) == 0 )
    {
        cpuScheduleCode = (char)(SRTF_P);
    }
    else if ( compareString( scheduleData, "FCFS-P" ) == 0 )
    {
        cpuScheduleCode = (char)(FCFS_P);
    }
    else if ( compareString( scheduleData, "RR-P" ) == 0 )
    {
        cpuScheduleCode = (char)(RR_P);
    }
    else
    {
        cpuScheduleCode = (char)(FCFS_N);
    }
    system->config.cpuScheduleCode = cpuScheduleCode;
}  //End getSchedule

void getQuantum( char* quantumData, System* system  )
{
    system->config.quantumCycles = atoi( quantumData );
}//End getQuantum

void getMemory( char* avalableMemory, System* system )
{
    system->memory.memAvalable = atoi( avalableMemory );
}//End getMemory

void getProcCycleTime( char* cycleTime, System* system )
{
   system->config.procCycleRate = atoi( cycleTime );
}//End getCycleTime

void getIoCycleTime( char* cycleTime, System* system )
{
    system->config.ioCycleRate = atoi( cycleTime );
}//End getIoCycleTime

void getLogStyle( char* logType, System* system )
{
    char logToCode;
    if ( compareString( logType, "Monitor" ) == 0 )
    {
        logToCode = (char)(MONITOR_LOG);
    }
    if ( compareString( logType, "File" ) == 0 )
    {
        logToCode = (char)(FILE_LOG);
    }
    if ( compareString( logType, "Both" ) == 0 )
    {
        logToCode = (char)(BOTH_LOG);
    }
    system->log.logToCode;
} //End getLogStyle

void getLogFilePath( char* logFileName, System* system )
{
    copyString( logFileName, system->log.logToFileName );
}//End getLogFilePath


char* returnCpuScheduleCode(System* system)
{
    switch (system->config.cpuScheduleCode)
    {
        case SJF_N:
            return "SJF_N";
        case SRTF_P:
            return "SRTF_P";
        case FCFS_P:
            return "FCFS_P";
        case RR_P:
            return "RR_P";
        case FCFS_N:
            return "FCFS_N";
        default:
            return "";
    }
}

char* returnLogStyle(System* system)
{
    switch (system->log.logToCode)
    {
        case MONITOR_LOG:
            return "Monitor";
        case FILE_LOG:
            return "File";
        case BOTH_LOG:
            return "Both";
        default:
            return "";
    }
} //end returnLogStyle
