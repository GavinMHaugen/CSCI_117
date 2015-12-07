/*Gavin Haugen
144 Final Project
Fall 2015 */

//Included Libraries used in program
#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <queue>
#inlcude <ctime>
#include <cmath>

using namespace std;

//These are static values used through the program
#define TOTAL_CARS 50
#define SPEED_OF_CAR 5

//The queue I'll be using to Simulate the Intersection traffic
queue<int> Traffic_Intersection;


//Car class that holds the direction and distance from center
class Car
{
    public:
        Car(int distance, int direction)
        {
            d = distance;
            dir = direction;
        }

        int d;
        int dir;
};

int main()
{


}

