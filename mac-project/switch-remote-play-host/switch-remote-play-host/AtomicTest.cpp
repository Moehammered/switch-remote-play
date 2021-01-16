#include "AtomicTest.h"
#include <thread>
#include <chrono>
#include <iostream>
using namespace std;
//atomic tests
const int UNINIT = -1;
const int LOADING = 0;
const int READY = 1;
const int FINISHED = 2;

void FlipToFinish(atomic<int>& state)
{
    while (state.load(memory_order_acquire) != READY);

    cout << "FlipToFinish is ready, gonna do stuff now" << endl;

    this_thread::sleep_for(chrono::duration<int, milli>(1000));
    state.store(FINISHED, memory_order_release);
}

void FlipToReady(atomic<int>& state)
{
    while (state.load(memory_order_acquire) != LOADING);

    cout << "t2 will load now." << endl;

    this_thread::sleep_for(chrono::duration<int, milli>(500));
    state.store(READY, memory_order_release);
}

void ReadState(atomic<int>& state)
{
    auto currState = state.load(memory_order_acquire);
    while (currState != FINISHED)
    {
        switch (currState)
        {
        case UNINIT:
            cout << "ReadState found uninit state -- changing to loading state" << endl;
            state.store(LOADING, memory_order_release);
            this_thread::sleep_for(chrono::duration<int, milli>(1000));
            break;
        case LOADING:
            cout << "ReadState found loading state... sleeping" << endl;
            this_thread::sleep_for(chrono::duration<int, milli>(100));
            break;
        case READY:
            cout << "ReadState found ready state... sleeping" << endl;
            this_thread::sleep_for(chrono::duration<int, milli>(100));
            break;
        case FINISHED:
            cout << "ReadState found finished state..." << endl;
            break;
        }
        currState = state.load(memory_order_acquire);
    }
}

void TestPassByRefAtomicThreads()
{
    atomic<int> localAtomic {UNINIT};
    auto t1 = thread(ReadState, ref(localAtomic));
    auto t2 = thread(FlipToReady, ref(localAtomic));
    auto t3 = thread(FlipToFinish, ref(localAtomic));

    t1.join();
    t2.join();
    t3.join();

    /*t1.detach();
    t2.detach();
    t3.detach();
    while (localAtomic.load(memory_order_acquire) != FINISHED);*/
}

void TestAtomicByReference()
{
    atomic<int> state {-1};
    const int UNINIT = -1;
    const int LOADING = 0;
    const int READY = 1;
    const int FINISHED = 2;

    auto t1 = thread([&] {
        while (state.load(memory_order_acquire) != READY);

        cout << "t1 is ready, gonna do stuff now" << endl;

        this_thread::sleep_for(chrono::duration<int, milli>(1000));
        state.store(FINISHED, memory_order_release);

        });

    auto t2 = thread([&] {
        while (state.load(memory_order_acquire) != LOADING);

        cout << "t2 will load now." << endl;

        this_thread::sleep_for(chrono::duration<int, milli>(500));
        state.store(READY, memory_order_release);

        });

    auto t3 = thread([&] {
        const int UNINIT = -1;
        const int LOADING = 0;
        const int READY = 1;
        const int FINISHED = 2;
        auto currState = state.load(memory_order_acquire);
        while (currState != FINISHED)
        {
            switch (currState)
            {
            case UNINIT:
                cout << "t3 found uninit state -- changing to loading state" << endl;
                state.store(LOADING, memory_order_release);
                this_thread::sleep_for(chrono::duration<int, milli>(1000));
                break;
            case LOADING:
                cout << "t3 found loading state... sleeping" << endl;
                this_thread::sleep_for(chrono::duration<int, milli>(100));
                break;
            case READY:
                cout << "t3 found ready state... sleeping" << endl;
                this_thread::sleep_for(chrono::duration<int, milli>(100));
                break;
            case FINISHED:
                cout << "t3 found finished state..." << endl;
                break;
            }
            currState = state.load(memory_order_acquire);
        }

        cout << "t3 has finished." << endl;
        });

    t1.join();
    t2.join();
    t3.join();
}
//atomic tests
