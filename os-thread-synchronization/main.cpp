#include <iostream>
#include <vector>
#include <string>
#include <pthread.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

extern "C" {
#include "hw2_output.h"
}

using namespace std;

class Order {
public:
    hw2_actions type;
    timespec *time_stamp;

    Order() {
    }
    Order(long _time_stamp) {
        struct timeval now{}; gettimeofday(&now,nullptr);
        time_stamp = new timespec();
        time_stamp->tv_sec =  _time_stamp / 1000;
        time_stamp->tv_nsec = ( (_time_stamp % 1000) * 1000) * 1000;
    }
};
class PrivateRegion {
public:
    int size_x;
    int size_y;
    int starting_index_x;
    int starting_index_y;

    PrivateRegion() {
        size_x = 0;
        size_y = 0;
        starting_index_x = 0;
        starting_index_y = 0;
    }
    PrivateRegion(int _size_y, int _size_x, int _starting_index_y, int _starting_index_x) {
        size_x = _size_x;
        size_y = _size_y;
        starting_index_x = _starting_index_x;
        starting_index_y = _starting_index_y;
    }
    void printPrivateRegion() const {
        std::cout << "size_x: " << size_x << std::endl;
        std::cout << "size_y: " << size_y << std::endl;
        std::cout << "starting_index_x: " << starting_index_x << std::endl;
        std::cout << "starting_index_y: " << starting_index_y << std::endl;
    }
};
class SmokerRegion {
public:
    int position_x;
    int position_y;
    int cigs_to_smoke;

    SmokerRegion() {
        position_x = 0;
        position_y = 0;
        cigs_to_smoke = 0;
    }
    SmokerRegion(int _position_y, int _position_x, int _cigs_to_smoke) {
        position_x = _position_x;
        position_y = _position_y;
        cigs_to_smoke = _cigs_to_smoke;
    }
    void printSmokerRegion() const {
        std::cout  << std::endl <<  "position_x: " << position_x << std::endl;
        std::cout << "position_y: " << position_y << std::endl;
        std::cout << "cigs_to_smoke: " << cigs_to_smoke << std::endl;
    }
};
class Private {
public:
    int gid;
    int picking_duration;
    int region_count;
    bool isStarted = false;
    vector<PrivateRegion> regions;

    Private() {
        gid = 0;
        region_count = 0;
        picking_duration = 0;
    }
    Private(int _id, int _region_count, int _picking_duration) {
        gid = _id;
        region_count = _region_count;
        picking_duration = _picking_duration;
        isStarted = false;
    }
    void addRegion(PrivateRegion region) {
        regions.push_back(region);
    }
    void printPrivate() const {
        std::cout <<  std::endl << "gid: " << gid << std::endl;
        std::cout << "region_count: " << region_count << std::endl;
        std::cout << "picking_duration: " << picking_duration << std::endl;
    }
};
class Smoker {
public:
    int sid;

    struct timespec smoking_duration;
    int region_count;
    vector<SmokerRegion> regions;

    Smoker() {
        sid = 0;
        smoking_duration.tv_sec = 0;
    }
    Smoker(int _id, int _smoking_duration, int _region_count) {
        sid = _id;
        smoking_duration.tv_sec = _smoking_duration / 1000;
        region_count = _region_count;
    }
    void addRegion(SmokerRegion region) {
        regions.push_back(region);
    }
    void printSmoker() const {
        std::cout << std::endl << "sid" << sid << std::endl;
        std::cout << "smoking_duration" << smoking_duration.tv_sec << std::endl;
        std::cout << "region_count" << region_count << std::endl;
    }
};
class Grid {
public:
    int length;
    int height;
    int layout[100][100];
    int private_count;
    int order_count;
    int smoker_count;
    vector<Private> privates;
    vector<Order> orders;
    vector<Smoker> smokers;

    Grid() {
        length = 0;
        height = 0;
        private_count = 0;
        order_count = 0;
        smoker_count = 0;
    }
    Grid(int _length, int _height) {
        length = _length;
        height = _height;
    }
    void printGrid() {
        for (int vertical_index = 0; vertical_index < height; vertical_index++) {
            for (int horizontal_index = 0; horizontal_index < length; horizontal_index++) {
                std::cout << layout[vertical_index][horizontal_index] << ' ';
            }
            std::cout << std::endl;
        }
    }
    void printOrders() {
        for (int i = 0; i < order_count; i++) {
            std::cout << endl << "ORDER " << i << ":" << orders[i].time_stamp << ", " << orders[i].type << std::endl;
        }
    }

    int getPrivateIndex(int privateId) {
        for (int i = 0; i < private_count; i++) {
            if (privates[i].gid == privateId) {
                return i;
            }
        }
        return 0;
    }
};

#pragma region INITIALIZE_GLOBALS
Grid *grid = new Grid();
pthread_mutex_t gridMutex[1000][1000];
pthread_mutex_t canContinueMutex;
pthread_cond_t canContinue;
pthread_cond_t canBreak;
hw2_actions state;
#pragma endregion INITIALIZE_GLOBALS

void* pickRoutine(void* pindex)  {


    //cout << " -ROUTINE GIRDI- " << endl;
    //cout << "PINDEX->" << *((int*)(pindex)) << "<-";
    Private& privat= grid->privates[*((int*) pindex)];
    hw2_notify(PROPER_PRIVATE_CREATED, privat.gid, 0, 0);

    label:
    //cout << " -REGION COUNT = " << grid->privates[*((int*) pindex)].region_count << endl;
    for (int k = 0; k < privat.region_count; k++) {
        //lock whole region, wait for a private end its job
        for (int j = privat.regions[k].starting_index_y; j < privat.regions[k].starting_index_y + privat.regions[k].size_y; j++) {
            for (int i = privat.regions[k].starting_index_x; i < privat.regions[k].starting_index_x + privat.regions[k].size_x; i++) {
                //cout << " -BURAYA GIRDI- " << endl;
                pthread_mutex_lock(&gridMutex[j][i]);
            }
        }
        hw2_notify(PROPER_PRIVATE_ARRIVED, privat.gid,privat.regions[k].starting_index_y ,privat.regions[k].starting_index_x);

        for (int j = privat.regions[k].starting_index_y; j < privat.regions[k].starting_index_y + privat.regions[k].size_y; j++) {
            for (int i = privat.regions[k].starting_index_x; i < privat.regions[k].starting_index_x + privat.regions[k].size_x; i++) {
                while(grid->layout[j][i] > 0){
                    if (state == ORDER_CONTINUE){
                        timespec tempDuration;
                        struct timeval now ;
                        gettimeofday(&now, NULL);
                        //tempDuration.tv_sec= privat.picking_duration / 1000;
                        //tempDuration.tv_nsec=((privat.picking_duration % 1000) * 1000) * 1000;
                        //nanosleep(&tempDuration, &rem);
                        tempDuration.tv_sec=time(NULL) + privat.picking_duration / 1000;
                        tempDuration.tv_sec = tempDuration.tv_sec + tempDuration.tv_nsec / (1000*1000*1000);
                        tempDuration.tv_nsec=(now.tv_usec + (privat.picking_duration % 1000) * 1000) * 1000;
                        tempDuration.tv_nsec = tempDuration.tv_nsec % (1000*1000*1000);

                        int ret = pthread_cond_timedwait(&canBreak, &gridMutex[j][i], &tempDuration);

                        if (ret ==0){
                            break;
                        }
                        else if (ret == ETIMEDOUT){
                            grid->layout[j][i]--;
                            hw2_notify(PROPER_PRIVATE_GATHERED, privat.gid,j ,i);
                        }
                    }
                    else{
                        break;
                    }
                        //cout << endl << privat.picking_duration / 1000 << ", " << (privat.picking_duration % 1000) * 1000 << endl;
                        //free(tempDuration);
                }

                        //pthread_cond_wait(&canContinue, &gridMutex[j][i]);
                    //cout << " -TEMIZLIYOR- " << endl;
                    //grid->printGrid();
                    //cout << endl << privat.regions[k].size_y << "," << privat.regions[k].size_x << endl;
                    //cout << endl << privat.regions[k].starting_index_y << "," << privat.regions[k].starting_index_x << endl;
            }
            if(state == ORDER_BREAK || state == ORDER_STOP) {
                break;
            }
        }
        //cout << " -BURAYA GIRDI- " << endl;

        for (int j = privat.regions[k].starting_index_y; j < privat.regions[k].starting_index_y + privat.regions[k].size_y; j++) {
            for (int i = privat.regions[k].starting_index_x; i < privat.regions[k].starting_index_x + privat.regions[k].size_x; i++) {
                pthread_mutex_unlock(&gridMutex[j][i]);
            }
        }

        //cout << "CIKTI";
        if(state == ORDER_BREAK) {
            hw2_notify(PROPER_PRIVATE_TOOK_BREAK, privat.gid, 0, 0);
            pthread_mutex_lock(&canContinueMutex);
            pthread_cond_wait(&canContinue, &canContinueMutex);
            pthread_mutex_unlock(&canContinueMutex);
            hw2_notify(PROPER_PRIVATE_CONTINUED,privat.gid, 0, 0);
            goto label;
        }
        else if (state == ORDER_STOP){
            hw2_notify(PROPER_PRIVATE_STOPPED, privat.gid, 0, 0);
            pthread_exit(nullptr);
        }
        hw2_notify(PROPER_PRIVATE_CLEARED, privat.gid, 0, 0);
    }
    hw2_notify(PROPER_PRIVATE_EXITED, privat.gid, 0, 0);
    return (void*) 3;
}

int main()
{
#pragma region GRID_INPUT
    ///////// GRID INPUT ////////////////
    hw2_init_notifier();
    int horizontal_length = 0, horizontal_index = 0, vertical_length = 0, vertical_index = 0;
    std::cin >> vertical_length >> horizontal_length;

    grid->length = horizontal_length;
    grid->height = vertical_length;
    for (vertical_index = 0; vertical_index < vertical_length; vertical_index++) {
        for (horizontal_index = 0; horizontal_index < horizontal_length; horizontal_index++) {
            std::cin >> grid->layout[vertical_index][horizontal_index];
        }
    }
    //grid.printGrid();
#pragma endregion
#pragma region PRIVATE_INPUT
    ///////// PRIVATE INPUT ////////////
    int private_count = 0, private_index = 0;
    std::cin >> private_count;
    grid->private_count = private_count;
    for (private_index = 0; private_index < private_count; private_index++) {
        int private_id = 0, region_size_x = 0, region_size_y = 0, picking_duration = 0, region_count = 0;

        std::cin >> private_id >> region_size_y >> region_size_x >> picking_duration >> region_count;
        Private *newPrivate = new Private(private_id, region_count, picking_duration);
        for (int j = 0; j < region_count; j++) {
            int region_starting_index_x = 0, region_starting_index_y = 0;
            std::cin >> region_starting_index_y >> region_starting_index_x;

            PrivateRegion *newRegion = new PrivateRegion(region_size_y, region_size_x, region_starting_index_y, region_starting_index_x);
            newPrivate->regions.push_back(*newRegion);
            //newPrivate.printPrivate();
        }
        grid->privates.push_back(*newPrivate);
    }
#pragma endregion
#pragma region ORDER_INPUT
    ///////// ORDER INPUT ////////////
    int order_count = 0;
    long previous_time_stamp=0;
    std::cin >> order_count;
    grid->order_count = order_count;
    for (int i = 0; i < order_count; i++) {

        long time_stamp=0;
        std::cin >> time_stamp;
        Order *newOrder = new Order(time_stamp - previous_time_stamp);

        string order_str;
        std::cin >> order_str;
        if (order_str.compare("break") == 0) {
            newOrder->type = ORDER_BREAK;
        }
        else if (order_str.compare("continue") == 0) {
            newOrder->type = ORDER_CONTINUE;
        }
        else if (order_str.compare("stop") == 0) {
            newOrder->type = ORDER_STOP;
        }
        grid->orders.push_back(*newOrder);
        previous_time_stamp = time_stamp;
    }
#pragma endregion
#pragma region SMOKER_INPUT
    ///////// SMOKER INPUT ////////////
    int smoker_count = 0, smoker_index = 0;
    std::cin >> smoker_count;
    grid->smoker_count = smoker_count;
    for ( smoker_index = 0; smoker_index < smoker_count; smoker_index++) {
        int smoker_id = 0, smoking_duration = 0, region_count = 0;
            std::cin >> smoker_id >> smoking_duration >> region_count;

            Smoker newSmoker(smoker_id, smoking_duration, region_count);
            for (int j = 0; j < region_count; j++) {
                int region_starting_index_x = 0, region_starting_index_y = 0, cigs_to_smoke = 0;
                std::cin >> region_starting_index_y >> region_starting_index_x >> cigs_to_smoke;

                SmokerRegion newRegion(region_starting_index_y, region_starting_index_x, cigs_to_smoke);
                newSmoker.regions.push_back(newRegion);
            }
            grid->smokers.push_back(newSmoker);
            //newSmoker.printSmoker();
    }
    //cout << " -INPUTLARI ALDI- ";
#pragma endregion
#pragma region START_THREADS
    pthread_cond_init(&canContinue, nullptr);
    pthread_cond_init(&canBreak, nullptr);
    pthread_t pickingThreads[1000];
    for (int j = 0; j < grid->height; j++) {
        for (int i = 0; i < grid->length; i++) {
            pthread_mutex_init(&gridMutex[j][i], nullptr);
        }
    }

    for (int i = 0; i < grid->private_count; i++) {

        int *pindex = (int*) malloc(sizeof(int));
        *pindex = i;
        //cout << *pindex << "<-";
        if (pthread_create(&pickingThreads[i], nullptr, &pickRoutine, pindex) != 0) {
            perror("THREAD CREATE ERROR");
            cout << "THREAD CREATE ERROR";
            return 1;
        }
    }



    //cout << " -THREADLER BASLADI- " << endl;

#pragma endregion START_THREADS
#pragma region PROCESS_ORDERS

    state = ORDER_CONTINUE;
    hw2_actions previousState = ORDER_CONTINUE;
    bool isExited=false;
    //nanosleep(grid->orders[0].time_stamp, nullptr);
    //cout << "ORDER COUNT" << grid->orders[1].type << "DU";

    for(int i=0; i< grid->order_count; i++){
        if(!isExited){
            nanosleep(grid->orders[i].time_stamp, nullptr);
            state = grid->orders[i].type;
            //cout << endl << "STATE-" << i << " ORDER-" << state << " PREVIOUS-" << previousState << endl;
            if ( state == ORDER_CONTINUE) {
                if(previousState == ORDER_BREAK){
                    //cout << "CAN CONTINUE";
                    pthread_cond_broadcast(&canContinue);
                }
                else if(previousState == ORDER_CONTINUE){
                }
            }
            else if (state == ORDER_BREAK) {
                if(previousState == ORDER_BREAK){
                }
                else if(previousState == ORDER_CONTINUE){
                    //cout << "CAN BREAK";
                    pthread_cond_broadcast(&canBreak);
                }
            }
            else if (state == ORDER_STOP) {
                    isExited=true;
                    pthread_cond_broadcast(&canBreak);
                    break;
                    //terminate threads
                    //hw2_notify(PROPER_PRIVATE_EXITED, grid->privates[ii].gid, 0, 0);

            }
            previousState = state;
        }
    }
    for (int i = 0; i < grid->private_count; i++) {
        if (pthread_join(pickingThreads[i], nullptr) != 0)
        {
            perror("THREAD JOIN ERROR");
            return 2;
        }
    }

#pragma endregion PROCESS_ORDERS
}