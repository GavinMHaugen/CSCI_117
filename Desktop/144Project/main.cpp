#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <queue>

using namespace std;

#define NUM_CARS 40//this is the number of cars given for the simulation each with a random direction
#define SIMULATIONTIME 60 //this represents the amount of time given to the program(in seconds) to run the simulation
#define NUM_THREADS 3

//This will be our universal queue for the intersection
queue <int> Intersection;

//initializing the mutex for each of the threads I'm going to need in order to avoid
//any concurrent modifications. These will be used to lock and unlock the threads

//protects the Main Intersection Queue operations
pthread_mutex_t Main_Intersection;
//Protects the Car ID numbers
pthread_mutex_t CarID;
//Protects the Cars that are in each direction thread
pthread_mutex_t North;
pthread_mutex_t South;
pthread_mutex_t East;
pthread_mutex_t West;

//pthread_create() requires a unique thread Identifier for the first argurment of the operation call
// THREAD will be used for that arg
pthread_t THREAD;
//pthread_create() requires a thread attribute set for its second arg
//THREADATTR will be used for that arg
pthread_attr_t THREADATTR;
//pthread_create() requires a routine that it will execute once the thread is created
//this is where we will simulate the traffic intersection
//STARTSIMULATION will be the function used for that arg
void *STARTSIMULATION(void *arg);

//Finally we need a counter for the Car ID
int CarIDNumber;

//We need to initialize the mutex object pointed to by mutex so we set the mutexattr to NULL to give it default values
void Initialize_Mutex()
{
    CarIDNumber = 0;
    pthread_mutex_init(&Main_Intersection, NULL);
    pthread_mutex_init(&CarID, NULL);
    pthread_mutex_init(&North, NULL);
    pthread_mutex_init(&South, NULL);
    pthread_mutex_init(&East, NULL);
    pthread_mutex_init(&West, NULL);
    pthread_attr_init(&THREADATTR);

}

int main()
{
    Initialize_Mutex();

    for(int i = 0; i <= NUM_THREADS; i++)
    {

    }

}

/*This function is essentially the meat of the simulation.
It will first assign the Car an ID from the global CarIDNumber value and then randomly place that car in a direction.
The Direction of the car will be listed as follows: 1 = North 2 = South 3 = East 4 = West. The function will check by dividing it by different values
The car will then be pushed into the main Intersection queue  */

void *STARTSIMULATION(void *arg)
{
    int id;

    while(CarIDNumber != NUM_CARS)
    {
        //The id will stand for the car in this simulation so we must increment the CarIDNumber value everytime a thread uses this function
        //in order to make sure we process the intended amount of cars.
        pthread_mutex_lock(&CarID);
        id = CarIDNumber++;
        pthread_mutex_unlock(&CarID);

        //this will handle the cars in the north direction
        if(id % 4 + 1 == 0)
        {
            //Pushing the car into the intersection queue
            pthread_mutex_lock(&Main_Intersection);
            Intersection.push(id);
            pthread_mutex_unlock(&Main_Intersection);

            //Because threads are working concurrently on one queue I have to check and see
            //if this car is at the front of it. If so, we pop it off and go on to the next one.
            //If it is NOT at the front we must sleep and wait for others to pop off the queue
            while(true)
            {
                pthread_mutex_lock(&Main_Intersection);
                if(id != Intersection.front())
                {
                    pthread_mutex_unlock(&Main_Intersection);
                    usleep(1); //sleeping for 1 second
                    continue;
                }
                else
                    break; //this means that it IS the front of the queue so we break out the loop and continure
            }

            pthread_mutex_unlock(&Main_Intersection);


            pthread_mutex_lock(&Main_Intersection);
            cout << "Car " << id << " going north enters the intersection" << endl;
            cout << "Car " << id << " going north leaves the intersection" << endl;
            pthread_mutex_unlock(&Main_Intersection);

            pthread_mutex_lock(&Main_Intersection);
            Intersection.pop();
            pthread_mutex_unlock(&Main_Intersection);
        }

        //This handles the cars in the south direction
        else if(id % 4 + 2 == 0)
        {
            pthread_mutex_lock(&Main_Intersection);
            Intersection.push(id);
            pthread_mutex_unlock(&Main_Intersection);

            while(true)
            {
                pthread_mutex_lock(&Main_Intersection);
                if(id != Intersection.front())
                {
                    pthread_mutex_unlock(&Main_Intersection);
                    usleep(1);
                    continue;
                }

                break;
            }

            pthread_mutex_unlock(&Main_Intersection);
            cout << "Car " << id << " going south enters the intersection" << endl;
            cout << "Car " << id << " going south leaves the intersection" << endl;
            pthread_mutex_lock(&Main_Intersection);

            pthread_mutex_unlock(&Main_Intersection);
            Intersection.pop();
            pthread_mutex_lock(&Main_Intersection);
        }

        //this will handle cars in the east direction
        else if(id % 4 + 3 == 0)
        {
            pthread_mutex_lock(&Main_Intersection);
            Intersection.push(id);
            pthread_mutex_unlock(&Main_Intersection);

            while(true)
            {
                pthread_mutex_lock(&Main_Intersection);
                if(id != Intersection.front())
                {
                    pthread_mutex_unlock(&Main_Intersection);
                    usleep(1);
                    continue;
                }

                break;
            }

            pthread_mutex_unlock(&Main_Intersection);
            cout << "Car " << id << " going east enters the intersection" << endl;
            cout << "Car " << id << " going east leaves the intersection" << endl;
            pthread_mutex_lock(&Main_Intersection);

            pthread_mutex_unlock(&Main_Intersection);
            Intersection.pop();
            pthread_mutex_lock(&Main_Intersection);
        }

        else if (id % 4 + 4 == 0)
        {
            pthread_mutex_lock(&Main_Intersection);
            Intersection.push(id);
            pthread_mutex_unlock(&Main_Intersection);

            while(true)
            {
                pthread_mutex_lock(&Main_Intersection);
                if(id != Intersection.front())
                {
                    pthread_mutex_unlock(&Main_Intersection);
                    usleep(1);
                    continue;
                }

                break;
            }

            pthread_mutex_unlock(&Main_Intersection);
            cout << "Car " << id << " going west enters the intersection" << endl;
            cout << "Car " << id << " going west leaves the intersection" << endl;
            pthread_mutex_lock(&Main_Intersection);

            pthread_mutex_unlock(&Main_Intersection);
            Intersection.pop();
            pthread_mutex_lock(&Main_Intersection);
        }
    }
}
