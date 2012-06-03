// Copyright (c) 2011-2012, Zeex
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "os.h"
#include "x86stacktrace.h"

static const int kMaxSymbolNameLength = 256;

X86StackFrame::X86StackFrame(void *frmAddr, void *retAddr, const std::string &name)
	: frmAddr_(frmAddr), retAddr_(retAddr), name_(name)
{
}

std::string X86StackFrame::GetString() const {
	std::stringstream stream;

	stream << std::hex << std::setw(8) << std::setfill('0') 
		<< reinterpret_cast<long>(retAddr_) << std::dec;
	if (!name_.empty()) {
		stream << " in " << name_ << " ()";
	} else {
		stream << " in ?? ()";
	}

	return stream.str();
}

static inline void *GetReturnAddress(void *frmAddr) {
	return *reinterpret_cast<void**>(reinterpret_cast<char*>(frmAddr) + 4);
}

static inline void *GetNextFrame(void *frmAddr) {
	return *reinterpret_cast<void**>(frmAddr);
}

X86StackTrace::X86StackTrace(int framesToSkip)
	: frames_()
{
	void *frmAddr;
	void *retAddr;

	void *stackTop = 0;
	void *stackBot = 0;

	int frameCount = 0;

	#if defined _MSC_VER
		__asm {
			mov dword ptr [frmAddr], ebp
			mov eax, fs:[0x04]
			mov dword ptr [stackTop], eax
			mov eax, fs:[0x08]
			mov dword ptr [stackBot], eax
		}
	#elif defined __GNUC__
		__asm__ __volatile__(
			"movl %%ebp, %0;" : "=r"(frmAddr) : : );
		#if defined WIN32
			__asm__ __volatile__(
				"movl %%fs:(0x04), %0;"
				"movl %%fs:(0x08), %1;"
				: "=r"(stackTop), "=r"(stackBot) : : );
		#endif
	#endif

	do {
		if ((frmAddr == 0)
			|| (frmAddr >= stackTop && stackTop != 0)
		    || (frmAddr < stackBot && stackBot != 0)) {
			break;
		}
		retAddr = GetReturnAddress(frmAddr);
		if (retAddr == 0) {
			break;
		}
		frmAddr = GetNextFrame(frmAddr);
		if (++frameCount <= framesToSkip) {
			continue;
		}
		frames_.push_back(X86StackFrame(frmAddr, retAddr, os::GetSymbolName(retAddr)));
	} while (true);
}
