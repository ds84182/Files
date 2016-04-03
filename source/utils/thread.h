#pragma once

#include <functional>

#include <3ds.h>

template <typename T>
struct ThreadContext {
public:
	Thread thread;

	using ThreadEntryPoint = std::function<int(T)>;
	ThreadEntryPoint entrypoint;

	T argument;

	ThreadContext(
		ThreadEntryPoint entrypoint, T argument, size_t stackSize = 0x1000, int priority = 0x30, int affinity = -1)
		: entrypoint(entrypoint), argument(argument) {
		thread = threadCreate(threadContextEntryPoint, this, stackSize, priority, affinity, false);
	}

	~ThreadContext() {
		threadFree(thread);
	}

	int result() {
		return threadGetExitCode(thread);
	}

	bool join(u64 timeout_ns = U64_MAX) {
		return threadJoin(thread, timeout_ns) == 0;
	}

	static void threadContextEntryPoint(void *arg) {
		ThreadContext<T> *context = reinterpret_cast<ThreadContext<T>*>(arg);

		threadExit(context->entrypoint(context->argument));
	}
};
