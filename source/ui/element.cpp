#include "element.hpp"

#include "manager.hpp"

UI::ElementBase::~ElementBase() {
    if (UI::Manager::GetFocusedElement() == this) {
        UI::Manager::ClearFocusedElement();
    }
}
