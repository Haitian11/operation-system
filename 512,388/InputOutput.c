// Code Implementation Information ///////////
/**
  * @file InputOutput.c
  *
  * @brief Program data for all I/O functionality in Simulator, as well as
	* 			string manipulation
  *
  * @details Implements all standard String utility functions
  *
  * @version 1.00
  *          Anonymous (2019 January 31)
  *          Inital Development and testing
  *
  * @note Requires StringUtils.h
  * @note Author redacted for grading integrity
  */

#include <stddef.h>
#include "InputOutput.h"
#include <stdbool.h>


// External Literal Constants /////////////////////////////////////////////////
//const int MAX_STR_LEN = 256;
// External char Literal Constants ////////////////////////////////////////////
const char NULL_CHAR = '\0';
const char UPPER_CASE_A = 'A';
const char UPPER_CASE_Z = 'Z';
const char LOWER_CASE_A = 'a';
const char LOWER_CASE_Z = 'z';
const char NEWLINE_CHAR = '\n';
const char SPACE = ' ';
const char COLON = ':';
const char SEMI_COLON = ';';
const char WRITE_ACCESS_POINTER = 'w';
const char OPEN_PARENTHESIS = '(';
const char CLOSED_PARENTHESIS = ')';

// Output Functions ///////////////////////////////////////////////////////////
void addToLog( char* string, System* system )
{
    LogQueue* newMessage = malloc(sizeof(LogQueue) );
    copyString(string, newMessage->message );
    if (system->log.logFirst == NULL)
    {
        system->log.logFirst = newMessage;
    }
    else
    {
        system->log.logLast->next = newMessage;
    }
    system->log.logLast = newMessage;
} // End addToLog

void logOut(System* system)
{
    LogQueue* logQueue = system->log.logFirst;
    LogQueue* logNext;
    char logMessage[MAX_STR_LEN];
    FILE* filePointer  = fopen( system->log.logToFileName, &WRITE_ACCESS_POINTER );
    int errorFlag = 0;
    LOG_CODE logCode = system->log.logToCode ;
    while ( logQueue != NULL )
    {
        nukeString(logMessage, MAX_STR_LEN);
        copyString(logQueue->message, logMessage);
        if ( logCode != FILE_LOG )
        {
            printf(logMessage);
        } //end ! file logged
        if ( logCode != MONITOR_LOG )
        {
            errorFlag = fputs(logMessage, filePointer);
            if ( errorFlag < 0 )
            {
                errorHandeler(FILE_WRITE_ERROR, false);
                if (logCode == BOTH_LOG)
                {
                    system->log.logToCode = MONITOR_LOG;
                }
                else
                {
                    errorHandeler(LOG_ERROR, true);
                }
            } //Attempt to recover from file write error
        } //end !MONITOR_LOG
        logNext = logQueue->next;
        free(logQueue);
        logQueue = logNext;
    } //end while logQueue not empty
} //end logOut

void errorHandeler(int errorCode, bool fatal )
{
    switch ( errorCode )
    {
    case FILE_READ_ERROR:
        printf("Fatal: Config File Read Error.\n");
        break;
    case INVALID_FILE_TYPE:
        printf("Fatal: Invalid Config File.\n");
        break;
    case INVALID_DATA_VALUE:
        printf("Fatal: Invalid data line in config.\n");
        break;
    case INSUFFICENT_MEMORY:
        printf("Fatal: Insufficent memory to create simulator.\n");
        break;
    case FILE_WRITE_ERROR:
        printf("Error: Can not write to log file.\n");
        break;
    case META_FILE_ERROR:
        printf("Fatal: MetaData File Read Errror.\n");
        break;
    case META_READ_ERROR:
        printf("Fatal: Invalid MetaData File.\n");
        break;
    default:
        printf("Unknown error caught.  This should be imposible.\n");
    } //End switch

    if ( fatal )
	{
	    exit(1);
	}
} // End errorHandeler

// String Utility Functions ///////////////////////////////////////////////////
int getStringLength( char* inStr )
{
	int index = 0;
    if( inStr != NULL )
	{
		while ( index < MAX_STR_LEN && inStr[ index ] != NULL_CHAR )
		{
			index++;
		}
		return index;
	}
	return NULL_STRING_ERROR;
} // End getStringLength

void copyString( char* orginString, char* cloneString )
{
	int iter = 0;
    nukeString(cloneString, MAX_STR_LEN );
	if ( orginString != NULL &&
	        cloneString != NULL &&
		        orginString != cloneString )
	{
		do
		{
			cloneString[ iter ] = orginString [ iter ];
			iter++;
		}
		while ( orginString[ iter - 1 ] != NULL_CHAR &&
		         iter < MAX_STR_LEN );
	}
} //end copyString

int compareString( char* stringA, char* stringB )
{
	int iter = 0;
	int compare = 0;
	while ( iter < MAX_STR_LEN &&
	            stringA[ iter ] != NULL_CHAR &&
				     stringB[ iter ] != NULL_CHAR )
	{
		compare = stringA[ iter ] - stringB[ iter ];
		if( compare != 0 )
		{
			return compare;
		}
		iter++;
	}
	if ( stringA[ iter ] == NULL_CHAR )
	{
		compare--;
	}
	if ( stringB[ iter ] == NULL_CHAR )
	{
		compare++;
	}
	return compare; //temp
} //end compareString

void concatanateString( char* destString, char* addString )
{
    int addIter = 0;
    int destIter = getStringLength( destString );
    int addLength = getStringLength( addString );

    while ( addIter < addLength )
    {
        destString[ destIter ] = addString[ addIter ];
        destIter++;
        addIter++;
    }
}

void getSubstring( char* destString, char* sourceString, int startIndex,
                                                            int endIndex )
{
	int strLength;
	int iter = 0;
	int index;
	char workingString[MAX_STR_LEN];

	strLength = getStringLength( sourceString );

	if ( endIndex >= strLength || startIndex >= endIndex || startIndex < 0 )
	{
		return;
	}

	copyString( sourceString, workingString );
	for ( index = startIndex; index < endIndex; index++ )
	{
		destString[ iter ] = workingString[ index ];
		iter++;
	}
	destString[ iter ] = NULL_CHAR;
    destString[ iter + 1 ] = NULL_CHAR;
}  //End getSubstring

int findSubstring( char* testString, char* searchString )
{
	int searchIndex = 0;
	int testIndex = 0;
	int returnIndex = SUBSTRING_NOT_FOUND;


	while( searchString[ searchIndex ] != NULL_CHAR &&
                searchIndex < MAX_STR_LEN &&
				    testString[ testIndex ] != NULL_CHAR )
	{
		if( searchString[ searchIndex ] == testString[ testIndex ] )
		{
			if( testIndex == 0 )
			{
				returnIndex = searchIndex;
			}
			testIndex++;
		}
		else if( testIndex != 0 ){
			testIndex = 0;
			returnIndex = SUBSTRING_NOT_FOUND;
		}
		searchIndex++;
	}

	return returnIndex;
} //End findSubstring

void toLowerCase( char* destString, char* sourceString )
{
	int strLength;
	int iter;
	char* workingString;
	char currentChar;

	strLength = getStringLength( sourceString );

	workingString = (char*)malloc( sizeof(char) * strLength );
	if ( workingString == NULL )
	{
		return;
	}

	copyString( sourceString, workingString );

	for ( iter = 0; iter < strLength; iter++ )
	{
		currentChar = workingString[ iter ];
		if ( currentChar >= UPPER_CASE_A && currentChar <= UPPER_CASE_Z )
		{
			currentChar = currentChar - UPPER_CASE_A + LOWER_CASE_A;
		}
		destString[ iter ] = currentChar;
	}
}

void toUpperCase( char* destString, char* sourceString )
{
	int strLength;
	int iter;
	char* workingString;
	char currentChar;

	strLength = getStringLength( sourceString );

	workingString = (char*)malloc( sizeof(char) * strLength );
	if ( workingString == NULL )
	{
		return;
	}

	copyString( sourceString, workingString );


	for ( iter = 0; iter < strLength; iter++ )
	{
		currentChar = workingString[ iter ];
		if ( currentChar >= LOWER_CASE_A && currentChar <= LOWER_CASE_Z )
		{
			currentChar = currentChar - LOWER_CASE_A + UPPER_CASE_A;
		}
		destString[ iter ] = currentChar;
	}
}

int getLineTo( FILE *filePointer, int bufferSize, char stopChar, char* outputBuffer )
{
	int charIndex = 0;
	int charAsInt;

	do
	{
		if ( feof( filePointer )  != 0 )
		{
			return INCOMPLETE_FILE_ERROR;
		}
		charAsInt = fgetc( filePointer );
		outputBuffer[charIndex] = (char) charAsInt;
		charIndex++;
	} while ( charAsInt != (int) stopChar && charIndex < bufferSize );
	return 0;
}

//Yes, I know Valgrind doesn't like this.  It likes not having this a LOT less.
void nukeString(char* stringIn, int length)
{
   for( int index = 0; index < length; index++ )
   {
        stringIn[ index ] = NULL_CHAR;
   }
}

void checkNewLines(FILE *filePointer)
{
    int newline = (int)(NEWLINE_CHAR);
    int charAsInt;
    charAsInt = fgetc( filePointer );
    if(charAsInt == newline)
    {
        return;
    }
    ungetc(charAsInt, filePointer);
}

void checkSpace(FILE *filePointer)
{
    int space = (int)(SPACE);
    int charAsInt;
    charAsInt = fgetc( filePointer );
    if(charAsInt == space)
    {
        return;
    }
    ungetc(charAsInt, filePointer);
}
