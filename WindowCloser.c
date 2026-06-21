#include <stdio.h>
#include <signal.h>
#include <string.h>

int RetrieveExternalPID(char programName[], int *pidArray)
{
    /*
    This function finds the PID of any program, using the name of that program, by
    using grep to filter the output of the ps -e command. The exact name of the 
    program must be used. If this can't be found, the function returns -1. Beware 
    that, when fed into the kill function, -1 tries to signal every single program.
    Input:
     char programName[]: A string containing the name of the program. This must be exact, or the program will throw an error.
     int *pidArray: An array of all 0s, to 
    Output:
     0, if the program runs correctly.
     -1, if no process matching programName could be found.
     The first few entries of pidArray will be updated to contain the PIDs found, in no particular order.
    */
    FILE *p;                              // Prepare file data structure
    char currentLine[200];                // Somewhere to put the retrieved data...
    char lookupCommand[200] = "ps -e|grep [[:space:]]";
    char endOfLineCharacter = '$';
    strcat(lookupCommand, programName);
    strncat(lookupCommand, &endOfLineCharacter, 1);
    p = popen(lookupCommand, "r"); // Execute "ps -e", filter for the program we're interested in with grep, and put the data in a 'file' (p) that we can read from.
                                   
    int currentCharacterIndex;
    char pidStr[7];
    int pidInt;
    int loopCount = 0;
    
    while(fgets(currentLine, 200, p) != NULL){
    
        int currentDigitIndex = 0;

        for (currentCharacterIndex = 0; currentCharacterIndex < 7; currentCharacterIndex++)
        { // The first 7 characters of the line are the PID, plus an unknown amount of whitespace. We read each one...
            if (currentLine[currentCharacterIndex] != ' ') // Check whether or not the character is an empty space, and save each non-empty character to pidStr.
            { 
                pidStr[currentDigitIndex] = currentLine[currentCharacterIndex];
                currentDigitIndex++;
            }
        }
        
        currentDigitIndex++;
        pidStr[currentDigitIndex] = 0; //Don't forget to terminate the string, now that we know how long it is.
        int pidInt;
        sscanf(pidStr, "%d", &pidInt); //Convert to an int...
        pidArray[loopCount] = pidInt;  //Save to the array...
        loopCount++;
    }
    
    if (loopCount == 0){ //If grep can't match anything, the while loop above won't execute. This leaves the loop count at 0, which
        return(-1);      //is used here to return an error code to indicate the program couldn't be found.
    }
    
    pclose(p);
    return(0);
}

int main()
{
   int PID[20] = {0};
   int errorCode = RetrieveExternalPID("Discord", PID);
   if (errorCode == -1) {
    printf("Error: Program could not be found.");
    return(-1);
   }
   
   int loopCount = 0;

   while (PID[loopCount] != 0)
   {
    kill(PID[loopCount], SIGTERM);
    loopCount++;
   }
   return(0);
}
