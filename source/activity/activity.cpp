#include "manager.h"

#include <deque>
#include <memory>

namespace ActivityManager {

std::unique_ptr<Activity> current;
std::deque<std::unique_ptr<Activity>> stack;

}
