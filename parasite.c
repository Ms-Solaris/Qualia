#include <stdio.h>
#include <string.h>

#define MAX_LINE 2048

int BlockWebsite(char url[MAX_LINE]){

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
}


int main()
{
   char url[MAX_LINE] = "xkcd.com";
   UnblockWebsite(url);
   return(0);
}
