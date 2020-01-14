// Header files
#include <stdio.h>
#include <stdlib.h>

#include "SimUtils.h"

// Value used in placeholder node
const int NA = -1;

/**
 * @brief Main simulator runner.
 *
 * @details Creates processes, executes op codes in the processes,
 *          stores output in LogOutput linked list, and outputs to monitor,
 *          file, or both. Supports FCFS-N, array arranged for this by default,
 *          and SJF-N.
 *
 * @param[in] opCodes Head pointer to metadata op codes.
 *                    Should be pointing at S(start)0
 *
 * @param[in] configData Pointer to config data struct.
 */
void runSim(OpCodeType *opCodes, ConfigDataType *configData)
{
    LogOutput *outputHeadPtr = NULL;
    // Because we must initialize the pointer in the main function,
    // we create a placeholder node with value NA for everything.
    // This node will be overrided with a real memory node.
    MemoryNode *memoryHeadPtr = addNewMemoryNode(NULL, NA, NA, NA, NA);

    ProcessControlBlock *headPCBNode = addNewPCB(NULL, NA, NA, NULL);

    char tempStr[MAX_STR_LEN];
    char timeStr[MAX_STR_LEN];

    if (configData->logToCode != LOGTO_MONITOR_CODE)
    {
        outputHeadPtr = createLogFileHeader(configData, outputHeadPtr);
    }

    outputHeadPtr = outputLine(configData->logToCode, outputHeadPtr,
                               "=================\n");
    outputHeadPtr = outputLine(configData->logToCode, outputHeadPtr,
                               "Begin Simulation\n\n");
    accessTimer(ZERO_TIMER, timeStr);
    sprintf(tempStr, "  %s, OS: System Start\n", timeStr);
    outputHeadPtr = outputLine(configData->logToCode, outputHeadPtr, tempStr);

    accessTimer(LAP_TIMER, timeStr);
    sprintf(tempStr, "  %s, OS: Create Process Control Blocks\n", timeStr);
    outputHeadPtr = outputLine(configData->logToCode, outputHeadPtr, tempStr);
    createProcesses(opCodes, configData, headPCBNode);

    accessTimer(LAP_TIMER, timeStr);
    sprintf(tempStr, "  %s, OS: All Processes initialized in NEW state\n", timeStr);
    outputHeadPtr = outputLine(configData->logToCode, outputHeadPtr, tempStr);

    accessTimer(LAP_TIMER, timeStr);
    sprintf(tempStr, "  %s, OS: All Processes now set in READY state\n", timeStr);
    outputHeadPtr = outputLine(configData->logToCode, outputHeadPtr, tempStr);

    ProcessControlBlock *currentProcess;
    while (allProcessesEnded(headPCBNode) != True || waitingQueue(CHECK_QUEUE, NA) == False)
    {
        // No processes ready, but there are processes in the waiting queue
        if (allProcessesEnded(headPCBNode) && waitingQueue(CHECK_QUEUE, NA) == False)
        {
            // accessTimer(LAP_TIMER, timeStr);
            // sprintf(tempStr, "  %s, OS: System/CPU idle\n", timeStr);
            // outputHeadPtr = outputLine(configData->logToCode, outputHeadPtr, tempStr);

            // Loop while there are no interrutps
            while (interruptManager(CHECK_FOR_INTERRUPTS, NA, NA, NULL, NULL, NULL, headPCBNode, NA) == NULL)
            {
            }
            headPCBNode = interruptManager(RESOLVE_INTERRUPTS, NA, NA, NULL, outputHeadPtr, configData, headPCBNode, NA);
        }
        else
        {
            currentProcess = selectNextProcess(headPCBNode, configData->cpuSchedCode);
            // If new, set to ready
            currentProcess->state = READY;
            headPCBNode = runProcess(currentProcess, configData, outputHeadPtr, memoryHeadPtr, headPCBNode);
        }
    }

    accessTimer(LAP_TIMER, timeStr);
    sprintf(tempStr, "  %s, OS: System Stop\n", timeStr);
    outputHeadPtr = outputLine(configData->logToCode, outputHeadPtr, tempStr);

    outputHeadPtr = outputLine(configData->logToCode, outputHeadPtr,
                               "End Simulation - Complete\n");
    outputHeadPtr = outputLine(configData->logToCode, outputHeadPtr,
                               "=========================\n");

    if (configData->logToCode != LOGTO_MONITOR_CODE)
    {
        outputToFile(outputHeadPtr, configData->logToFileName);
    }
    headPCBNode = clearPCBList(headPCBNode);
    outputHeadPtr = clearLogOutput(outputHeadPtr);
    memoryHeadPtr = clearMemoryList(memoryHeadPtr);
}

Boolean allProcessesEnded(ProcessControlBlock *headPtr)
{
    while (headPtr != NULL)
    {
        if ((headPtr->state == READY) || (headPtr->state == NEW))
        {
            return False;
        }
        headPtr = headPtr->next;
    }
    return True;
}

ProcessControlBlock *selectNextProcess(ProcessControlBlock *headPtr, int schedCode)
{
    if (schedCode == CPU_SCHED_SJF_N_CODE || schedCode == CPU_SCHED_SRTF_P_CODE)
    {
        return findMinimum(headPtr, schedCode);
    }
    return findNextReady(headPtr, schedCode);
}

/**
 * @brief Thread runner for run and I/O op code execution.
 *
 * @details Thread runner that calculates the cycle time of the op code
 *          and executes the op code. Stores calculated
 *          cycle time in inputStruct.
 *
 * @param[in] inputStruct Contains a pointer to the configData,
 *                        the log to Code, the op code to be executed,
 *                        the process number of the current process
 *                        in RUNNING state, and the LogOutput linked list.
 */
void *timerRunner(void *inputStruct)
{
    TimerRunnerStruct *structPtr = (TimerRunnerStruct *)inputStruct;

    char *outputStr = structPtr->opCode->opName;
    switch (structPtr->opCode->opLtr)
    {
    case 'I':
        concatenateString(outputStr, " input");
        break;
    case 'O':
        concatenateString(outputStr, " output");
        break;
    }
    double cycleTime = structPtr->configData->ioCycleRate * structPtr->opCode->opValue;

    runTimer(cycleTime);
    structPtr->cycleTime = cycleTime;

    pthread_exit(NULL);
}

void *timerRunnerPremptive(void *inputStruct)
{
    TimerRunnerStruct *structPtr = (TimerRunnerStruct *)inputStruct;
    int processNum = structPtr->processNum;
    pthread_mutex_t *mutexLock = structPtr->mutexLock;

    runTimer(structPtr->cycleTime);

    // critical section
    pthread_mutex_lock(mutexLock);

    // Add interrupt node
    interruptManager(ADD_NODE, NA, processNum, structPtr->outputStr, NULL, NULL, NULL, structPtr->cycleTime);

    pthread_mutex_unlock(mutexLock);

    pthread_exit(NULL);
}

/**
 * @brief Creates PCB objects and stores them in an array.
 *
 * @details Loops through all op codes, creates a ProcessControlBlock
 *          for each A(start) with a pointer to the A(start) op code,
 *          stores each PCB in an array, and calculates the total cycle time
 *          remaining for each process.
 *
 * @param[in] opCodes Head pointer to the op codes linked list.
 *                    Starts at S(start)
 *
 * @param[in] configData Pointer to config data struct
 *
 * @param[out] processArray[] ProcessControlBlock array that stores each process
 *
 * @return none
 */
void createProcesses(OpCodeType *opCodes, ConfigDataType *configData,
                     ProcessControlBlock *headPtr)
{
    int counter = 0;
    ProcessControlBlock *newProcess = NULL;

    while (opCodes != NULL)
    {
        // Skip S(start)
        if ((opCodes->opLtr == 'S') && (compareString(opCodes->opName, "start") == STR_EQ))
        {
            opCodes = opCodes->next;
        }
        else if ((opCodes->opLtr == 'A') && (compareString(opCodes->opName, "start") == STR_EQ))
        {
            // Create PCB and store in linked list
            if ((configData->cpuSchedCode == CPU_SCHED_FCFS_P_CODE) || (configData->cpuSchedCode == CPU_SCHED_FCFS_N_CODE))
            {
                headPtr = addNewPCB(headPtr, counter, counter, opCodes);
                opCodes = opCodes->next;
                newProcess = getLastNode(headPtr);
                counter++;
            }
            else
            {
                headPtr = addNewPCB(headPtr, counter, 0, opCodes);
                opCodes = opCodes->next;
                newProcess = getLastNode(headPtr);
                counter++;
            }
        }
        // Do not add memory op code values to time remaining
        else if (opCodes->opLtr == 'M')
        {
            opCodes = opCodes->next;
        }
        else
        {
            // Add op code cycle time
            newProcess->timeRemaining += calcCycleTime(opCodes, configData);
            newProcess->totalTime += calcCycleTime(opCodes, configData);
            opCodes = opCodes->next;
        }
    }
}

ProcessControlBlock *getLastNode(ProcessControlBlock *head)
{
    while (head != NULL)
    {
        if (head->next == NULL)
        {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

ProcessControlBlock *addNewPCB(ProcessControlBlock *head, int number, int priority,
                               OpCodeType *programCounter)
{
    if (head == NULL)
    {
        head = (ProcessControlBlock *)malloc(sizeof(ProcessControlBlock));
        head->number = number;
        head->priority = priority;
        head->state = NEW;
        head->programCounter = programCounter;
        head->timeRemaining = 0;
        head->totalTime = 0;
        head->next = NULL;
        return head;
    }
    // First node will be a placeholder, so overwrite its values
    // with a real memory node
    else if (head->number == NA)
    {
        (*head).number = number;
        (*head).priority = priority;
        (*head).state = NEW;
        (*head).programCounter = programCounter;
        (*head).timeRemaining = 0;
        (*head).totalTime = 0;
        (*head).next = NULL;
        return head;
    }
    // assume end of list not found yet
    head->next = addNewPCB(head->next, number, priority, programCounter);
    return head;
}

ProcessControlBlock *findNextReady(ProcessControlBlock *headPtr, int schedCode)
{
    while (headPtr != NULL)
    {
        if (headPtr->state == READY || headPtr->state == NEW)
        {
            return headPtr;
        }
        headPtr = headPtr->next;
    }
    return NULL;
}

Boolean waitingQueue(int code, int processNum)
{
    static ProcessControlBlock *headPtr = NULL;

    if (code == CHECK_QUEUE)
    {
        return (headPtr == NULL);
    }
    else if (code == ADD_NODE)
    {
        headPtr = addNewPCB(headPtr, processNum, NA, NULL);
    }
    else if (code == DELETE_NODE)
    {
        ProcessControlBlock *temp = headPtr;
        headPtr = headPtr->next;
        free(temp);
    }
    return True;
}

/**
  * @details Orders processes for SJF. If two processes have the same calculated
  *          time, then they are ordered by FCFS-N.
 *
 * @param[in] processArray[] ProcessControlBlock array that stores each process
 *
 * @param[in] numProcesses Number of processes in processArray
 *
 * @return none
 */
ProcessControlBlock *findMinimum(ProcessControlBlock *headPtr, int schedCode)
{
    ProcessControlBlock *minimum = findNextReady(headPtr, schedCode);
    headPtr = minimum->next;
    while (headPtr != NULL)
    {
        if (schedCode == CPU_SCHED_SJF_N_CODE)
        {
            if ((headPtr->totalTime < minimum->totalTime) && (headPtr->state == READY || headPtr->state == NEW))
            {
                minimum = headPtr;
            }
        }
        else
        {
            if ((headPtr->timeRemaining < minimum->timeRemaining) && (headPtr->state == READY || headPtr->state == NEW))
            {
                minimum = headPtr;
            }
        }
        headPtr = headPtr->next;
    }
    return minimum;
}

/**
 * @brief Method that runs the current process.
 *
 * @details Sets the current process in RUNNING state, prepares the input for
 *          the thread runner method, calls the thread runner method for each
 *          op code until the end of the process (A(end)), subtracts the
 *          time each op code ran from the time remaining, and sets the
 *          current process in EXIT state.
 *
 * @param[in] currentProcess Process to be ran containing a pointer to
 *                           its A(start) node.
 *
 * @param[in] configData Pointer to config data struct
 *
 * @param[out] ouptutPtr Ouptut linked list
 *
 * @return none
 */
ProcessControlBlock *runProcess(ProcessControlBlock *currentProcess, ConfigDataType *configData,
                                LogOutput *ouptutPtr, MemoryNode *memoryPtr, ProcessControlBlock *headPtr)
{
    char tempStr[MAX_STR_LEN];
    char timeStr[MAX_STR_LEN];

    accessTimer(LAP_TIMER, timeStr);
    sprintf(tempStr, "  %s, OS: Process %d selected with %.0f ms remaining.\n",
            timeStr, currentProcess->number, currentProcess->timeRemaining);
    ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);

    accessTimer(LAP_TIMER, timeStr);
    sprintf(tempStr, "  %s, OS: Process %d set in RUNNING state.\n", timeStr,
            currentProcess->number);
    ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);
    currentProcess->state = RUNNING;

    Boolean preemption = True;
    if ((configData->cpuSchedCode == CPU_SCHED_SJF_N_CODE) || (configData->cpuSchedCode == CPU_SCHED_FCFS_N_CODE))
    {
        preemption = False;
    }

    char *opCodeName = currentProcess->programCounter->opName;
    while ((currentProcess->programCounter != NULL) && (compareString(opCodeName, "end") != STR_EQ))
    {
        // Skip A(start)
        if (compareString(opCodeName, "start") == STR_EQ)
        {
            currentProcess->programCounter = currentProcess->programCounter->next;
            opCodeName = currentProcess->programCounter->opName;
        }
        else if (currentProcess->programCounter->opLtr == 'M')
        {
            int returnVal = memoryManager(*currentProcess, configData, ouptutPtr,
                                          memoryPtr);
            if (returnVal == SEG_FAULT)
            {
                accessTimer(LAP_TIMER, timeStr);
                sprintf(tempStr, "  %s, Process: %d, experiences segmentation fault.\n",
                        timeStr, currentProcess->number);
                ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);
                break;
            }
            currentProcess->programCounter = currentProcess->programCounter->next;
            opCodeName = currentProcess->programCounter->opName;
        }
        else if (currentProcess->programCounter->opLtr == 'P')
        {
            // No preemption
            if (!preemption)
            {
                accessTimer(LAP_TIMER, timeStr);
                sprintf(tempStr, "\n  %s, Process: %d, run operation start\n", timeStr, currentProcess->number);
                ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);

                int cycleTime = calcCycleTime(currentProcess->programCounter, configData);
                runTimer(cycleTime);
                currentProcess->timeRemaining -= cycleTime;

                accessTimer(LAP_TIMER, timeStr);
                sprintf(tempStr, "  %s, Process: %d, run operation end\n", timeStr, currentProcess->number);
                ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);

                currentProcess->programCounter = currentProcess->programCounter->next;
                opCodeName = currentProcess->programCounter->opName;
            }
            //Preemption
            else
            {
                accessTimer(LAP_TIMER, timeStr);
                sprintf(tempStr, "\n  %s, Process: %d, run operation start\n", timeStr, currentProcess->number);
                ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);

                int remainingQuantum = configData->quantumCycles;
                while ((remainingQuantum > 0) && (currentProcess->programCounter->opValue > 0) && (interruptManager(CHECK_FOR_INTERRUPTS, NA, NA, NULL, NULL, NULL, headPtr, NA) == NULL))
                {
                    runTimer(configData->procCycleRate);
                    (currentProcess->programCounter->opValue)--;
                    remainingQuantum--;
                    currentProcess->timeRemaining = currentProcess->timeRemaining - configData->procCycleRate;
                }

                // If the run operation ended
                if (currentProcess->programCounter->opValue == 0)
                {
                    accessTimer(LAP_TIMER, timeStr);
                    sprintf(tempStr, "  %s, Process: %d, run operation end\n", timeStr, currentProcess->number);
                    ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);

                    accessTimer(LAP_TIMER, timeStr);
                    sprintf(tempStr, "\n  %s, Process: %d, set in READY state\n", timeStr, currentProcess->number);
                    ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);
                    currentProcess->state = READY;

                    currentProcess->programCounter = currentProcess->programCounter->next;
                    opCodeName = currentProcess->programCounter->opName;
                    // break out of loop?
                    // Append process to PCB list if RR-P
                    if (configData->cpuSchedCode == CPU_SCHED_RR_P_CODE)
                    {
                        headPtr = reorderPCBList(headPtr, currentProcess->number);
                    }
                    return headPtr;
                }

                // If we broke out of the loop and there is an interrupt to resolve
                if (interruptManager(CHECK_FOR_INTERRUPTS, NA, NA, NULL, NULL, NULL, headPtr, NA) != NULL)
                {
                    // Resolve interrupts
                    headPtr = interruptManager(RESOLVE_INTERRUPTS, currentProcess->number, NA, NULL, ouptutPtr, configData, headPtr, NA);

                    // Append process to PCB list if RR-P
                    if (configData->cpuSchedCode == CPU_SCHED_RR_P_CODE)
                    {
                        headPtr = reorderPCBList(headPtr, currentProcess->number);
                    }
                    return headPtr;
                }
            }
        }
        else if ((currentProcess->programCounter->opLtr == 'I') || (currentProcess->programCounter->opLtr == 'O'))
        {
            // Create thread variables
            pthread_t threadID;
            pthread_attr_t threadAttr;
            pthread_attr_init(&threadAttr);
            pthread_mutex_t mutexLock;
            pthread_mutex_init(&mutexLock, NULL);

            // Create thread input struct
            TimerRunnerStruct threadInput;
            threadInput.configData = configData;
            threadInput.processNum = currentProcess->number;
            threadInput.mutexLock = &mutexLock;
            threadInput.opCode = currentProcess->programCounter;

            char opString[MAX_STR_LEN];
            copyString(opString, currentProcess->programCounter->opName);
            switch (currentProcess->programCounter->opLtr)
            {
            case 'I':
                concatenateString(opString, " input");
                break;
            case 'O':
                concatenateString(opString, " output");
                break;
            }
            accessTimer(LAP_TIMER, timeStr);
            sprintf(tempStr, "\n  %s, Process: %d, %s start\n", timeStr, currentProcess->number, opString);
            ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);

            // Run thread
            if (!preemption)
            {
                pthread_create(&threadID, &threadAttr, timerRunner, &threadInput);
                pthread_join(threadID, NULL);
                currentProcess->timeRemaining -= threadInput.cycleTime;

                accessTimer(LAP_TIMER, timeStr);
                sprintf(tempStr, "  %s, Process: %d, %s end\n", timeStr, currentProcess->number, opString);
                ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);

                currentProcess->programCounter = currentProcess->programCounter->next;
                opCodeName = currentProcess->programCounter->opName;
            }
            else
            {
                sprintf(tempStr, "Process: %d, %s end\n\n", currentProcess->number, opString);
                copyString(threadInput.outputStr, tempStr);
                double cycleTime = calcCycleTime(currentProcess->programCounter, configData);
                threadInput.cycleTime = cycleTime;

                pthread_create(&threadID, &threadAttr, timerRunnerPremptive, &threadInput);
                currentProcess->timeRemaining = currentProcess->timeRemaining - cycleTime;

                accessTimer(LAP_TIMER, timeStr);
                sprintf(tempStr, "\n  %s, OS: Process %d set in BLOCKED state.\n", timeStr,
                        currentProcess->number);
                ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);
                currentProcess->state = BLOCKED;
                waitingQueue(ADD_NODE, currentProcess->number);
                currentProcess->programCounter = currentProcess->programCounter->next;
                return headPtr;
            }
        }
    }

    accessTimer(LAP_TIMER, timeStr);
    sprintf(tempStr, "  %s, OS: Process %d ended and set in EXIT state.\n",
            timeStr, currentProcess->number);
    ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);
    currentProcess->state = EXIT;
    return headPtr;
}

// Append PCB to end of list for RR-P
ProcessControlBlock *reorderPCBList(ProcessControlBlock *headPtr, int processNum)
{
    ProcessControlBlock *temp = headPtr;
    ProcessControlBlock *foundPCB = NULL;
    ProcessControlBlock *prev = headPtr;

    while (temp != NULL)
    {
        if (temp->number == processNum)
        {
            foundPCB = temp;

            // If foundPCB is the first in the list
            if (prev->number == foundPCB->number)
            {
                headPtr = foundPCB->next;
                foundPCB->next = NULL;
                break;
            }
            // If foundPCB is in the middle of the list
            else if (foundPCB->next != NULL)
            {
                //                headPtr = foundPCB->next;
                prev->next = foundPCB->next;
                foundPCB->next = NULL;
                break;
            }
            // If foundPCB is at end of list
            // We do not need to reorder
            else
            {
                temp = NULL;
            }
        }
        else
        {
            prev = temp;
            temp = temp->next;
        }
    }
    // Append to end of list if foundPCB wasn't already at the end
    if (temp != NULL)
    {
        temp = getLastNode(headPtr);
        temp->next = foundPCB;
    }
    return headPtr;
}

ProcessControlBlock *interruptManager(int interruptCode, int interruptedProcess, int processNum,
                                      char *outputString, LogOutput *ouptutPtr, ConfigDataType *configData, ProcessControlBlock *headPtr, int cycleTime)
{
    char tempStr[MAX_STR_LEN];
    char timeStr[MAX_STR_LEN];
    static InterruptNode *interruptHead = NULL;

    if (interruptCode == CHECK_FOR_INTERRUPTS)
    {
        if (interruptHead == NULL)
        {
            return NULL;
        }
        else
        {
            // Return headPtr because we need to return a PCB and this will tell
            // the calling the method that the interruptHead is not NULL
            return headPtr;
        }
    }
    else if (interruptCode == ADD_NODE)
    {
        interruptHead = addNewInterruptNode(interruptHead, processNum, cycleTime, outputString);
    }
    else
    {
        while (interruptHead != NULL)
        {
            InterruptNode *temp = interruptHead;

            // If CPU is idle
            if (interruptedProcess == NA)
            {
                accessTimer(LAP_TIMER, timeStr);
                sprintf(tempStr, "  %s, OS: Interrupt called by process %d.\n",
                        timeStr, interruptHead->processNum);
                ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);
            }
            // If process was interrupted
            else
            {
                accessTimer(LAP_TIMER, timeStr);
                sprintf(tempStr, "  %s, OS: Process %d interrupted by process %d.",
                        timeStr, interruptedProcess, interruptHead->processNum);
                accessTimer(LAP_TIMER, timeStr);
                ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);

                sprintf(tempStr, "\n  %s, OS: Process %d put in READY state.\n\n",
                        timeStr, interruptedProcess);
                ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);
                findPCB(headPtr, interruptedProcess)->state = READY;
            }

            accessTimer(LAP_TIMER, timeStr);
            sprintf(tempStr, "  %s, %s", timeStr, interruptHead->outputStr);
            ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);

            accessTimer(LAP_TIMER, timeStr);
            sprintf(tempStr, "  %s, OS: Process %d put in READY state.\n",
                    timeStr, interruptHead->processNum);
            ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);

            // Subtract cycle time and set process to ready
            ProcessControlBlock *process = findPCB(headPtr, interruptHead->processNum);
            // process->timeRemaining = process->timeRemaining - interruptHead->cycleTime;
            process->state = READY;

            // Append process to end of PCB list if RR-P
            if (configData->cpuSchedCode == CPU_SCHED_RR_P_CODE)
            {
                headPtr = reorderPCBList(headPtr, interruptHead->processNum);
            }

            waitingQueue(DELETE_NODE, NA);
            interruptHead = interruptHead->next;
            free(temp);
        }
        return headPtr;
    }
    return NULL;
}

ProcessControlBlock *findPCB(ProcessControlBlock *headPtr, int processNum)
{
    while (headPtr != NULL)
    {
        if (headPtr->number == processNum)
        {
            return headPtr;
        }
        headPtr = headPtr->next;
    }
    return NULL;
}

ProcessControlBlock *clearPCBList(ProcessControlBlock *head)
{
    if (head != NULL)
    {
        if (head->next != NULL)
        {
            clearPCBList(head->next);
        }
        free(head);
    }
    return NULL;
}

InterruptNode *addNewInterruptNode(InterruptNode *head, int processNum, int cycleTime, char *outputString)
{
    if (head == NULL)
    {
        head = (InterruptNode *)malloc(sizeof(InterruptNode));
        copyString(head->outputStr, outputString);
        head->processNum = processNum;
        head->cycleTime = cycleTime;
        head->next = NULL;
        return head;
    }
    // Do not add an interrupt for the same process if one already exists
    // Was getting weird behavior with threads where one thread was
    // creating multiple interrupts
    // if(head->processNum == processNum)
    // {
    //     return head;
    // }
    // assume end of list not found yet
    head->next = addNewInterruptNode(head->next, processNum, cycleTime, outputString);

    return head;
}

/**
 * @brief Handles memory allocations and accesses
 *
 * @details Runs allocate and access op codes, throws seg faults, and adds
 *          ouptut lines to outputPtr.
 *
 * @param[in] process Current process running
 *
 * @param[in] configData Pointer to config data struct
 *
 * @param[out] ouptutPtr Ouptut linked list
 *
 * @param[out] memoryPtr Memory linked list
 *
 * @return int Returns SEG_FAULT or NO_ERROR
 */
int memoryManager(ProcessControlBlock process, ConfigDataType *configData,
                  LogOutput *ouptutPtr, MemoryNode *memoryPtr)
{
    char tempStr[MAX_STR_LEN];
    char timeStr[MAX_STR_LEN];

    int identifier = process.programCounter->opValue / 1000000;
    int base = (process.programCounter->opValue / 1000) % 1000;
    int offset = process.programCounter->opValue % 1000;

    if (compareString(process.programCounter->opName, "allocate") == STR_EQ)
    {
        accessTimer(LAP_TIMER, timeStr);
        sprintf(tempStr, "  %s, Process: %d, MMU attempt to allocate %d/%d/%d.\n",
                timeStr, process.number, identifier, base, offset);
        ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);

        // Convert config data memory available to MB and compare it
        // to the base of the allocation
        if ((!memorySpaceFree(memoryPtr, base, process.number, identifier)) || (base > configData->memAvailable / 1024))
        {
            accessTimer(LAP_TIMER, timeStr);
            sprintf(tempStr, "  %s, Process: %d, MMU failed to allocate.\n",
                    timeStr, process.number);
            ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);
            return SEG_FAULT;
        }
        // Allocation successful
        memoryPtr = addNewMemoryNode(memoryPtr, identifier, base, offset,
                                     process.number);
        accessTimer(LAP_TIMER, timeStr);
        sprintf(tempStr, "  %s, Process: %d, MMU successful allocate.\n",
                timeStr, process.number);
        ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);
    }
    else if (compareString(process.programCounter->opName, "access") == STR_EQ)
    {
        accessTimer(LAP_TIMER, timeStr);
        sprintf(tempStr, "  %s, Process: %d, MMU attempt to access %d/%d/%d.\n",
                timeStr, process.number, identifier, base, offset);
        ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);

        if (!requestMemory(memoryPtr, process.number, identifier, base, offset))
        {
            accessTimer(LAP_TIMER, timeStr);
            sprintf(tempStr, "  %s, Process: %d, MMU failed to access.\n",
                    timeStr, process.number);
            ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);
            return SEG_FAULT;
        }
        // Access successful
        accessTimer(LAP_TIMER, timeStr);
        sprintf(tempStr, "  %s, Process: %d, MMU successful access.\n",
                timeStr, process.number);
        ouptutPtr = outputLine(configData->logToCode, ouptutPtr, tempStr);
    }
    return NO_ERR;
}

/**
 * @details Checks if memory space is already allocated and if the memory
 *           identifier has already been used for the process running.
 *
 * @param[in] memoryPtr Memory head pointer
 *
 * @param[in] memoryBase Memory size requesting to be allocated.
 *
 * @param[in] processNum Process requesting memory
 *
 * @param[in] memoryIdentifier Memory node identifier
 *
 * @return Boolean Returns True memory space is free. Returns False
 *                 if memory space has already been allocated.
 */
Boolean memorySpaceFree(MemoryNode *memoryPtr, int memoryBase, int processNum,
                        int memoryIdentifier)
{
    if (memoryPtr == NULL)
    {
        return True;
    }
    else if ((memoryPtr->base == memoryBase) || (memoryPtr->processNum == processNum && memoryPtr->identifier == memoryIdentifier))
    {
        return False;
    }
    return memorySpaceFree(memoryPtr->next, memoryBase, processNum, memoryIdentifier);
}

/**
 * @details Checks if allocated is called before access and if the
 *          requested memory is not larger than the memory allocated.
 *          Finds requested memory matching the process number, base,
 *          and identifier.
 *
 * @param[in] memoryPtr Memory head pointer
 *
 * @param[in] processNum Process that is currently running
 *
 * @param[in] identifier Memory node identifier
 *
 * @param[in] base Memory node base
 *
 * @param[in] offset Memory node offset
 *
 * @return Boolean Returns True if requested memory is smaller than allocated
 *                 memory. Returns False if access called before allocate or
 *                 there is not enough memory allocated for requested memory.
 */
Boolean requestMemory(MemoryNode *memoryPtr, int processNum, int identifier,
                      int base, int offset)
{ // If requested memory has not been found or we have not added
    // any nodes to the memory list besides the placeholder node.
    if (memoryPtr == NULL || memoryPtr->identifier == NA)
    {
        return False;
    }
    else if ((memoryPtr->processNum == processNum) && (memoryPtr->base == base) && (memoryPtr->identifier == identifier))
    {
        if (offset <= memoryPtr->offset)
        {
            return True;
        }
        else
        {
            return False;
        }
    }
    return requestMemory(memoryPtr->next, processNum, identifier, base, offset);
}

/**
 * @brief Creates new memory node.
 *
 * @details Checks if the local head pointer is NULL. If so, creates
 *          new node to be the head pointer. If not, adds new node to end of
 *          linked list.
 *
 * @param[in] head MemoryNode head pointer
 *
 * @param[in] memoryID New node identifier
 *
 * @param[in] memoryBase New node base
 *
 * @param[in] memoryOffset New node offset
 *
 * @param[in] processNum Current process running
 *
 * @return MemoryNode Head of linked list
 */
MemoryNode *addNewMemoryNode(MemoryNode *head, int memoryID, int memoryBase,
                             int memoryOffset, int processNum)
{
    if (head == NULL)
    {
        head = (MemoryNode *)malloc(sizeof(MemoryNode));
        head->processNum = processNum;
        head->identifier = memoryID;
        head->base = memoryBase;
        head->offset = memoryOffset;
        head->next = NULL;
        return head;
    }
    // First node will be a placeholder, so overwrite its values
    // with a real memory node
    else if (head->processNum == NA)
    {
        (*head).processNum = processNum;
        (*head).identifier = memoryID;
        (*head).base = memoryBase;
        (*head).offset = memoryOffset;
        (*head).next = NULL;
        return head;
    }
    // assume end of list not found yet
    head->next = addNewMemoryNode(head->next, memoryID, memoryBase,
                                  memoryOffset, processNum);
    return head;
}

/**
 * @brief Clears memory linked list.
 *
 * @details Recursively iterates through MemoryNode linked list and
           returns memory to OS from the bottom of the list upward.
 *
 * @param[in] head Memory list head pointer
 *
 * @return MemoryNode Head pointer, which should be NULL
 */
MemoryNode *clearMemoryList(MemoryNode *head)
{
    if (head != NULL)
    {
        if (head->next != NULL)
        {
            clearMemoryList(head->next);
        }
        free(head);
    }
    return NULL;
}

/**
 * @details Calculates the cycle time for an op code by checking if the op code
 *          is a run or I/O code and multiplying by the respective rate.
 *
 * @param[in] opCode Pointer to current op code
 *
 * @param[in] configData Pointer to config data struct
 *
 * @return double The calculated cycle time
 */
double calcCycleTime(OpCodeType *opCode, ConfigDataType *configData)
{
    switch (opCode->opLtr)
    {
    case 'P':
        return opCode->opValue * configData->procCycleRate;
    default:
        return opCode->opValue * configData->ioCycleRate;
    }
}

/**
 * @details Handles output depending on logToCode.
 *          If logToCode is MONITOR, prints string.
 *          If logToCode is FILE, saves string to buffer.
 *          If logToCode is BOTH, prints and saves string to buffer.
 *          Note: the string is always added to the buffer to not mess up the
 *          output head pointer, but only outputted to file at end if
 *          logToCode is FILE or BOTH.
 *
 * @param[in] logToCode Config data log to code
 *
 * @param[out] ouptutPtr Ouptut linked list
 *
 * @param[in] inputStr string to be outputted
 *
 * @return LogOutput Head of linked list
 */
LogOutput *outputLine(int logToCode, LogOutput *ouptutPtr, char *inputStr)
{
    ouptutPtr = addNewStrNode(ouptutPtr, inputStr);

    if (logToCode != LOGTO_FILE_CODE)
    {
        printf("%s", inputStr);
    }
    return ouptutPtr;
}

/**
 * @details Outputs buffer to the log file.
 *
 * @param[out] ouptutPtr Ouptut linked list
 *
 * @param[in] outputFileName The name of the output log file
 *
 * @return none
 */
void outputToFile(LogOutput *ouptutPtr, char *outputFileName)
{
    FILE *outputFile = fopen(outputFileName, "w");

    while (ouptutPtr != NULL)
    {
        fprintf(outputFile, "%s", ouptutPtr->outputStr);
        ouptutPtr = ouptutPtr->next;
    }
    fclose(outputFile);
}

/**
 * @details Creates log file header when log to code is FILE or BOTH
 *
 * @param[in] configData Pointer to config data struct
 *
 * @param[out] ouptutPtr Ouptut linked list
 *
 * @return LogOutput Head of linked list
 */
LogOutput *createLogFileHeader(ConfigDataType *configData, LogOutput *ouptutPtr)
{
    char tempStr[MAX_STR_LEN];

    ouptutPtr = addNewStrNode(ouptutPtr, "==========================");
    ouptutPtr = addNewStrNode(ouptutPtr, "=======================\n");
    ouptutPtr = addNewStrNode(ouptutPtr, "Simulator Log File Header\n\n");

    sprintf(tempStr, "File Name                       : %s\n",
            configData->metaDataFileName);

    ouptutPtr = addNewStrNode(ouptutPtr, tempStr);

    char cpuSchedCode[MAX_STR_LEN];
    configCodeToString(configData->cpuSchedCode, cpuSchedCode);
    sprintf(tempStr, "CPU Scheduling                  : %s\n", cpuSchedCode);
    ouptutPtr = addNewStrNode(ouptutPtr, tempStr);

    sprintf(tempStr, "Quantum Cycles                  : %d\n",
            configData->quantumCycles);
    ouptutPtr = addNewStrNode(ouptutPtr, tempStr);

    sprintf(tempStr, "Memory Available (KB)           : %d\n",
            configData->memAvailable);
    ouptutPtr = addNewStrNode(ouptutPtr, tempStr);

    sprintf(tempStr, "Processor Cycle Rate (ms/cycle) : %d\n",
            configData->procCycleRate);
    ouptutPtr = addNewStrNode(ouptutPtr, tempStr);

    sprintf(tempStr, "I/O Cycle Rate (ms/cycle)       : %d\n\n",
            configData->ioCycleRate);
    ouptutPtr = addNewStrNode(ouptutPtr, tempStr);
    return ouptutPtr;
}

/**
 * @brief Creates new LogOutput node.
 *
 * @details Checks if the local head pointer is NULL. If so, creates
 *          new node to be the head pointer. If not, adds new node to end of
 *          linked list.
 *
 * @param[in] head LogOutput head pointer
 *
 * @param[in] logString Ouptut string for new node
 *
 * @return LogOutput Head of linked list
 */
LogOutput *addNewStrNode(LogOutput *head, char *logString)
{
    if (head == NULL)
    {
        head = (LogOutput *)malloc(sizeof(LogOutput));
        copyString(head->outputStr, logString);
        head->next = NULL;
        return head;
    }
    // assume end of list not found yet
    head->next = addNewStrNode(head->next, logString);

    return head;
}

/**
 * @brief Clears LogOutput linked list.
 *
 * @details Recursively iterates through LogOutput linked list and
           returns memory to OS from the bottom of the list upward.
 *
 * @param[in] head LogOutput head pointer
 *
 * @return LogOutput Head pointer, which should be NULL
 */
LogOutput *clearLogOutput(LogOutput *head)
{
    if (head != NULL)
    {
        if (head->next != NULL)
        {
            clearLogOutput(head->next);
        }
        free(head);
    }
    return NULL;
}
