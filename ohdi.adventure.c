#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <time.h> 
#include <fcntl.h> 
#include <sys/types.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <unistd.h>
#include <pthread.h>

//make room struct. recreate room structs after reading the room filenames
struct newRooms {
char name[30];
char* type; 
char  roomConnections[7][30];
int numberOfRooms;

};

//store rooms
struct newRooms*  newRoomsMain[7];

//path took to find end room
char path[7][30];

int startIndex;
int endIndex;
int steps=0; 

//store the filenames in the directory 

// Mutex 

pthread_t thread2;
pthread_mutex_t tlock; 


char filenames[7][50];

void findLatestDirectory(char* curdir){

  int newestDirTime = -1; // Modified timestamp of newest subdir examined
  char targetDirPrefix[32] = "ohdi.rooms."; // Prefix we're looking for
  char newestDirName[256]; // Holds the name of the newest dir that contains prefix
  memset(newestDirName, '\0', sizeof(newestDirName));

  DIR* dirToCheck; // Holds the directory we're starting in
  struct dirent *fileInDir; // Holds the current subdir of the starting dir
  struct stat dirAttributes; // Holds information we've gained about subdir

  dirToCheck = opendir("."); // Open up the directory this program was run in

  if (dirToCheck > 0) // Make sure the current directory could be opened
  {
    while ((fileInDir = readdir(dirToCheck)) != NULL) // Check each entry in dir
    {
      if (strstr(fileInDir->d_name, targetDirPrefix) != NULL) // If entry has prefix
      {
       // printf("Found the prefex: %s\n", fileInDir->d_name);
        stat(fileInDir->d_name, &dirAttributes); // Get attributes of the entry

        if ((int)dirAttributes.st_mtime > newestDirTime) // If this time is bigger
        {
          newestDirTime = (int)dirAttributes.st_mtime;
          memset(newestDirName, '\0', sizeof(newestDirName));
          strcpy(newestDirName, fileInDir->d_name);
    //      printf("Newer subdir: %s, new time: %d\n",
      //           fileInDir->d_name, newestDirTime);
        }
      }
    }
  }

  closedir(dirToCheck); // Close the directory we opened
strcpy (curdir, newestDirName);

//  printf("Newest entry found is: %s\n", newestDirName);



	
}

void readFilesFromDirectory(char *file) {
char filename2[50];
DIR* dirToCheck;
int count=0; 
struct dirent *fileInDir;

//open the correct directory 

dirToCheck= opendir(file); 

if (dirToCheck>0)
{

while ((fileInDir = readdir(dirToCheck)) != NULL) // Check each entry in  dir
{

if(strcmp(fileInDir->d_name,".")!=0 && strcmp(fileInDir->d_name, "..") !=0)
{
//copy the name in the file d_name
strcpy(filename2, fileInDir->d_name);

//put the name in the filenames array to keep an array of filenames(rooms)
strcpy(filenames[count],filename2);



count++;
}

}
}

int index=0;
int line=1;
char mystring[100];
char filepath[50];
char room[50], roomname[50],room2[50], connection[50], connection2[50],
connection1[50];
//open file and put things in structs


  
while(true)

{

//build rooms again

if (index==7)
{break;}
strcpy(filepath,file);
strcat(filepath,"/");
strcat(filepath,filenames[index]);

newRoomsMain[index] = (struct newRooms *) malloc(sizeof(struct newRooms));


FILE* pFile; 

//open one of the rooms files 

pFile= fopen(filepath,"r");

int count=0;
if(pFile!=NULL)
{


	while(fgets(mystring,100,pFile) != NULL)
	{
 //Line 1 has to give the name information
		if (line==1)
	{
 			sscanf(mystring, "%s %s %s ", room, room2, roomname);
//isolate the roomname using sscanf


		strcpy(newRoomsMain[index]->name,roomname); 
	// set the room name
		}
//line !=1 has to give connetion information or room type information
		if (line  != 1)

		{

		sscanf(mystring, "%s%s%s ", connection,connection1, connection2);

//when scanning mystring if connection is equal to CONNECTION that means you're on the line with a connection in it
			if (strcmp(connection, "CONNECTION")==0)

			{
			strcpy(newRoomsMain[index]->roomConnections[count],connection2);
			//	its known that the line mystring says connection in it so connection2 stores the name of that connected room
         newRoomsMain[index]->numberOfRooms++;
			
			count++; 

				}
//if mystring doesnt contain connection information and its not on line one it has to contain type information
			else{
				newRoomsMain[index]->type= connection2;
				//	connection 2 now hold the room type in the mystring line

						if (strcmp(connection2, "START_ROOM")==0)
							{ startIndex= index;
					//	if the room type is start room capture the index and store in the global variable start room
							}
						if (strcmp(connection2,"END_ROOM")==0)
						{endIndex= index;

						//if the room is end room capture the index and store in the global variable end room


							}
			}



			}


line++;


	}


fclose (pFile);
}

//incremnet index to get data for the next room
index++;
//reset line since  loop is going to look at the beginning of a new file 
line=1;
count=0;
 }

}


//main thread reads from currentTime.txt file and prints the time 
void readAndPrintTime() 
{
char c; 

//open relevant file 
FILE *f = fopen("currentTime.txt","r");
//print character by character until reaching the end of file 
c=fgetc(f);
while (c!= EOF)
{
	printf("%c", c); 
	c=fgetc(f);
}

printf("\n");

fclose(f);




}


//thread write time functinon
void* writeTime()

{
//lock the thread 

pthread_mutex_lock(&tlock); 
//write to a file 

FILE *f = fopen("currentTime.txt","w");


char timeArray[250];

char time2[250];

time_t t; 

struct tm *tmp; 

t=time(NULL);
//get local time 
tmp= localtime(&t);
//structure time format 
strftime(timeArray, sizeof(timeArray),"%I:%M%p, %A, %b, %d, %Y" ,tmp); 


strcpy( time2, timeArray); 

//print local time to file 
fprintf(f,"%s", time2);



fclose(f); 

//unlock mutex so main thread can take control 
pthread_mutex_unlock(&tlock); 
}







//BREWSTER SAID IT'S OKAY TO PUT MAIN THREAD IN A FUNCTION  NOT IN MAIN()

int gameInterface(keyIndex) {
//lock main thread
pthread_mutex_lock(&tlock);

char sRoom[50];

 
int test=1; 

while(test==1)

{//print the current location and possible locations to go to
printf("\n"); 
printf("CURRENT LOCATION: %s\n",newRoomsMain[keyIndex]->name);
printf("POSSIBLE CONNECTIONS: ");
int i; 
for (i=0; i<newRoomsMain[keyIndex]->numberOfRooms;i++)
{
if(i==(newRoomsMain[keyIndex]->numberOfRooms-1))
{//print all connections 
printf("%s. \n", newRoomsMain[keyIndex]->roomConnections[i]);
}
else
{

printf("%s,", newRoomsMain[keyIndex]->roomConnections[i]);

}

}

char buffer[50];
char *b = buffer;
size_t bufsize=32;


printf("WHERE TO? > ");
getline(&b, &bufsize,stdin);

buffer[strcspn(buffer, "\r\n")] = 0;



int match=0;

//BREWSTER SAID IT WAS OKAY TO DO PARALLEL THINGS OUTSIDE OF MAIN() 

if(strcmp(buffer,"time")==0)

{
//primary thread unlocks to allow second thread to do work 
pthread_mutex_unlock(&tlock);
//create second thread  and write time
pthread_create(&thread2,NULL,(void*)&writeTime,NULL);
//wait for thread 2 to finish
pthread_join(thread2,NULL); 
//lock the main thread
pthread_mutex_lock(&tlock);
//main thread reads and prints time
printf("\n"); 
readAndPrintTime();

}


//BREWSTER SAID IT WAS OKAY TO DO PARALLEL THINGS OUTSIDE OF MAIN()

//if user types something other than time
else{

for (i=0; i<newRoomsMain[keyIndex]->numberOfRooms;i++)
{

//check if input equals the possible rooms to go to
if (strcmp(newRoomsMain[keyIndex]->roomConnections[i],buffer)==0)


   {
			
			match=1; 
 			test=0;
			int index; int j;
			for (j=0; j<7;j++)
		{	if(strcmp(newRoomsMain[j]->name,newRoomsMain[keyIndex]->roomConnections[i])==0)
        {
				 //if a math is found as in  if the input matches one of the possible connections return the index of that room connection to go to next
         //unlock the main thread  since the start of this function locks it
pthread_mutex_unlock(&tlock);

					return j; 
					break;	}	 
		}
	   	}

}
//if match not found ask player to reenter input 
if (match==0)

{

printf("\n");

printf(" HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN. \n");

	
}


}

}



}

//everything else 



void playGame()

{


int retIndex;

//print out the start room game information to user
retIndex= gameInterface(startIndex);

int retIndex2;
while(true)


{
char stringCopy[30];
//put the chosen room connectionin in the path
strcpy(path[steps], newRoomsMain[retIndex]->name);
//increment steps
steps++;

//if the return index (room chosen by user) is equal to the end index then the user found the room

if(retIndex==endIndex)
{
printf("\n");
printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
printf("YOU TOOK  %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
int i;
//iteratate over the path array to reveal path user took to reach end room
//printf("Path taken:");
for(i=0;i<steps;i++)
{

//printf("in here\n");
printf("%s\n", path[i]);
}

printf("\n");

break;

} 



else

{
//if the return index does not equal end index, keep presenting game interface to allow user to choose another room to visit
retIndex=gameInterface(retIndex);
}


}


}






int main(){




char dir[100];
memset(dir, '\0', sizeof(dir));

findLatestDirectory(dir);
//read file from directory to make the room structs from the room files
readFilesFromDirectory(dir);

playGame(); 


	return 0;
}
