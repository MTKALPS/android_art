/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ART_RUNTIME_THREAD_LIST_H_
#define ART_RUNTIME_THREAD_LIST_H_

#include "base/histogram.h"
#include "base/mutex.h"
#include "base/value_object.h"
#include "gc_root.h"
#include "jni.h"
#include "object_callbacks.h"

#include <bitset>
#include <list>

namespace art {
namespace gc {
  namespace collector {
    class GarbageCollector;
  }  // namespac collector
}  // namespace gc
class Closure;
class Thread;
class TimingLogger;

class ThreadList {
 public:
  static const uint32_t kMaxThreadId = 0xFFFF;
  static const uint32_t kInvalidThreadId = 0;
  static const uint32_t kMainThreadId = 1;

  explicit ThreadList();
  ~ThreadList();

  void DumpForSigQuit(std::ostream& os)
      REQUIRES(!Locks::thread_list_lock_, !Locks::mutator_lock_);
  // For thread suspend timeout dumps.
  void Dump(std::ostream& os, bool dump_native_stack = true)
      REQUIRES(!Locks::thread_list_lock_, !Locks::thread_suspend_count_lock_);
  pid_t GetLockOwner();  // For SignalCatcher.

  // Thread suspension support.
  void ResumeAll()
      REQUIRES(!Locks::thread_list_lock_, !Locks::thread_suspend_count_lock_)
      UNLOCK_FUNCTION(Locks::mutator_lock_);
  void Resume(Thread* thread, bool for_debugger = false)
      REQUIRES(!Locks::thread_suspend_count_lock_);

  // Suspends all threads and gets exclusive access to the mutator_lock_.
  // If long_suspend is true, then other threads who try to suspend will never timeout.
  // long_suspend is currenly used for hprof since large heaps take a long time.
  void SuspendAll(const char* cause, bool long_suspend = false)
      EXCLUSIVE_LOCK_FUNCTION(Locks::mutator_lock_)
      REQUIRES(!Locks::thread_list_lock_,
               !Locks::thread_suspend_count_lock_,
               !Locks::mutator_lock_);

  // Suspend a thread using a peer, typically used by the debugger. Returns the thread on success,
  // else null. The peer is used to identify the thread to avoid races with the thread terminating.
  // If the thread should be suspended then value of request_suspension should be true otherwise
  // the routine will wait for a previous suspend request. If the suspension times out then *timeout
  // is set to true.
  Thread* SuspendThreadByPeer(jobject peer, bool request_suspension, bool debug_suspension,
                              bool* timed_out)
      REQUIRES(!Locks::mutator_lock_,
               !Locks::thread_list_lock_,
               !Locks::thread_suspend_count_lock_);

  // Suspend a thread using its thread id, typically used by lock/monitor inflation. Returns the
  // thread on success else null. The thread id is used to identify the thread to avoid races with
  // the thread terminating. Note that as thread ids are recycled this may not suspend the expected
  // thread, that may be terminating. If the suspension times out then *timeout is set to true.
  Thread* SuspendThreadByThreadId(uint32_t thread_id, bool debug_suspension, bool* timed_out)
      REQUIRES(!Locks::mutator_lock_,
               !Locks::thread_list_lock_,
               !Locks::thread_suspend_count_lock_);

  // Find an existing thread (or self) by its thread id (not tid).
  Thread* FindThreadByThreadId(uint32_t thread_id) REQUIRES(Locks::thread_list_lock_);

  // Run a checkpoint on threads, running threads are not suspended but run the checkpoint inside
  // of the suspend check. Returns how many checkpoints that are expected to run, including for
  // already suspended threads for b/24191051.
  size_t RunCheckpoint(Closure* checkpoint_function)
      REQUIRES(!Locks::thread_list_lock_, !Locks::thread_suspend_count_lock_);

  size_t RunCheckpointOnRunnableThreads(Closure* checkpoint_function)
      REQUIRES(!Locks::thread_list_lock_, !Locks::thread_suspend_count_lock_);

  // Flip thread roots from from-space refs to to-space refs. Used by
  // the concurrent copying collector.
  size_t FlipThreadRoots(Closure* thread_flip_visitor,
                         Closure* flip_callback,
                         gc::collector::GarbageCollector* collector)
      REQUIRES(!Locks::mutator_lock_,
               !Locks::thread_list_lock_,
               !Locks::thread_suspend_count_lock_);

  // Suspends all threads
  void SuspendAllForDebugger()
      REQUIRES(!Locks::mutator_lock_,
               !Locks::thread_list_lock_,
               !Locks::thread_suspend_count_lock_);

  void SuspendSelfForDebugger()
      REQUIRES(!Locks::thread_suspend_count_lock_);

  // Resume all threads
  void ResumeAllForDebugger()
      REQUIRES(!Locks::thread_list_lock_, !Locks::thread_suspend_count_lock_);

  void UndoDebuggerSuspensions()
      REQUIRES(!Locks::thread_list_lock_, !Locks::thread_suspend_count_lock_);

  // Iterates over all the threads.
  void ForEach(void (*callback)(Thread*, void*), void* context)
      REQUIRES(Locks::thread_list_lock_);

  // Add/remove current thread from list.
  void Register(Thread* self)
      REQUIRES(Locks::runtime_shutdown_lock_)
      REQUIRES(!Locks::mutator_lock_,
               !Locks::thread_list_lock_,
               !Locks::thread_suspend_count_lock_);
  void Unregister(Thread* self)
      REQUIRES(!Locks::mutator_lock_,
               !Locks::thread_list_lock_,
               !Locks::thread_suspend_count_lock_);

  void VisitRoots(RootVisitor* visitor) const
      SHARED_REQUIRES(Locks::mutator_lock_);

  // Return a copy of the thread list.
  std::list<Thread*> GetList() REQUIRES(Locks::thread_list_lock_) {
    return list_;
  }

  void DumpNativeStacks(std::ostream& os)
      REQUIRES(!Locks::thread_list_lock_);

 private:
  uint32_t AllocThreadId(Thread* self);
  void ReleaseThreadId(Thread* self, uint32_t id) REQUIRES(!Locks::allocated_thread_ids_lock_);

  bool Contains(Thread* thread) REQUIRES(Locks::thread_list_lock_);
  bool Contains(pid_t tid) REQUIRES(Locks::thread_list_lock_);
#ifdef MTK_ART_FIX_THREAD_LIST_MEM_LEAKAGE
  std::list<Thread*>::iterator FindThread(Thread* thread) REQUIRES(Locks::thread_list_lock_);
#endif

  size_t RunCheckpoint(Closure* checkpoint_function, bool includeSuspended)
      REQUIRES(!Locks::thread_list_lock_, !Locks::thread_suspend_count_lock_);

  void DumpUnattachedThreads(std::ostream& os, bool dump_native_stack)
      REQUIRES(!Locks::thread_list_lock_);

  void SuspendAllDaemonThreadsForShutdown()
      REQUIRES(!Locks::thread_list_lock_, !Locks::thread_suspend_count_lock_);
  void WaitForOtherNonDaemonThreadsToExit()
      REQUIRES(!Locks::thread_list_lock_, !Locks::thread_suspend_count_lock_);

  void SuspendAllInternal(Thread* self,
                          Thread* ignore1,
                          Thread* ignore2 = nullptr,
                          bool debug_suspend = false)
      REQUIRES(!Locks::thread_list_lock_, !Locks::thread_suspend_count_lock_);

  void AssertThreadsAreSuspended(Thread* self, Thread* ignore1, Thread* ignore2 = nullptr)
      REQUIRES(!Locks::thread_list_lock_, !Locks::thread_suspend_count_lock_);

  std::bitset<kMaxThreadId> allocated_ids_ GUARDED_BY(Locks::allocated_thread_ids_lock_);

  // The actual list of all threads.
  std::list<Thread*> list_ GUARDED_BY(Locks::thread_list_lock_);

  // Ongoing suspend all requests, used to ensure threads added to list_ respect SuspendAll.
  int suspend_all_count_ GUARDED_BY(Locks::thread_suspend_count_lock_);
  int debug_suspend_all_count_ GUARDED_BY(Locks::thread_suspend_count_lock_);

  // Number of threads unregistering, ~ThreadList blocks until this hits 0.
  int unregistering_count_ GUARDED_BY(Locks::thread_list_lock_);

  // Thread suspend time histogram. Only modified when all the threads are suspended, so guarding
  // by mutator lock ensures no thread can read when another thread is modifying it.
  Histogram<uint64_t> suspend_all_historam_ GUARDED_BY(Locks::mutator_lock_);

  // Whether or not the current thread suspension is long.
  bool long_suspend_;

  friend class Thread;

  DISALLOW_COPY_AND_ASSIGN(ThreadList);
};

// Helper for suspending all threads and
class ScopedSuspendAll : public ValueObject {
 public:
  ScopedSuspendAll(const char* cause, bool long_suspend = false)
     EXCLUSIVE_LOCK_FUNCTION(Locks::mutator_lock_)
     REQUIRES(!Locks::thread_list_lock_,
              !Locks::thread_suspend_count_lock_,
              !Locks::mutator_lock_);
  // No REQUIRES(mutator_lock_) since the unlock function already asserts this.
  ~ScopedSuspendAll()
      REQUIRES(!Locks::thread_list_lock_, !Locks::thread_suspend_count_lock_)
      UNLOCK_FUNCTION(Locks::mutator_lock_);
};

}  // namespace art

#endif  // ART_RUNTIME_THREAD_LIST_H_
