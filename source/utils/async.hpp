#pragma once

#include "handler.hpp"

namespace Async {

template <typename T, typename R = void>
class Future {
public:
	Future(std::shared_ptr<Handler> handler = Current) : handler(handler) {}

	void complete(T &value) {
		if (handler && callback) {
			if (std::is_void<R>::value) {
				handler->postCallback([=]() {
					callback(value);
				});
			} else {
				handler->postCallback([=]() {
					R ret = callback(value);
				});
			}
		}
	}

private:
	T data;
	std::shared_ptr<Handler> handler;
	std::function<R(T)> callback;
};

}
