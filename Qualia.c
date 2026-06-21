#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_LINE 2048

int BlockWebsite(char url[MAX_LINE]){
   /*
   Adds the url provided to hosts, thus preventing Linux from connecting to it.
   Must be run twice to fully block a url - once with *website.com* and once with
   www.*website.com*.
   If the website is already blocked, no change will be made, and an error code
   will be returned.
   Inputs:
    char url[MAX_LINE]: The url to be blocked. Should have the length MAX_LINE to avoid overflow errors.
   Outputs:
    0 if the program works correctly, -1 if the website is already blocked.
   */
   FILE *host;
   host = fopen("/etc/hosts", "r");
   char currentLine[MAX_LINE];
   char blockString[MAX_LINE] = "127.0.0.1 ";
   char endOfLine[MAX_LINE] = "\n";
   strcat(blockString, url);
   strcat(blockString, endOfLine);

   while (fgets(currentLine, MAX_LINE, host) != NULL){
      
      if (strcmp(blockString, currentLine) == 0)
      {
         printf("Error: website is already blocked.");
         return -1;
      }
   }

   fclose(host);
   host = fopen("/etc/hosts", "a");
   fprintf(host, blockString);
   return 0;

}

int UnblockWebsite(char url[MAX_LINE]){
   /*
   Looks for the provided url in hosts, and removes all instances of it,
   thus allowing Linux to connect to it again.
   Doesn't do anything if no urls matching the string 'url' are found.
   Inputs:
    char url: The url to be unblocked. This should have length MAX_LINE
    to avoid overflow errors.
   Outputs:
    Always outputs 0.
   */
   FILE *host;
   FILE *clone = tmpfile();
   host = fopen("/etc/hosts", "r");
   char currentLine[MAX_LINE];
   char blockString[MAX_LINE] = "127.0.0.1 ";
   char endOfLine[MAX_LINE] = "\n";
   strcat(blockString, url);
   strcat(blockString, endOfLine);

   while (fgets(currentLine, MAX_LINE, host) != NULL){
      if (strcmp(currentLine, blockString) != 0)
      {
         fputs(currentLine, clone);
      }
   }
   fclose(host);
   rewind(clone);
   host = fopen("/etc/hosts", "w");
   while (fgets(currentLine, MAX_LINE, clone) != NULL)
   {
      fputs(currentLine, host);
   }
   fclose(host);
   fclose(clone);
   return(0);
}

int RetrieveExternalPID(char programName[], int *pidArray)
{
    /*
    This function finds the PIDs of any program, using the name of that program, by
    using grep to filter the output of the ps -e command. The exact name of the 
    program must be used. If this can't be found, the function returns -1. Beware 
    that, when fed into the kill function, -1 tries to signal every single program.
    Input:
     char programName[]: A string containing the name of the program. This must be exact, or the program will throw an error.
     int *pidArray: An array of all 0s, to be filled with found PIDs.
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

int CloseProgram(char programName[]) {
    /*
    Sends SIGTERM to all the processes with a given name, as displayed by 'ps -e'. The 
    name must be exact. If no program with the given name is found, the function returns -1.
    Using regex characters may also screw up the string parsing. I'll sanitise my strings...
    eventually.

    Input:
        char programName[]: The name of the program to be closed, as given by 
        'ps -e'.
    Outputs:
        0 if SIGTERM is sent successfully.
        -1 if the name of the program cannot be found.
    */

    int PID[20] = {0};
   int errorCode = RetrieveExternalPID(programName, PID);
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



int main(){
    time_t t;
    FILE *programsToBeKilled;
    programsToBeKilled = fopen("FullKillList.txt", "r");
    FILE *websitesToBeBlocked;
    websitesToBeBlocked = fopen("FullBlockList.txt", "r");
    char currentProgram[100]; //Also contains urls sometimes.
    

                                     //Creates a struct called date depending on 
    while (1){                       //the value of t, which can be used to extract
        sleep(60);                   //the day, hour, minute, etc.
        t = time(NULL);
        struct tm date = *localtime(&t);
        if (date.tm_hour == 22 && date.tm_min > 5)
        {
            while(fgets(currentProgram, 100, programsToBeKilled) != NULL)
            {                                                              //Reads every line of FullKillList.txt, and tries
                CloseProgram(currentProgram);                              //to send SIGTERM to every process with a name that is
            }                                                              //in the list. At the moment, this will spew out a lot
            rewind(programsToBeKilled);                                    //of error messages, because CloseProgram is designed to
        } else if (date.tm_hour > 22)                                      //fail loudly if it can't find the process you asked for
        {                                                                  //(which it probably can't, because this code block runs
            while(fgets(currentProgram, 100, programsToBeKilled) != NULL)  //every single minute)
            { 
                CloseProgram(currentProgram);
            }
            rewind(programsToBeKilled);
        }

        if (date.tm_hour > 7 && date.tm_hour < 17)
        {
            while(fgets(currentProgram, 100, websitesToBeBlocked) != NULL)
            {
                BlockWebsite(currentProgram)
            }

        } else
        {
            while(fgets(currentProgram, 100, websitesToBeBlocked) != NULL)
            {
                UnblockWebsite(currentProgram)
            }
        }
        
    }
    
}
   