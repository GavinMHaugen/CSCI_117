/*Gavin Haugen
144 Final Project
Fall 2015 */

//Included Libraries used in program
#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <queue>
#include <ctime>
#include <cmath>

using namespace std;

//These are static values used through the program
#define TOTAL_CARS 50
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
int Search(int direction);

//pthread functions
void *ProcessTraffic(void *arg);
void *ProcessDirections(void *arg);
void *IntersectionStatus(void *arg);


int main()
{
    srand(0);
    pthread_t p_ProcessTraffic, p_ProcessDirection0, p_ProcessDirection1, p_ProcessDirection2, p_ProcessDirection3, p_IntersectionStatus;

    pthread_mutex_init(&Traffic_Lock, NULL);
    pthread_mutex_init(&Light_Lock, NULL);

    pthread_create(&p_ProcessTraffic, NULL, ProcessTraffic, (void*)0);
    pthread_create(&p_ProcessDirection0, NULL, ProcessDirections, (void*)0);
    pthread_create(&p_ProcessDirection1, NULL, ProcessDirections, (void*)1);
    pthread_create(&p_ProcessDirection2, NULL, ProcessDirections, (void*)2);
    pthread_create(&p_ProcessDirection3, NULL, ProcessDirections, (void*)3);
    pthread_create(&p_IntersectionStatus, NULL, IntersectionStatus, (void*)2);

    pthread_join(p_ProcessTraffic, NULL);
    pthread_join(p_ProcessDirection0, NULL);
    pthread_join(p_ProcessDirection1, NULL);
    pthread_join(p_ProcessDirection2, NULL);
    pthread_join(p_ProcessDirection3, NULL);
    pthread_join(p_IntersectionStatus, NULL);


    return 0;
}

void *IntersectionStatus(void *arg)
{
    cout << "Total number of cars being processed " << TOTAL_CARS << endl;
    cout << "Minimum distance for car to pass intersection " << MIN_DISTANCE_TO_PASS << endl;
    cout << "Constant speed of cars being simulated " << SPEED_OF_CAR << endl;
    cout << "Status of simulation" << endl;

    while(runsim)
    {
        pthread_mutex_lock(&Traffic_Lock);
        pthread_mutex_lock(&Light_Lock);

        if(!Traffic_Intersection_Waiting.empty())
        {
            cout << Traffic_Intersection_Waiting.size() << " cars on the way to intersection" << endl;
            for(int i = 0; i < Traffic_Intersection_Waiting.size(); i++)
            {
                cout << "Car " << i << " " << Traffic_Intersection_Waiting[i].distance << " units away from intersection in direction " << Traffic_Intersection_Waiting[i].direction << endl;

            }
            cout << endl;
        }
        else
        {
            cout << "No Traffic waiting to be processed" << endl;
        }

        if(!Traffic_Intersection_FOQ.empty())
        {
            cout << Traffic_Intersection_FOQ.size() << " cars stopped at intersection waiting to be processed." << endl;
        }

        cout << "Total number of cars processed " << TotalProcessedTraffic << endl;
        cout << endl << endl;

        sleep(1);
        pthread_mutex_unlock(&Light_Lock);
        pthread_mutex_unlock(&Traffic_Lock);
    }
}

void *ProcessTraffic(void *arg)
{
    sleep(2);
    while(runsim)
    {
        pthread_mutex_lock(&Traffic_Lock);

        if(!Traffic_Intersection_Waiting.empty())
        {
            for(int i = 0; i < Traffic_Intersection_Waiting.size(); i++)
            {
                if(Traffic_Intersection_Waiting[i].distance <= MIN_DISTANCE_TO_PASS)
                    {
                        pthread_mutex_lock(&Light_Lock);
                        Traffic_Intersection_FOQ.push_back(Traffic_Intersection_Waiting[i]);
                        Traffic_Intersection_Waiting.erase(Traffic_Intersection_Waiting.begin()+i);
                        i--;
                        pthread_mutex_unlock(&Light_Lock);
                    }
                else
                {
                    Traffic_Intersection_Waiting[i].distance -= SPEED_OF_CAR;
                }
            }
        }

        if(Traffic_Intersection_Waiting.size()+Traffic_Intersection_FOQ.size() < TOTAL_CARS)
        {
            Car newcar(CAR_DEFAULT_DISTANCE+(rand() % (CAR_DEFAULT_DISTANCE/3)), rand()%NUM_OF_DIRECTIONS);
            Traffic_Intersection_Waiting.push_back(newcar);
        }

        pthread_mutex_unlock(&Traffic_Lock);
    }
}

void *ProcessDirections(void *arg)
{
    sleep(2);
    int ThreadCount = (long)arg;

    while(runsim)
    {
        pthread_mutex_lock(&Light_Lock);
        int position = Search(ThreadCount);
        if(position != -1)
        {
            Traffic_Intersection_FOQ.erase(Traffic_Intersection_FOQ.begin() + position);
            TotalProcessedTraffic++;
        }

        pthread_mutex_unlock(&Light_Lock);
    }
}

int Search(int direction)
{
    if(Traffic_Intersection_FOQ.empty())
    {
        for(int i = 0; i < Traffic_Intersection_FOQ.size(); i++)
        {
            if(Traffic_Intersection_FOQ[i].direction == direction)
            {
                return i;
            }
        }
    }
    return -1;
}
