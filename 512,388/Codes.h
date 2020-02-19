// Header Implementation Information //////
/**
  * @file Codes.h
  *
  *@brief extern info for all files
  *
  *@details Contains all extern declarations for all Computer message
  * codes as well as comments listing brief explanations.
  *
  *@note Error codes are NEGATIVE, all other codes positive
  *@note Exception: ERROR_CODE, NO_ERROR is reserved value zero ( 0 )
  *
  * @version 1.00
  *          Anonymous (2019 September 10)
  *          Initial Development and testing
  */

#ifndef CODES_H
#define CODES_H

// CPU Codes as enum. /////////////////////////////////////////////////////////
typedef enum
{
FCFS_N = 50,
SJF_N = 51,
SRTF_P = 52,
FCFS_P = 53,
RR_P = 54
}
CPU_CODE;

extern CPU_CODE cpuCode;

// Error Codes as enum ////////////////////////////////////////////////////////
typedef enum
{
    NO_ERROR = 0,
    INSUFFICENT_MEMORY = -1,
    FILE_READ_ERROR = -3,
    INVALID_FILE_TYPE = -2,
    INCOMPLETE_FILE_ERROR = -4,
    INVALID_DATA_VALUE = -5,
    FILE_WRITE_ERROR = -6,
    META_FILE_ERROR = -12,
    META_READ_ERROR = -13,
    LOG_ERROR = -30,
    SUBSTRING_NOT_FOUND = -21,
    NULL_STRING_ERROR = -22
}
ERROR_CODE;

extern ERROR_CODE errorCode;

// Log Codes as enum //////////////////////////////////////////////////////////
typedef enum
{
    MONITOR_LOG = 90,
    FILE_LOG = 91,
    BOTH_LOG = 92
}
LOG_CODE;

extern enum LOG_CODE logCode;
// Meta Data Codes ////////////////////////////////////////////////////////////
typedef enum
{
    OS = 30,
    APP = 31,
    PROC = 32,
    MEM_ACC = 33,
    INPUT = 34,
    OUTPUT = 35
}
COMPONENT_CODE;

extern enum COMPONENT_CODE componetCode;

typedef enum
{
    ACCESS = 70,
    ALLOCATE = 71,
    END = 72,
    HDD = 73,
    KEYBOARD = 74,
    PRINTER = 75,
    MONITOR = 76,
    RUN = 77,
    START = 78
}
OPPERATION_CODE;

extern enum OPPERATION_CODE operationCode;

#endif // CODES_H
