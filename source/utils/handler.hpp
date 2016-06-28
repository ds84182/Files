#pragma once

#include <3ds.h>

#include <functional>
#include <memory>

#include <utils/thread.hpp>

class Message;

class Messenger {
public:
	virtual void send(std::shared_ptr<Message> message);
};

class Message {
public:
	int what = 0;
	int arg1 = 0;
	int arg2 = 0;
	u64 when = 0;
	std::function<void()> callback;

	Message() {};
	Message(std::function<void()> callback, u64 when = 0) : when(when), callback(callback) {};
	Message(int what, int arg1 = 0, int arg2 = 0, u64 when = 0) :
		what(what), arg1(arg1), arg2(arg2), when(when) {};

private:
	std::shared_ptr<Message> next;
	std::shared_ptr<Messenger> replyTo;
	friend class Handler;
};

template <typename T>
class MessageWithObject : public Message {
public:
	T object;
};

class Handler {
public:
	std::function<void(std::shared_ptr<Message>)> callback;

	Handler() {init();};
	Handler(std::function<void(std::shared_ptr<Message>)> callback) : callback(callback) {init();};
	~Handler();

	// Runs the handler in a loop
	void run();
	// Handles a single message, if available.
	// If no message is available, this method returns false.
	bool handleMessage();

	// Posts a message onto the message queue depending on it's time.
	void post(std::shared_ptr<Message> m) {
		lock();

		if ((!messages) || messages->when > m->when) {
			m->next = messages;
			messages = m;
		} else {
			auto &prev = messages;
			while (prev->when < m->when && prev->next) {
				prev = prev->next;
			}
			prev->next = m;
			m->next.reset();
		}

		unlock();

		signal();
	}

	// Posts a message to the front of the message queue. This should only be used to instantly interrupt a thread.
	void postFront(std::shared_ptr<Message> m) {
		lock();

		if (!messages) {
			messages = m;
		} else {
			m->next = messages;
			messages = m;
		}
		m->when = 0; // lowest possible time for when

		unlock();

		signal();
	}

	// Posts a message to execute "now"
	void postNow(std::shared_ptr<Message> m) {
		m->when = osGetTime();
		post(m);
	}

	// Posts a message with a millisecond delay
	void postDelayed(std::shared_ptr<Message> m, u64 milliseconds) {
		m->when = osGetTime()+milliseconds;
		post(m);
	}

	// Posts a message at a certain time.
	// If this time is in the past the message will execute instantly.
	void postAt(std::shared_ptr<Message> m, u64 milliseconds) {
		m->when = milliseconds;
		post(m);
	}

	// Posts a callback onto the queue.
	void postCallback(std::function<void()> callback) {
		std::shared_ptr<Message> message = std::make_shared<Message>(callback);
		postNow(message);
	}

	// Posts a delayed callback onto the queue.
	void postCallbackDelayed(std::function<void()> callback, u64 milliseconds) {
		std::shared_ptr<Message> message = std::make_shared<Message>(callback);
		postDelayed(message, milliseconds);
	}

	// Posts a callback that executes at a certain point in time.
	void postCallbackAt(std::function<void()> callback, u64 milliseconds) {
		std::shared_ptr<Message> message = std::make_shared<Message>(callback);
		postAt(message, milliseconds);
	}

	// Stops the handler loop, if any.
	// TODO: stopSafe, which will process all messages that need to be processed now before quitting
	void stop() {
		running = false;

		signal(); // Signal the queue to continue, even if there is nothing left.
	}

	void stopSafe() {
		postCallback([&]() {
			running = false;
		});
	}

private:
	std::shared_ptr<Message> messages;
	LightLock lck;
	Handle eventHandle;
	volatile bool running = false;

	void init();

	void lock() {
		LightLock_Lock(&lck);
	}

	void unlock() {
		LightLock_Unlock(&lck);
	}

	void signal() {
		svcSignalEvent(eventHandle);
	}

	void pop() {
		if (messages) {
			messages = messages->next;
		}
	}

	std::shared_ptr<Message> peek() {
		return messages;
	}

	bool handleMessageUnsafe();
};

extern thread_local std::shared_ptr<Handler> Current;

class HandlerThread : ThreadContext<std::shared_ptr<Handler>> {
public:
	std::shared_ptr<Handler> handler;

	HandlerThread(std::shared_ptr<Handler> handler) : ThreadContext([=](std::shared_ptr<Handler> handler) {
    Current = handler;
		handler->run();
		Current.reset();
		delete this;
		return 0;
	}, handler, true), handler(handler) {}
};
