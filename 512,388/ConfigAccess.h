// Header File Information //////////
/**
  * @file ConfigAccess.h
  *
  * @brief Header file for ConfigAccess.h
  *
  * @details
  *
  * @version 1.00
  *          Anonymous 2019 Febuary 1
  *          Inital development and testing
  *
  * @note Author redacted for grading integrity
  */

#ifndef CONFIG_ACCESS_H
#define CONFIG_ACEESS_H

#include "InputOutput.h"
#include "System.h"
#include <stdlib.h>

/**
  * getConfigData : returns poiner to data structure
  *
  * @pre: file is avalable
  * @pre: file is in correct format
  *
  * @post: Returns pointer to data structure
  * @post: Responds to file access failure
  * @post: responds to incorrect format
  * @post: responds to incomplete file
  */
void getConfigData( char* fileName, System* system);
/**
  * getDataLine : takes in next line from file and acuires the config setting
  *
  * @pre: file is avalable
  * @pre: file is in correct format
  *
  * @post: responds to incorrect format
  */
void getDataLine( FILE* filePointer, System* system );

/**
  * logConfigData : recrods all Config details to log queue
  *
  * @pre: valid sysstem struct
  *
  * @post: logData contains printout of all Config details
  */
void logConfigData(System* system);

// Get Data Functions Below ///////////////////////////////////////////////////

/**
  * getVersionData : takes in the version data as a string and adds it to
  *    configData struct
  *
  * @pre: valid versionData string (C-format)
  * @pre: valid System* struct
  *
  * @post: Config data added to System struct
  */
void getVersionData( char* versionData, System* system );

/**
  * getMetaFilePath : takes in the metadata file path as a string and adds it to
  *    configData struct
  *
  * @pre: valid metaFileName string (C-format)
  * @pre: valid System struct init
  *
  * @post: Metadata filepath added to System struct
  */
void getMetaFilePath( char* metaFileName, System* system );

/**
  * getSchedule : takes in the schedule data as a string and adds it to
  *    configData struct as int
  *
  * @pre: valid schedule string (C-format)
  * @pre: valid System struct init
  *
  * @post: CPU schedule added to System struct
  */
void getSchedule( char* scheduleData, System* system );

/**
  * getQuantum : takes in the quantum data as a string and adds it to
  *    configData struct after converting to int
  *
  * @pre: valid quantum string (C-format)
  * @pre: valid System struct init
  *
  * @post: quantum data added to System struct
  */
void getQuantum( char* quantumData, System* system );

/**
  * getMemory : takes in the avalable memory as a string and adds it to
  *    configData struct after converting to int
  *
  * @pre: valid avalableMemory string (C-format)
  * @pre: valid System struct init
  *
  * @post: avalableMemory added to System struct
  */
void getMemory( char* avalableMemory, System* system);

/**
  * getProcCycleTime : takes in the cycle time as a string and adds it to
  *    configData struct after converting to int
  *
  * @pre: valid cycleTime string (C-format)
  * @pre: valid System struct init
  *
  * @post: cycleTime added to System struct
  */
void getProcCycleTime( char* cycleTime, System* system );

/**
  * getIoCycleTime : takes in the cycle time as a string and adds it to
  *    configData struct after converting to int
  *
  * @pre: valid cycleTime string (C-format)
  * @pre: valid System struct init
  *
  * @post: cycleTime added to System struct
  */
void getIoCycleTime( char* cycleTime, System* system );

/**
  * getLogStyle : takes in the log type as a string and adds it to
  *    configData struct after converting to int
  *
  * @pre: valid logType string (C-format)
  * @pre: valid System struct init
  *
  * @post: cycleTime added to System struct
  */
void getLogStyle( char* logType, System* system );

/**
  * getLogFilePath : takes in the Log file path as a string and adds it to
  *    configData struct
  *
  * @pre: valid logFileName string (C-format)
  * @pre: valid System struct init
  *
  * @post: Metadata filepath added to System struct
  */
void getLogFilePath( char* logFileName, System* system );

char* returnCpuScheduleCode(System* system);

char* returnLogStyle(System* system);

#endif  //CONFIG_ACEESS_H
