// Copyright 2018 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef GVISOR_TEST_UTIL_SIGNAL_UTIL_H_
#define GVISOR_TEST_UTIL_SIGNAL_UTIL_H_

#include <signal.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <ostream>

#include "gmock/gmock.h"
#include "test/util/cleanup.h"
#include "test/util/posix_error.h"

// Format a sigset_t as a comma separated list of numeric ranges.
::std::ostream& operator<<(::std::ostream& os, const sigset_t& sigset);

namespace gvisor {
namespace testing {

// The maximum signal number.
static constexpr int kMaxSignal = 64;

// Wrapper for the tgkill(2) syscall, which glibc does not provide.
inline int tgkill(pid_t tgid, pid_t tid, int sig) {
  return syscall(__NR_tgkill, tgid, tid, sig);
}

// Installs the passed sigaction and returns a cleanup function to restore the
// previous handler when it goes out of scope.
PosixErrorOr<Cleanup> ScopedSigaction(int sig, struct sigaction const& sa);

// Updates the signal mask as per sigprocmask(2) and returns a cleanup function
// to restore the previous signal mask when it goes out of scope.
PosixErrorOr<Cleanup> ScopedSignalMask(int how, sigset_t const& set);

// ScopedSignalMask variant that creates a mask of the single signal 'sig'.
inline PosixErrorOr<Cleanup> ScopedSignalMask(int how, int sig) {
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, sig);
  return ScopedSignalMask(how, set);
}

// Asserts equality of two sigset_t values.
MATCHER_P(EqualsSigset, value, "equals " + ::testing::PrintToString(value)) {
  for (int sig = 1; sig <= kMaxSignal; ++sig) {
    if (sigismember(&arg, sig) != sigismember(&value, sig)) {
      return false;
    }
  }
  return true;
}

#ifdef __x86_64__
// Fault can be used to generate a synchronous SIGSEGV.
//
// This fault can be fixed up in a handler via fixup, below.
inline void Fault() {
  // Zero and dereference %ax.
  asm("movabs $0, %%rax\r\n"
      "mov 0(%%rax), %%rax\r\n"
      :
      :
      : "ax");
}

// FixupFault fixes up a fault generated by fault, above.
inline void FixupFault(ucontext* ctx) {
  // Skip the bad instruction above.
  //
  // The encoding is 0x48 0xab 0x00.
  ctx->uc_mcontext.gregs[REG_RIP] += 3;
}
#endif

}  // namespace testing
}  // namespace gvisor

#endif  // GVISOR_TEST_UTIL_SIGNAL_UTIL_H_
