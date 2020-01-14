// Pre-compiler directive
#ifndef SIM_UTILS_H
#define SIM_UTILS_H

#include "MetaDataAccess.h"
#include "ConfigAccess.h"
#include "simtimer.h"
#include "StringUtils.h"
#include <pthread.h>

// Buffer Structure
typedef struct LogOutput
{
    char outputStr[200];
    struct LogOutput *next;
} LogOutput;

// Thread input data structure
typedef struct
{
    ConfigDataType *configData;
    struct OpCodeType *opCode;
    int processNum;
    double cycleTime;
    char outputStr[200];
    pthread_mutex_t *mutexLock;
} TimerRunnerStruct;

// Process States
typedef enum
{
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    EXIT
} ProcessStates;

// PCB Data Structure
typedef struct ProcessControlBlock
{
    int state;
    int number;
    struct OpCodeType *programCounter;
    double timeRemaining;
    double totalTime;
    int priority;
    struct ProcessControlBlock *next;
} ProcessControlBlock;

// Memory Structure
typedef struct MemoryNode
{
    int processNum;
    int identifier;
    int base;
    int offset;
    struct MemoryNode *next;
} MemoryNode;

// Memory Error, start at 3 to incorporate errors from StringUtils
typedef enum
{
    SEG_FAULT = 3
} MemoryErrors;

// Interrupt manager codes
typedef enum
{
    ADD_NODE,
    DELETE_NODE,
    CHECK_FOR_INTERRUPTS,
    RESOLVE_INTERRUPTS,
    CHECK_QUEUE,
} SimManagerCodes;

// Interrupt List
typedef struct InterruptNode
{
    int processNum;
    char outputStr[200];
    int cycleTime;
    struct InterruptNode *next;
} InterruptNode;

// Function Prototypes
void *timerRunner(void *inputStruct);
void runSim(struct OpCodeType *opCodes, ConfigDataType *configData);
void createProcesses(OpCodeType *opCodes, ConfigDataType *configData,
                     ProcessControlBlock *headPtr);
ProcessControlBlock *addNewPCB(ProcessControlBlock *head, int number, int priority,
                               OpCodeType *programCounter);
ProcessControlBlock *runProcess(ProcessControlBlock *currentProcess, ConfigDataType *configData,
                                LogOutput *ouptutPtr, MemoryNode *memoryPtr, ProcessControlBlock *headPtr);
double calcCycleTime(OpCodeType *opCodes, ConfigDataType *configData);
void outputToFile(LogOutput *ouptutPtr, char *outputFileName);
LogOutput *createLogFileHeader(ConfigDataType *configData,
                               LogOutput *ouptutPtr);
LogOutput *addNewStrNode(LogOutput *head, char *logString);
LogOutput *clearLogOutput(LogOutput *head);
LogOutput *outputLine(int logToCode, LogOutput *ouptutPtr, char *inputStr);
int memoryManager(ProcessControlBlock process, ConfigDataType *configData,
                  LogOutput *ouptutPtr, MemoryNode *memoryPtr);
MemoryNode *addNewMemoryNode(MemoryNode *head, int memoryID, int memoryBase,
                             int memoryOffset, int processNum);
Boolean requestMemory(MemoryNode *memoryPtr, int processNum, int identifier, int base, int offset);
Boolean memorySpaceFree(MemoryNode *memoryPtr, int memoryBase, int processNum,
                        int memoryIdentifier);
MemoryNode *clearMemoryList(MemoryNode *head);

ProcessControlBlock *findMinimum(ProcessControlBlock *headPtr, int schedCode);
ProcessControlBlock *findNextReady(ProcessControlBlock *headPtr, int schedCode);
ProcessControlBlock *selectNextProcess(ProcessControlBlock *headPtr, int schedCode);
ProcessControlBlock *interruptManager(int interruptCode, int interruptedProcess, int processNum,
                                      char *outputString, LogOutput *ouptutPtr, ConfigDataType *configData, ProcessControlBlock *headPtr, int cycleTime);
InterruptNode *addNewInterruptNode(InterruptNode *head, int processNum, int cycleTime, char *outputString);
ProcessControlBlock *getLastNode(ProcessControlBlock *head);
Boolean allProcessesEnded(ProcessControlBlock *headPtr);
Boolean waitingQueue(int code, int processNum);
ProcessControlBlock *reorderPCBList(ProcessControlBlock *headPtr, int processNum);
ProcessControlBlock *findPCB(ProcessControlBlock *headPtr, int processNum);
ProcessControlBlock *clearPCBList(ProcessControlBlock *head);
void *timerRunnerPremptive(void *inputStruct);

#endif // SIM_UTILS_H
