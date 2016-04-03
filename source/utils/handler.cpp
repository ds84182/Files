#include "handler.h"

#include <cstdio>

bool Handler::handleMessage() {
	lock();
	bool handled = handleMessageUnsafe();
	unlock();

	return handled;
}

bool Handler::handleMessageUnsafe() {
	auto message = peek();
	bool handled = false;

	if (message) {
		printf("Message %p (time %llu current %llu next %p)\n", *message, message->when, osGetTime(), *message->next);
	} else {
		printf("No message\n");
	}

	if (message && message->when <= osGetTime()) {
		if (message->callback) {
			message->callback();
		}

		if (callback) {
			callback(message);
		}

		handled = true;
		pop();
	}

	return handled;
}

void Handler::run() {
	lock();
	running = true;
	while (running) { // TODO: volatile bool for running state
		if (!handleMessageUnsafe()) {
			// If there is a message on the queue, find the reason why it didn't execute
			auto message = peek();
			u64 time = osGetTime();
			if (message && message->when > time) {
				u64 when = message->when;
				unlock();
				svcWaitSynchronization(eventHandle, (when-time)*1000000LL);
			} else {
				unlock();
				svcWaitSynchronization(eventHandle, UINT64_MAX);
			}

			lock();
		}
		svcClearEvent(eventHandle);
	}
}
