// Header File Information //////////
/**
  * @file StringUtils.h
  *
  * @brief Header file for StringUtils.c_str
  *
  * @details Specifies constants and functions for StringUtils.c_str
  *
  * @version 2.00
  *          Anonymous 2019 August 27
  *          Renamed to InputOutput.h
  *          Added input and output function sections
  * @version 1.00
  *          Anonymous 2019 January 31
  *          Inital development and testing
  *
  * @note Author redacted for grading integrity
  */

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include "System.h"
#include "Codes.h"




// Internal Constants
#define MAX_STR_LEN 255

extern const char NULL_CHAR;// = '\0'
extern const char NEWLINE_CHAR;// = '\n'
extern const char UPPER_CASE_A;// = 'A'
extern const char UPPER_CASE_Z;// = 'Z'
extern const char LOWER_CASE_A;// = 'a'
extern const char LOWER_CASE_Z;// = 'z'
extern const char SPACE;// = ' '
extern const char COLON;// = ':'
extern const char SEMI_COLON;// = ';'
extern const char OPEN_PARENTHESIS; // = '('
extern const char CLOSED_PARENTHESIS; // = ')'

typedef struct
{
    char* logMessage;
    int strLength;
}
LogMessage;

// Output Functions ///////////////////////////////////////////////////////////

/**
  * addToLog : adds string to the end of the log queue
  */
void addToLog( char* string, System* system );

/**
  * errorHandeler : Records error and exits program if fatal
  *
  * @pre: errorCode valid ERROR_CODE enum
  */
void errorHandeler(int errorCode, bool fatal );

/**
  * logOut : takes in all logged messages and dumps them to log location
  */
void logOut(System* system);

// String Utility Functions ///////////////////////////////////////////////////

/**
  * getStrLength : returns the length of a given string
  *
  * @pre: String formated C-style ( ending with a NULL_CHAR )
  *
  * @post: Returns length of string.
  * @post: If longer than MAX_STR_LEN, returns MAX_STR_LEN.
  * @post: If inStr is NULL, returns ERROR_FLAG
  */
int getStringLength( char* inStr );

/**
  * copyString : copys a given string into an "empty" array
  *
  * @pre: String formated C-style ( ending with a NULL_CHAR )
  * @pre: Destination array contains enough space to accept inputString
  *
  * @post: clonedString contains exact copy of inputString
  *
  * @note: Limited to MAX_STR_LEN for safety
  */
void copyString( char* inputString, char* clonedString );

/**
  * compareString : compares the length of two strings alphabetically
  *
  * @pre: Strings formated C-style ( ending with a NULL_CHAR )
  *
  * @post: if stringA > stringB, result < 0
  * @post: if stringA < stringB, result > 0
  * @post: if stringA = stringB, result = 0
  *
  * @note: Limited to MAX_STR_LEN for safety
  */
int compareString( char* stringA, char* stringB );

/**
  * concatanateString : appends addString onto the end of destString
  *
  * @pre: Strings formated C-style ( ending with a NULL_CHAR )
  *
  * @post: Strings concatanated
  *
  * @note: Limited to MAX_STR_LEN for safety
  */
void concatanateString( char* destString, char* addString );

/**
  * getSubstring : captures portion of string and returns it into destString
  *
  * @pre: String formated C-style ( ending with a NULL_CHAR )
  * @pre: destString contains enough space to accept substring
  *
  * @post: substring returned in destString pointer
  * @post: empty string returned on error ( out of bounds/ NULL pointer )
  *
  * @note: internal copy created for protection.
  */
void getSubstring( char* destString, char* sourceString, int startIndex,
                                                            int endIndex );

/**
  * findSubstring : locates the starting index of a contained substring
  *
  * @pre: Strings formated C-style ( ending with a NULL_CHAR )
  *
  * @post: returns starting index of contained testString
  * @post: returns SUBSTRING_NOT_FOUND if substring not found
  */
int findSubstring( char* testString, char* searchString );

/**
  * toLowerCase : converts all upper case characters to lower case
  *
  * @pre: String formated C-style ( ending with a NULL_CHAR )
  *
  * @post: all upper cars characters converted to lower case.
  * @post: no impact on any other characters
  *
  * @note: internal copy created for protection.
  */
void toLowerCase( char* destString, char* sourceString );

/**
  * toUpperCase : converts all lower case characters to upper case
  *
  * @pre: String formated C-style ( ending with a NULL_CHAR )
  *
  * @post: all lower cars characters converted to upper case.
  * @post: no impact on any other characters
  *
  * @note: internal copy created for protection.
  */
void toUpperCase( char* destString, char* sourceString );

/**
  * getLineTo : returns everything in filePointer till stopChar
  *
  * @pre: file ends with EoF symbol
  *
  * @post: all characters to stopChar are returned as a single string        OR
  * @post: bufferSize characters returned                                    OR
  * @post: IncompleteFileError returned if enf of file reached
  */
int getLineTo( FILE *filePointer, int bufferSize, char stopChar, char* outputBuffer );

/**
  * nukeString : completely zeroes every byte in string.
  *
  * @pre: string is length long
  *
  * @note: Yes, this is hilariously overkill.
  */
void nukeString(char* string, int length);

// checkNewLines : checks if the next character in the file stream is a newline
// and skips it if so.
void checkNewLines(FILE *filePointer);

// checkNewLines : checks if the next character in the file stream is a space
// and skips it if so.
void checkSpace(FILE *filePointer);

#endif // STRING_UTILS_H
