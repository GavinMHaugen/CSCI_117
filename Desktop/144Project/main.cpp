#include <iostream>
#include <ctime>
#include <pthread.h>
#include <queue>

using namespace std;

const int NUM_THREADS = 4;

void *Testing(void *threadid)
{
    long temp_thread_id;
    temp_thread_id = (long)threadid;
    cout << "Testing from thread " << temp_thread_id << endl;
    pthread_exit(NULL);

}

struct Car{
    int id;

};

int main()
{
    queue<Car> Intersection;

    pthread_t threads[NUM_THREADS];
    int thread_temp;
    for(int i = 0; i < NUM_THREADS; i++)
    {
        cout << "Creating thread " << i << endl;
        thread_temp = pthread_create(&threads[i], NULL, Testing, (void*)i);
    }
}
