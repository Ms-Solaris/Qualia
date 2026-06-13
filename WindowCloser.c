#include <stdio.h>
#include <signal.h>
#include <string.h>

int RetrieveExternalPID(char programName[])
{
    /*
    This function finds the PID of any program, using the name of that program, by
    using grep to filter the output of the ps -e command. The exact name of the 
    program must be used. If this can't be found, the function returns -1. Beware 
    that, when fed into the kill function, -1 tries to signal every single program.
    Input:
     char programName[]: A string containing the name of the program. This must be exact, or the program will throw an error.
     Output:
     int pid:            The PID of the requested program.
    */
    FILE *p;                              // Prepare file data structure
    char currentLine[200];                // Somewhere to put the retrieved data...
    char lookupCommand[200] = "ps -e|grep [[:space:]]";
    char endOfLineCharacter = '$';
    strcat(lookupCommand, programName);
    strncat(lookupCommand, &endOfLineCharacter, 1);
    p = popen(lookupCommand, "r"); // Execute "ps -e", filter for the program we're interested in with grep, and put the data in a 'file' (p) that we can read from.
                                   // Currently assumes that grep finds only a single line.
    fgets(currentLine, 200, p);    // Attempt to extract the single line from the file.
     if (feof(p)) {                // Check if it worked. If not, there was no file i.e. grep turned up nothing: we exit with an error.
        return(-1);
     }
    int currentCharacterIndex;
    char pidStr[7];
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
    sscanf(pidStr, "%d", &pidInt);
    pclose(p);
    return(pidInt);
}

int main()
{
   int PID = RetrieveExternalPID("firefox-esr");
   if (PID == -1) {
    printf("Error: Program could not be found.");
    return(-1);
   }
   kill(PID, SIGTERM);
   return(0);
}
