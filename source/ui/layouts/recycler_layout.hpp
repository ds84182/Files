#pragma once

#include <deque>
#include <functional>
#include <limits>
#include <memory>
#include <vector>

#include <ui/layer.hpp>
#include <ui/layout.hpp>
#include <ui/manager.hpp>

#include <ui/elements/scroll_listener.hpp>

namespace UI {
namespace Layouts {

using UI::ElementBase;
using UI::Elements::ScrollListenerElement;
using UI::Elements::ScrollListener;

class RecyclerLayout : public Layout, public ScrollListener {
	std::deque<std::shared_ptr<ElementBase>> queue;
	std::shared_ptr<ScrollListenerElement> scrollListener;
	std::weak_ptr<ElementBase> focused;
	int top = 0; // the data index of the element at the top of the queue
	int scrollFeedbackAmount;

public:
	class Adapter {
	public:
		RecyclerLayout *layout;

		Adapter() {}
		virtual ~Adapter() = default;

		virtual std::shared_ptr<ElementBase> createElement() = 0;
		virtual void bindElement(std::shared_ptr<ElementBase> element, unsigned int position) = 0;
		virtual unsigned int count() = 0;

		void notifyChanged() {
			layout->clear();
			layout->update();
		}
	};

	Adapter *adapter;
	// Gets the size of an element
	//std::function<int(const Data&, int)> getElementSize;
	int elementSize; // < This is used if getElementSize is null
	std::function<void(int)> onSelected;
	Layer elementLayer;
	Layer scrollLayer;

	RecyclerLayout(Adapter *adapter);

	~RecyclerLayout();

	virtual int onScroll(int amount) override;

	virtual void forwardTouchStart(int x, int y) override;
	virtual void forwardTouchMove(int x, int y) override;
	virtual void forwardTouchEnd(int x, int y) override;
	virtual void forwardTap(int x, int y) override;

	void clear();

	virtual void update() override;

	virtual void getLayers(std::function<void(Layer*)> callback) override;
};

}
}
