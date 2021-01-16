#pragma once
#include <atomic>

void FlipToFinish(std::atomic<int>& state);
void FlipToReady(std::atomic<int>& state);
void ReadState(std::atomic<int>& state);
void TestPassByRefAtomicThreads();
void TestAtomicByReference();