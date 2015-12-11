/*Gavin Haugen
144 Final Project
Fall 2015*/

//Included Libraries used in program
#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <queue>
#include <ctime>
#include <time.h>
#include <cstdlib>

using namespace std;

//These are static values used through the program
#define TOTAL_CARS 20
#define SPEED_OF_CAR 5
#define MIN_DISTANCE_TO_PASS 50
#define CAR_DEFAULT_DISTANCE 200
#define NUM_OF_DIRECTIONS 4

//Car class that holds the direction and distance from center
class Car
{
    public:
        Car(int d, int dir)
        {
            distance = d;
            direction = dir;
        }

        int distance;
        int direction;
};

//The queue I'll be using to Simulate the Intersection traffic
deque<Car> Traffic_Intersection_Waiting;
deque<Car> Traffic_Intersection_FOQ;

//I'll use this to continually simulate the process in the simulation function
bool runsim = true;

//used to count the total amount of cars the simulation has processed
int TotalProcessedTraffic = 0;

//The pthread mutex's ill be using to protect the data
pthread_mutex_t Traffic_Lock;
pthread_mutex_t Light_Lock;

//HELPER FUNCTIONS

//This Function checks for another car in the queue that can be processed
int Search(int dir);

//pthread functions
void *ProcessTraffic(void *arg);
void *ProcessDirections(void *arg);
void *IntersectionStatus(void *arg);


int main()
{
    //initializing the time value for our random values we will generate
    srand(time(0));
    //initializing the threads we will need for the simulation
    pthread_t p_ProcessTraffic, p_ProcessDirection0, p_ProcessDirection1, p_ProcessDirection2, p_ProcessDirection3, p_IntersectionStatus;

    //initializing the mutex locks for the simulation
    pthread_mutex_init(&Traffic_Lock, NULL);
    pthread_mutex_init(&Light_Lock, NULL);

    //now we create and start the threads that will be the meat of the simulation
    // one for handling the cars on the way to the intersection
    pthread_create(&p_ProcessTraffic, NULL, ProcessTraffic, (void*)0);
    //4 for each direction that will be handled when the car is waiting at the intersection
    pthread_create(&p_ProcessDirection0, NULL, ProcessDirections, (void*)0);
    pthread_create(&p_ProcessDirection1, NULL, ProcessDirections, (void*)1);
    pthread_create(&p_ProcessDirection2, NULL, ProcessDirections, (void*)2);
    pthread_create(&p_ProcessDirection3, NULL, ProcessDirections, (void*)3);
    //one for the status of the simulation that outputs how many cars processed and how many cars waiting etc
    pthread_create(&p_IntersectionStatus, NULL, IntersectionStatus, (void*)2);

    //joining all of the used threads
    pthread_join(p_ProcessTraffic, NULL);
    pthread_join(p_ProcessDirection0, NULL);
    pthread_join(p_ProcessDirection1, NULL);
    pthread_join(p_ProcessDirection2, NULL);
    pthread_join(p_ProcessDirection3, NULL);
    pthread_join(p_IntersectionStatus, NULL);


    //end of the simulation
    cout << "Done with simulation..." << endl;

    return 0;
}

//this function just gives an update on what the program is doing in the simulation
//giving the number of cars on the way to the intersection and the number of cars waiting at intersection to pass
//also gives the direction of the cars as follows: 1 = North 2 = South 3 = East 4 = West
void *IntersectionStatus(void *arg)
{
    //giving the static number of variables for the simulation
    cout << "Total number of cars being processed " << TOTAL_CARS << endl;
    cout << "Minimum distance for car to pass intersection " << MIN_DISTANCE_TO_PASS << endl;
    cout << "Constant speed of cars being simulated " << SPEED_OF_CAR << endl;
    cout << "Status of simulation" << endl;

    while(runsim)
    {
        pthread_mutex_lock(&Traffic_Lock);
        pthread_mutex_lock(&Light_Lock);

        //checking to see if there are any cars on the way to the intersection(in the waiting queue)
        if(!Traffic_Intersection_Waiting.empty())
        {
            cout << Traffic_Intersection_Waiting.size() << " cars on the way to intersection" << endl;
            //Iterating through the Waiting queue of cars on the way to the intersection giving the distance and direction of car
            for(unsigned int i = 0; i < Traffic_Intersection_Waiting.size(); i++)
            {
                cout << "Car " << i << " " << Traffic_Intersection_Waiting[i].distance << " units away from intersection in direction " << Traffic_Intersection_Waiting[i].direction << endl;

            }
            cout << endl;
        }
        //Otherwise...
        else
        {
            cout << "No Traffic waiting to be processed" << endl;
        }


        //checking the queue of cars AT the intersection ready to be processed
        if(!Traffic_Intersection_FOQ.empty())
        {
            cout << Traffic_Intersection_FOQ.size() << " cars stopped at intersection waiting to be processed." << endl;
        }
        //giving the total number of cars processed by the simulation
        cout << "Total number of cars processed " << TotalProcessedTraffic << endl;
        cout << endl << endl;

        sleep(1);
        pthread_mutex_unlock(&Light_Lock);
        pthread_mutex_unlock(&Traffic_Lock);
    }
}

//This function checks on the cars that are "on the way" to the intersection by decrementing the speed as it gets closer
//This function also generates the Car object for the program to process
void *ProcessTraffic(void *arg)
{
    sleep(2);
    while(runsim)
    {
        pthread_mutex_lock(&Traffic_Lock);
        //if there are cars on the way to the intersection
        if(!Traffic_Intersection_Waiting.empty())
        {
            for(unsigned int i = 0; i < Traffic_Intersection_Waiting.size(); i++)
            {
                //if there are cars waiting to pass the intersection that are stopped
                if(Traffic_Intersection_Waiting[i].distance <= MIN_DISTANCE_TO_PASS)
                    {
                        pthread_mutex_lock(&Light_Lock);
                        //we move the car thats not on its way anymore to the queue that holds the cars that are literally waiting at the intersection
                        Traffic_Intersection_FOQ.push_back(Traffic_Intersection_Waiting[i]);
                        //now we remove car
                        Traffic_Intersection_Waiting.erase(Traffic_Intersection_Waiting.begin()+i);
                        i--;
                        pthread_mutex_unlock(&Light_Lock);
                    }
                else
                {
                    //moving the oncoming traffic closer
                    Traffic_Intersection_Waiting[i].distance -= SPEED_OF_CAR;
                }
            }
        }

        //if the number of cars on their way + number of cars waiting is less than the max amount of cars we are simulating
        //we create another Car object to put in the queue of cars on their way to the intersection
        if(Traffic_Intersection_Waiting.size() + Traffic_Intersection_FOQ.size() < TOTAL_CARS)
        {
            //Car constructor giving the car a random distance from intersection and random direction its coming from
            Car newcar((CAR_DEFAULT_DISTANCE + (rand() % (CAR_DEFAULT_DISTANCE))), (rand() % NUM_OF_DIRECTIONS));
            Traffic_Intersection_Waiting.push_back(newcar);
            cout << "Car being generated" << endl;
        }

        pthread_mutex_unlock(&Traffic_Lock);
    }
}

//This function essentially processes the traffic for the function
//takes the cars waiting at the intersection in the Front of Queue Queue and then erases them because they are ready to cross
// then increments the total number of cars we have processed.
void *ProcessDirections(void *arg)
{
    sleep(2);
    int ThreadCount = (long)arg;

    while(runsim)
    {
        pthread_mutex_lock(&Light_Lock);
        //searches the front of queue queue for the matching car with the matching direction number
        int position = Search(ThreadCount);
        //if it is a viable direction then we erase it from the queue
        if(position != -1)
        {
            Traffic_Intersection_FOQ.erase(Traffic_Intersection_FOQ.begin() + position);
            //and then we increment the number of cars we have currently processed
            TotalProcessedTraffic++;
        }

        pthread_mutex_unlock(&Light_Lock);
    }
}

//this function searches the queue of cars that are currently stopped and waiting to be processed
//and checks is direction so that the function ProcessDirections can gather what direction the car it is processesing is going
int Search(int dir)
{
    if(!Traffic_Intersection_FOQ.empty())
    {
        //searching through the queue of cars that are stopped at the intersection
        //to see if their direction is equivelent to that of the one in the argument of the function
        //if it checks out then it returns that value(direction)
        for(unsigned int i = 0; i < Traffic_Intersection_FOQ.size(); i++)
        {
            if(Traffic_Intersection_FOQ[i].direction == dir)
            {
                return i;
            }
        }
    }
    return -1;
}
