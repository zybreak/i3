#ifndef XPP_GENERIC_EVENT_HPP
#define XPP_GENERIC_EVENT_HPP

#include <memory> // shared_ptr
#include <xcb/xcb.h> // xcb_generic_event_t

namespace xpp::generic {

    template<typename Event>
    class event {
    public:
        event(const std::shared_ptr<xcb_generic_event_t> &event) : m_event(event) {
        }

        virtual ~event() = default;

        virtual operator const Event &() const {
            return reinterpret_cast<const Event &>(*m_event);
        }

        virtual const Event &operator*() const {
            return reinterpret_cast<const Event &>(*m_event);
        }

        virtual Event *operator->() const {
            return reinterpret_cast<Event *const>(m_event.get());
        }

    protected:
        std::shared_ptr<xcb_generic_event_t> m_event;
    };

}

#endif // XPP_GENERIC_EVENT_HPP
