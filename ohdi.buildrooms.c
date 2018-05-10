#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/stat.h>



struct room
{
	
	char name[30];
	int numOutboundConnections;
	struct room* outboundConnections[6];
	char* type;
};

struct room* chosenRooms[7]; 

char* roomName[10]= { "Dungeon","Dragon","QueenKng","Princess",	"Kitchen","Library",
	"DiningRm","MagicRm","PotionRm","PrinceRm"};



void fillRooms();
bool IsGraphFull();
bool IsSameRoom(struct room* RoomX, struct room* RoomY);
void ConnectRoom(struct room* RoomX, struct room* RoomY);
bool ConnectionAlreadyExists(struct room* RoomX, struct room* RoomY);
bool CanAddConnectionFrom(struct room* RoomX);
struct room* GetRandomRoom();
void AddRandomConnection();
void swap(int *a, int *b);
void shuffle(int arr[]);

int main()
{


	srand(time(NULL));
	int i;
	char filename[50] = { 0 };
  char holder[50];
	//make directory path that consists of name, rooms, process id 
	char dir_path[20];

	char name[100];
	char name1[100];
	sprintf(dir_path, "ohdi.rooms.%d", getpid());
	int fd;
	//make directory path 
	int result = mkdir(dir_path, 0755);
	//generate 7 different room files 

	fillRooms(); 

		// Create all connections in graph
		while (IsGraphFull() == false)
		{
			AddRandomConnection();
		}

		//Write to file and store in directory (7 files) 


		for (i = 0; i < 7; i++)

		{

			//Build the filename
			strcpy(filename, dir_path);         
			strcat(filename, "/");
			strcat(filename, chosenRooms[i]->name);

FILE *f= fopen(filename,"w");

//print out the room name 

fprintf(f, "ROOM NAME: %s\n", chosenRooms[i]->name);
 
int j;
for (j = 0; j < chosenRooms[i]->numOutboundConnections; j++)
{

//print out the connections for each room 

fprintf(f, "CONNECTION %d: %s \n" ,j+1, chosenRooms[i]->outboundConnections[j]->name);


}



//print out the room type 
fprintf(f, "ROOM TYPE: %s\n",chosenRooms[i]->type); 


//close file  in each loop 

fclose(f); 

		}

	


return 0; 
}




//CITE: Geeks for Geeks Shuffle Algorithm 

//swap content of two numbers to create random array values 
void swap (int *a, int *b)
{



int temp= *a;
*a=*b;
*b=temp; 


}



void shuffle(int arr[])
{
	srand(time(NULL));
int i; 




	for ( i = 9; i > 0; i--)
	{
		// Pick a random index from 0 to i
		int j = rand()%(i + 1);

		// Swap arr[i] with the element at random index
		swap(&arr[i], &arr[j]);
	}




}





void fillRooms()
{
	//d
	int i,h, roomCount = 0, r, typeCount=0;


 int count = 0, index=0;
	bool unique = false; 
	//give  random room names 

	while (true)

	{




//Cite: Geeks for Geeks Shuffly Array and Sort Algorithm 
//https://www.geeksforgeeks.org/shuffle-a-given-array/

// fill the array values from 0-9 to account for all room names
int randomArray[10];
for (i=0; i<10; i++)

{

randomArray[i]=i;
//printf("random array %d\n", i);
}

//shuffle the array that consists of numbers 0-9
shuffle(randomArray);

//transfer just the first seven shuffled values over to the keeper array 
int keeperArray[7];

for(i=0; i<7; i++)
{
keeperArray[i]=randomArray[i];


}
//create seven rooms 
		for (i = 0; i < 7; i++)

		{
			
//allocate space for room 
			chosenRooms[i] = (struct room *) malloc(sizeof(struct room));
//assign room name 
			strcpy(chosenRooms[i]->name, roomName[keeperArray[i]]);
//set num outbound connections to 0 
			chosenRooms[i]->numOutboundConnections = 0;
			roomCount++;
			
		}
		
		if (roomCount==7)
		{
			break;
		}


	}

	//give random room types. will be random by default since room selection will always be random

	//if it's index 0 or 1 then give it start room, end room respective, otherwise give it midroom.



 
int startRoom=0, endRoom=0; 
// keeping looping until two numbers are distinct 



do
{
//generate random number for start and end index
startRoom= rand()%7;
endRoom=rand()%7;

}

//keep doing this until start room and end room are distinct 
while(startRoom==endRoom);


	for (i= 0; i< 7; i++)

	{
//assign startroom to its matching i value 
		if (i == startRoom)
		{
			chosenRooms[i]->type = "START_ROOM";

		}
//assign end room to its matching i value 
		else if (i == endRoom)
		{
			chosenRooms[i]->type = "END_ROOM";

		} //everything else should be a midroom
		else
		{
			chosenRooms[i]->type = "MID_ROOM";

		}

	}

}



// Returns true if all rooms have 3 to 6 outbound connections, false otherwise
bool IsGraphFull()
{
	int i; 
	for (i = 0; i < 7; i++)
	{
		if (chosenRooms[i]->numOutboundConnections < 3)
// if numOutBoundConnections is less than 3 the graph is not full

		{
			return false;
		}
	}
	return true;
}

// Adds a random, valid outbound connection from a room to another room
void AddRandomConnection()
{
	struct room* RoomA;  // Maybe a struct, maybe global arrays of ints
	struct room* RoomB;


	while (true)
	{
//room a gets assigned a random room 
		RoomA = GetRandomRoom();

		if (CanAddConnectionFrom(RoomA) == true)
			break;
	}

	do
	{
		RoomB = GetRandomRoom();
	} while (CanAddConnectionFrom(RoomB) == false || IsSameRoom(RoomA, RoomB) == true || ConnectionAlreadyExists(RoomA, RoomB) == true);

	ConnectRoom(RoomA, RoomB);   
	ConnectRoom(RoomB, RoomA);  //  because this A and B will be destroyed when this function terminates
}

// Returns a random room, does NOT validate if connection can be added
struct room*  GetRandomRoom()
{
	
	int r = rand() % 7;
	return chosenRooms[r];
}

// Returns true if a connection can be added from room x (< 6 outbound connections), false otherwise
bool CanAddConnectionFrom(struct room* RoomX)
{
	if (RoomX->numOutboundConnections < 6)
	{
		return true;
	}

	else
	{
		return false;
	}



}
// Returns true if a connection from room x to room y already exists, false otherwise
bool ConnectionAlreadyExists(struct room* RoomX, struct room* RoomY)
{
	//iterate to see if there's a match
	int conRoomX = RoomX->numOutboundConnections;

	int i; 
	for (i = 0; i < conRoomX; i++)
	{
		//this might be wrong 
	//	if (RoomX->outboundConnections[i]->id == RoomY->id)
if(strcmp(RoomX->outboundConnections[i]->name, RoomY->name) == 0)
			return 1;
	}

	return 0; 
}

// Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(struct room* RoomX, struct room* RoomY)
{
//pick up from the most recent numOutboundConnections index 	
	int conRoomX = RoomX->numOutboundConnections; 


//new connection so you dont have to decrement 
	RoomX->outboundConnections[conRoomX] = RoomY;
	RoomX->numOutboundConnections++;


}

// Returns true if Rooms x and y are the same room, false otherwise
bool IsSameRoom(struct room* RoomX, struct room* RoomY)
{
//	if the names are the same then its the same room
if(strcmp(RoomX->name, RoomY->name) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
