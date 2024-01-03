#ifndef XPP_INPUT_HPP
#define XPP_INPUT_HPP

#include <string>
#include <vector>

#include <xcb/xinput.h>

#include "xpp/generic.hpp"

namespace xpp::input {

template<typename Derived, typename Connection>
class interface;

namespace event {
template<typename Connection>
class dispatcher;
}

namespace error {
class dispatcher;
}

class extension
  : public xpp::generic::extension<extension, &xcb_input_id>
 {
  public:
    using base = xpp::generic::extension<extension, &xcb_input_id>;
    using base::base;
    template<typename Derived, typename Connection>
    using interface = xpp::input::interface<Derived, Connection>;
    template<typename Connection>
    using event_dispatcher = xpp::input::event::dispatcher<Connection>;
    using error_dispatcher = xpp::input::error::dispatcher;
};

namespace event {
template<typename Connection>
class device_valuator : public xpp::generic::event<xcb_input_device_valuator_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_device_valuator_event_t> base;

    template<typename C>
    device_valuator(C && c,
                    uint8_t first_event,
                    const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~device_valuator() = default;

    static uint8_t opcode() {
      return XCB_INPUT_DEVICE_VALUATOR;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_DEVICE_VALUATOR");
    }

    uint8_t first_event() {
      return m_first_event;
    }
  protected:
    Connection m_c;
    const uint8_t m_first_event;
};
}


namespace event {
template<typename Connection>
class device_key_press : public xpp::generic::event<xcb_input_device_key_press_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_device_key_press_event_t> base;

    template<typename C>
    device_key_press(C && c,
                     uint8_t first_event,
                     const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~device_key_press() = default;

    static uint8_t opcode() {
      return XCB_INPUT_DEVICE_KEY_PRESS;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_DEVICE_KEY_PRESS");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class device_key_release : public xpp::generic::event<xcb_input_device_key_press_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_device_key_press_event_t> base;

    template<typename C>
    device_key_release(C && c,
                       uint8_t first_event,
                       const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~device_key_release() = default;

    static uint8_t opcode() {
      return XCB_INPUT_DEVICE_KEY_RELEASE;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_DEVICE_KEY_RELEASE");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class device_button_press : public xpp::generic::event<xcb_input_device_key_press_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_device_key_press_event_t> base;

    template<typename C>
    device_button_press(C && c,
                        uint8_t first_event,
                        const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~device_button_press() = default;

    static uint8_t opcode() {
      return XCB_INPUT_DEVICE_BUTTON_PRESS;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_DEVICE_BUTTON_PRESS");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class device_button_release : public xpp::generic::event<xcb_input_device_key_press_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_device_key_press_event_t> base;

    template<typename C>
    device_button_release(C && c,
                          uint8_t first_event,
                          const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~device_button_release() = default;

    static uint8_t opcode() {
      return XCB_INPUT_DEVICE_BUTTON_RELEASE;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_DEVICE_BUTTON_RELEASE");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class device_motion_notify : public xpp::generic::event<xcb_input_device_key_press_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_device_key_press_event_t> base;

    template<typename C>
    device_motion_notify(C && c,
                         uint8_t first_event,
                         const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~device_motion_notify() = default;

    static uint8_t opcode() {
      return XCB_INPUT_DEVICE_MOTION_NOTIFY;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_DEVICE_MOTION_NOTIFY");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class device_focus_in : public xpp::generic::event<xcb_input_device_focus_in_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_device_focus_in_event_t> base;

    template<typename C>
    device_focus_in(C && c,
                    uint8_t first_event,
                    const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~device_focus_in() = default;

    static uint8_t opcode() {
      return XCB_INPUT_DEVICE_FOCUS_IN;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_DEVICE_FOCUS_IN");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType window(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->window),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->window,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class device_focus_out : public xpp::generic::event<xcb_input_device_focus_in_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_device_focus_in_event_t> base;

    template<typename C>
    device_focus_out(C && c,
                     uint8_t first_event,
                     const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~device_focus_out() = default;

    static uint8_t opcode() {
      return XCB_INPUT_DEVICE_FOCUS_OUT;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_DEVICE_FOCUS_OUT");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType window(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->window),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->window,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class proximity_in : public xpp::generic::event<xcb_input_device_key_press_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_device_key_press_event_t> base;

    template<typename C>
    proximity_in(C && c,
                 uint8_t first_event,
                 const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~proximity_in() = default;

    static uint8_t opcode() {
      return XCB_INPUT_PROXIMITY_IN;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_PROXIMITY_IN");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class proximity_out : public xpp::generic::event<xcb_input_device_key_press_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_device_key_press_event_t> base;

    template<typename C>
    proximity_out(C && c,
                  uint8_t first_event,
                  const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~proximity_out() = default;

    static uint8_t opcode() {
      return XCB_INPUT_PROXIMITY_OUT;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_PROXIMITY_OUT");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class device_state_notify : public xpp::generic::event<xcb_input_device_state_notify_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_device_state_notify_event_t> base;

    template<typename C>
    device_state_notify(C && c,
                        uint8_t first_event,
                        const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~device_state_notify() = default;

    static uint8_t opcode() {
      return XCB_INPUT_DEVICE_STATE_NOTIFY;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_DEVICE_STATE_NOTIFY");
    }

    uint8_t first_event() {
      return m_first_event;
    }
  protected:
    Connection m_c;
    const uint8_t m_first_event;
};
}


namespace event {
template<typename Connection>
class device_mapping_notify : public xpp::generic::event<xcb_input_device_mapping_notify_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_device_mapping_notify_event_t> base;

    template<typename C>
    device_mapping_notify(C && c,
                          uint8_t first_event,
                          const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~device_mapping_notify() = default;

    static uint8_t opcode() {
      return XCB_INPUT_DEVICE_MAPPING_NOTIFY;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_DEVICE_MAPPING_NOTIFY");
    }

    uint8_t first_event() {
      return m_first_event;
    }
  protected:
    Connection m_c;
    const uint8_t m_first_event;
};
}


namespace event {
template<typename Connection>
class change_device_notify : public xpp::generic::event<xcb_input_change_device_notify_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_change_device_notify_event_t> base;

    template<typename C>
    change_device_notify(C && c,
                         uint8_t first_event,
                         const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~change_device_notify() = default;

    static uint8_t opcode() {
      return XCB_INPUT_CHANGE_DEVICE_NOTIFY;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_CHANGE_DEVICE_NOTIFY");
    }

    uint8_t first_event() {
      return m_first_event;
    }
  protected:
    Connection m_c;
    const uint8_t m_first_event;
};
}


namespace event {
template<typename Connection>
class device_key_state_notify : public xpp::generic::event<xcb_input_device_key_state_notify_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_device_key_state_notify_event_t> base;

    template<typename C>
    device_key_state_notify(C && c,
                            uint8_t first_event,
                            const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~device_key_state_notify() = default;

    static uint8_t opcode() {
      return XCB_INPUT_DEVICE_KEY_STATE_NOTIFY;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_DEVICE_KEY_STATE_NOTIFY");
    }

    uint8_t first_event() {
      return m_first_event;
    }
  protected:
    Connection m_c;
    const uint8_t m_first_event;
};
}


namespace event {
template<typename Connection>
class device_button_state_notify : public xpp::generic::event<xcb_input_device_button_state_notify_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_device_button_state_notify_event_t> base;

    template<typename C>
    device_button_state_notify(C && c,
                               uint8_t first_event,
                               const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~device_button_state_notify() = default;

    static uint8_t opcode() {
      return XCB_INPUT_DEVICE_BUTTON_STATE_NOTIFY;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_DEVICE_BUTTON_STATE_NOTIFY");
    }

    uint8_t first_event() {
      return m_first_event;
    }
  protected:
    Connection m_c;
    const uint8_t m_first_event;
};
}


namespace event {
template<typename Connection>
class device_presence_notify : public xpp::generic::event<xcb_input_device_presence_notify_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_device_presence_notify_event_t> base;

    template<typename C>
    device_presence_notify(C && c,
                           uint8_t first_event,
                           const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~device_presence_notify() = default;

    static uint8_t opcode() {
      return XCB_INPUT_DEVICE_PRESENCE_NOTIFY;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_DEVICE_PRESENCE_NOTIFY");
    }

    uint8_t first_event() {
      return m_first_event;
    }
  protected:
    Connection m_c;
    const uint8_t m_first_event;
};
}


namespace event {
template<typename Connection>
class device_property_notify : public xpp::generic::event<xcb_input_device_property_notify_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_device_property_notify_event_t> base;

    template<typename C>
    device_property_notify(C && c,
                           uint8_t first_event,
                           const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~device_property_notify() = default;

    static uint8_t opcode() {
      return XCB_INPUT_DEVICE_PROPERTY_NOTIFY;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_DEVICE_PROPERTY_NOTIFY");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_atom_t, typename ... Parameter>
    ReturnType property(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->property),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->property,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class device_changed : public xpp::generic::event<xcb_input_device_changed_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_device_changed_event_t> base;

    template<typename C>
    device_changed(C && c,
                   uint8_t first_event,
                   const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~device_changed() = default;

    static uint8_t opcode() {
      return XCB_INPUT_DEVICE_CHANGED;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_DEVICE_CHANGED");
    }

    uint8_t first_event() {
      return m_first_event;
    }
  protected:
    Connection m_c;
    const uint8_t m_first_event;
};
}


namespace event {
template<typename Connection>
class key_press : public xpp::generic::event<xcb_input_key_press_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_key_press_event_t> base;

    template<typename C>
    key_press(C && c,
              uint8_t first_event,
              const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~key_press() = default;

    static uint8_t opcode() {
      return XCB_INPUT_KEY_PRESS;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_KEY_PRESS");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class key_release : public xpp::generic::event<xcb_input_key_press_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_key_press_event_t> base;

    template<typename C>
    key_release(C && c,
                uint8_t first_event,
                const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~key_release() = default;

    static uint8_t opcode() {
      return XCB_INPUT_KEY_RELEASE;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_KEY_RELEASE");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class button_press : public xpp::generic::event<xcb_input_button_press_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_button_press_event_t> base;

    template<typename C>
    button_press(C && c,
                 uint8_t first_event,
                 const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~button_press() = default;

    static uint8_t opcode() {
      return XCB_INPUT_BUTTON_PRESS;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_BUTTON_PRESS");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class button_release : public xpp::generic::event<xcb_input_button_press_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_button_press_event_t> base;

    template<typename C>
    button_release(C && c,
                   uint8_t first_event,
                   const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~button_release() = default;

    static uint8_t opcode() {
      return XCB_INPUT_BUTTON_RELEASE;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_BUTTON_RELEASE");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class motion : public xpp::generic::event<xcb_input_button_press_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_button_press_event_t> base;

    template<typename C>
    motion(C && c,
           uint8_t first_event,
           const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~motion() = default;

    static uint8_t opcode() {
      return XCB_INPUT_MOTION;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_MOTION");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class enter : public xpp::generic::event<xcb_input_enter_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_enter_event_t> base;

    template<typename C>
    enter(C && c,
          uint8_t first_event,
          const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~enter() = default;

    static uint8_t opcode() {
      return XCB_INPUT_ENTER;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_ENTER");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class leave : public xpp::generic::event<xcb_input_enter_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_enter_event_t> base;

    template<typename C>
    leave(C && c,
          uint8_t first_event,
          const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~leave() = default;

    static uint8_t opcode() {
      return XCB_INPUT_LEAVE;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_LEAVE");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class focus_in : public xpp::generic::event<xcb_input_enter_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_enter_event_t> base;

    template<typename C>
    focus_in(C && c,
             uint8_t first_event,
             const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~focus_in() = default;

    static uint8_t opcode() {
      return XCB_INPUT_FOCUS_IN;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_FOCUS_IN");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class focus_out : public xpp::generic::event<xcb_input_enter_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_enter_event_t> base;

    template<typename C>
    focus_out(C && c,
              uint8_t first_event,
              const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~focus_out() = default;

    static uint8_t opcode() {
      return XCB_INPUT_FOCUS_OUT;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_FOCUS_OUT");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class hierarchy : public xpp::generic::event<xcb_input_hierarchy_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_hierarchy_event_t> base;

    template<typename C>
    hierarchy(C && c,
              uint8_t first_event,
              const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~hierarchy() = default;

    static uint8_t opcode() {
      return XCB_INPUT_HIERARCHY;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_HIERARCHY");
    }

    uint8_t first_event() {
      return m_first_event;
    }
  protected:
    Connection m_c;
    const uint8_t m_first_event;
};
}


namespace event {
template<typename Connection>
class property : public xpp::generic::event<xcb_input_property_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_property_event_t> base;

    template<typename C>
    property(C && c,
             uint8_t first_event,
             const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~property() = default;

    static uint8_t opcode() {
      return XCB_INPUT_PROPERTY;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_PROPERTY");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_atom_t, typename ... Parameter>
    ReturnType property_(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->property),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->property,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class raw_key_press : public xpp::generic::event<xcb_input_raw_key_press_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_raw_key_press_event_t> base;

    template<typename C>
    raw_key_press(C && c,
                  uint8_t first_event,
                  const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~raw_key_press() = default;

    static uint8_t opcode() {
      return XCB_INPUT_RAW_KEY_PRESS;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_RAW_KEY_PRESS");
    }

    uint8_t first_event() {
      return m_first_event;
    }
  protected:
    Connection m_c;
    const uint8_t m_first_event;
};
}


namespace event {
template<typename Connection>
class raw_key_release : public xpp::generic::event<xcb_input_raw_key_press_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_raw_key_press_event_t> base;

    template<typename C>
    raw_key_release(C && c,
                    uint8_t first_event,
                    const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~raw_key_release() = default;

    static uint8_t opcode() {
      return XCB_INPUT_RAW_KEY_RELEASE;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_RAW_KEY_RELEASE");
    }

    uint8_t first_event() {
      return m_first_event;
    }
  protected:
    Connection m_c;
    const uint8_t m_first_event;
};
}


namespace event {
template<typename Connection>
class raw_button_press : public xpp::generic::event<xcb_input_raw_button_press_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_raw_button_press_event_t> base;

    template<typename C>
    raw_button_press(C && c,
                     uint8_t first_event,
                     const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~raw_button_press() = default;

    static uint8_t opcode() {
      return XCB_INPUT_RAW_BUTTON_PRESS;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_RAW_BUTTON_PRESS");
    }

    uint8_t first_event() {
      return m_first_event;
    }
  protected:
    Connection m_c;
    const uint8_t m_first_event;
};
}


namespace event {
template<typename Connection>
class raw_button_release : public xpp::generic::event<xcb_input_raw_button_press_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_raw_button_press_event_t> base;

    template<typename C>
    raw_button_release(C && c,
                       uint8_t first_event,
                       const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~raw_button_release() = default;

    static uint8_t opcode() {
      return XCB_INPUT_RAW_BUTTON_RELEASE;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_RAW_BUTTON_RELEASE");
    }

    uint8_t first_event() {
      return m_first_event;
    }
  protected:
    Connection m_c;
    const uint8_t m_first_event;
};
}


namespace event {
template<typename Connection>
class raw_motion : public xpp::generic::event<xcb_input_raw_button_press_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_raw_button_press_event_t> base;

    template<typename C>
    raw_motion(C && c,
               uint8_t first_event,
               const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~raw_motion() = default;

    static uint8_t opcode() {
      return XCB_INPUT_RAW_MOTION;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_RAW_MOTION");
    }

    uint8_t first_event() {
      return m_first_event;
    }
  protected:
    Connection m_c;
    const uint8_t m_first_event;
};
}


namespace event {
template<typename Connection>
class touch_begin : public xpp::generic::event<xcb_input_touch_begin_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_touch_begin_event_t> base;

    template<typename C>
    touch_begin(C && c,
                uint8_t first_event,
                const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~touch_begin() = default;

    static uint8_t opcode() {
      return XCB_INPUT_TOUCH_BEGIN;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_TOUCH_BEGIN");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class touch_update : public xpp::generic::event<xcb_input_touch_begin_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_touch_begin_event_t> base;

    template<typename C>
    touch_update(C && c,
                 uint8_t first_event,
                 const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~touch_update() = default;

    static uint8_t opcode() {
      return XCB_INPUT_TOUCH_UPDATE;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_TOUCH_UPDATE");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class touch_end : public xpp::generic::event<xcb_input_touch_begin_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_touch_begin_event_t> base;

    template<typename C>
    touch_end(C && c,
              uint8_t first_event,
              const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~touch_end() = default;

    static uint8_t opcode() {
      return XCB_INPUT_TOUCH_END;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_TOUCH_END");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class touch_ownership : public xpp::generic::event<xcb_input_touch_ownership_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_touch_ownership_event_t> base;

    template<typename C>
    touch_ownership(C && c,
                    uint8_t first_event,
                    const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~touch_ownership() = default;

    static uint8_t opcode() {
      return XCB_INPUT_TOUCH_OWNERSHIP;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_TOUCH_OWNERSHIP");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class raw_touch_begin : public xpp::generic::event<xcb_input_raw_touch_begin_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_raw_touch_begin_event_t> base;

    template<typename C>
    raw_touch_begin(C && c,
                    uint8_t first_event,
                    const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~raw_touch_begin() = default;

    static uint8_t opcode() {
      return XCB_INPUT_RAW_TOUCH_BEGIN;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_RAW_TOUCH_BEGIN");
    }

    uint8_t first_event() {
      return m_first_event;
    }
  protected:
    Connection m_c;
    const uint8_t m_first_event;
};
}


namespace event {
template<typename Connection>
class raw_touch_update : public xpp::generic::event<xcb_input_raw_touch_begin_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_raw_touch_begin_event_t> base;

    template<typename C>
    raw_touch_update(C && c,
                     uint8_t first_event,
                     const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~raw_touch_update() = default;

    static uint8_t opcode() {
      return XCB_INPUT_RAW_TOUCH_UPDATE;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_RAW_TOUCH_UPDATE");
    }

    uint8_t first_event() {
      return m_first_event;
    }
  protected:
    Connection m_c;
    const uint8_t m_first_event;
};
}


namespace event {
template<typename Connection>
class raw_touch_end : public xpp::generic::event<xcb_input_raw_touch_begin_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_raw_touch_begin_event_t> base;

    template<typename C>
    raw_touch_end(C && c,
                  uint8_t first_event,
                  const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~raw_touch_end() = default;

    static uint8_t opcode() {
      return XCB_INPUT_RAW_TOUCH_END;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_RAW_TOUCH_END");
    }

    uint8_t first_event() {
      return m_first_event;
    }
  protected:
    Connection m_c;
    const uint8_t m_first_event;
};
}


namespace event {
template<typename Connection>
class barrier_hit : public xpp::generic::event<xcb_input_barrier_hit_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_barrier_hit_event_t> base;

    template<typename C>
    barrier_hit(C && c,
                uint8_t first_event,
                const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~barrier_hit() = default;

    static uint8_t opcode() {
      return XCB_INPUT_BARRIER_HIT;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_BARRIER_HIT");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_xfixes_barrier_t, typename ... Parameter>
    ReturnType barrier(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->barrier),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->barrier,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class barrier_leave : public xpp::generic::event<xcb_input_barrier_hit_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_barrier_hit_event_t> base;

    template<typename C>
    barrier_leave(C && c,
                  uint8_t first_event,
                  const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~barrier_leave() = default;

    static uint8_t opcode() {
      return XCB_INPUT_BARRIER_LEAVE;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_BARRIER_LEAVE");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_xfixes_barrier_t, typename ... Parameter>
    ReturnType barrier(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->barrier),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->barrier,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class gesture_pinch_begin : public xpp::generic::event<xcb_input_gesture_pinch_begin_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_gesture_pinch_begin_event_t> base;

    template<typename C>
    gesture_pinch_begin(C && c,
                        uint8_t first_event,
                        const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~gesture_pinch_begin() = default;

    static uint8_t opcode() {
      return XCB_INPUT_GESTURE_PINCH_BEGIN;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_GESTURE_PINCH_BEGIN");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class gesture_pinch_update : public xpp::generic::event<xcb_input_gesture_pinch_begin_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_gesture_pinch_begin_event_t> base;

    template<typename C>
    gesture_pinch_update(C && c,
                         uint8_t first_event,
                         const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~gesture_pinch_update() = default;

    static uint8_t opcode() {
      return XCB_INPUT_GESTURE_PINCH_UPDATE;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_GESTURE_PINCH_UPDATE");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class gesture_pinch_end : public xpp::generic::event<xcb_input_gesture_pinch_begin_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_gesture_pinch_begin_event_t> base;

    template<typename C>
    gesture_pinch_end(C && c,
                      uint8_t first_event,
                      const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~gesture_pinch_end() = default;

    static uint8_t opcode() {
      return XCB_INPUT_GESTURE_PINCH_END;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_GESTURE_PINCH_END");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class gesture_swipe_begin : public xpp::generic::event<xcb_input_gesture_swipe_begin_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_gesture_swipe_begin_event_t> base;

    template<typename C>
    gesture_swipe_begin(C && c,
                        uint8_t first_event,
                        const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~gesture_swipe_begin() = default;

    static uint8_t opcode() {
      return XCB_INPUT_GESTURE_SWIPE_BEGIN;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_GESTURE_SWIPE_BEGIN");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class gesture_swipe_update : public xpp::generic::event<xcb_input_gesture_swipe_begin_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_gesture_swipe_begin_event_t> base;

    template<typename C>
    gesture_swipe_update(C && c,
                         uint8_t first_event,
                         const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~gesture_swipe_update() = default;

    static uint8_t opcode() {
      return XCB_INPUT_GESTURE_SWIPE_UPDATE;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_GESTURE_SWIPE_UPDATE");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class gesture_swipe_end : public xpp::generic::event<xcb_input_gesture_swipe_begin_event_t> {
  public:
    typedef xpp::input::extension extension;

    typedef xpp::generic::event<xcb_input_gesture_swipe_begin_event_t> base;

    template<typename C>
    gesture_swipe_end(C && c,
                      uint8_t first_event,
                      const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~gesture_swipe_end() = default;

    static uint8_t opcode() {
      return XCB_INPUT_GESTURE_SWIPE_END;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_INPUT_GESTURE_SWIPE_END");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}



namespace error {
class device : public xpp::generic::error<device, xcb_input_device_error_t> {
  public:
    using xpp::generic::error<device, xcb_input_device_error_t>::error;

    virtual ~device() = default;

    static uint8_t opcode() {
      return XCB_INPUT_DEVICE;
    }
    
    static uint8_t opcode(uint8_t first_error) {
      return first_error + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_error);
    }

    static std::string description() {
      return std::string("XCB_INPUT_DEVICE");
    }

  protected:
    uint8_t m_first_error;
};
} 


namespace error {
class event : public xpp::generic::error<event, xcb_input_event_error_t> {
  public:
    using xpp::generic::error<event, xcb_input_event_error_t>::error;

    virtual ~event() = default;

    static uint8_t opcode() {
      return XCB_INPUT_EVENT;
    }
    
    static uint8_t opcode(uint8_t first_error) {
      return first_error + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_error);
    }

    static std::string description() {
      return std::string("XCB_INPUT_EVENT");
    }

  protected:
    uint8_t m_first_error;
};
} 


namespace error {
class mode : public xpp::generic::error<mode, xcb_input_mode_error_t> {
  public:
    using xpp::generic::error<mode, xcb_input_mode_error_t>::error;

    virtual ~mode() = default;

    static uint8_t opcode() {
      return XCB_INPUT_MODE;
    }
    
    static uint8_t opcode(uint8_t first_error) {
      return first_error + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_error);
    }

    static std::string description() {
      return std::string("XCB_INPUT_MODE");
    }

  protected:
    uint8_t m_first_error;
};
} 


namespace error {
class device_busy : public xpp::generic::error<device_busy, xcb_input_device_busy_error_t> {
  public:
    using xpp::generic::error<device_busy, xcb_input_device_busy_error_t>::error;

    virtual ~device_busy() = default;

    static uint8_t opcode() {
      return XCB_INPUT_DEVICE_BUSY;
    }
    
    static uint8_t opcode(uint8_t first_error) {
      return first_error + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_error);
    }

    static std::string description() {
      return std::string("XCB_INPUT_DEVICE_BUSY");
    }

  protected:
    uint8_t m_first_error;
};
} 


namespace error {
class _class : public xpp::generic::error<_class, xcb_input_class_error_t> {
  public:
    using xpp::generic::error<_class, xcb_input_class_error_t>::error;

    virtual ~_class() = default;

    static uint8_t opcode() {
      return XCB_INPUT_CLASS;
    }
    
    static uint8_t opcode(uint8_t first_error) {
      return first_error + opcode();
    }
    
    static uint8_t opcode(const xpp::input::extension & extension) {
      return opcode(extension->first_error);
    }

    static std::string description() {
      return std::string("XCB_INPUT_CLASS");
    }

  protected:
    uint8_t m_first_error;
};
} 


namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_extension_version : public xpp::generic::reply<get_extension_version<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_get_extension_version_reply), xcb_input_get_extension_version_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_extension_version<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_get_extension_version_reply), xcb_input_get_extension_version_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_extension_version(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_input_get_extension_version_cookie_t cookie(xcb_connection_t * const c, uint16_t name_len, const char * name) {
      return base::cookie(c, name_len, name);
    }

    static xcb_input_get_extension_version_cookie_t cookie(xcb_connection_t * const c, const std::string & name) {
      return base::cookie(c, static_cast<uint16_t>(name.length()), name.c_str());
    }
};

}

namespace checked {
template<typename Connection>
using get_extension_version = detail::get_extension_version<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_get_extension_version), xcb_input_get_extension_version>>;
}

namespace unchecked {
template<typename Connection>
using get_extension_version = detail::get_extension_version<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_get_extension_version_unchecked), xcb_input_get_extension_version_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_extension_version<Connection> get_extension_version(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_extension_version<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_extension_version<Connection> get_extension_version_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_extension_version<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class list_input_devices : public xpp::generic::reply<list_input_devices<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_list_input_devices_reply), xcb_input_list_input_devices_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<list_input_devices<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_list_input_devices_reply), xcb_input_list_input_devices_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    list_input_devices(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_input_list_input_devices_reply_t, xpp::generic::iterator<Connection,
                                              xcb_input_device_info_t,
                                              xpp::generic::signature<decltype(xcb_input_list_input_devices_devices), xcb_input_list_input_devices_devices>,
                                              xpp::generic::signature<decltype(xcb_input_list_input_devices_devices_length), xcb_input_list_input_devices_devices_length>>>
    devices() {
      return xpp::generic::list<Connection,
                                xcb_input_list_input_devices_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_input_device_info_t,
                                                       xpp::generic::signature<decltype(xcb_input_list_input_devices_devices), xcb_input_list_input_devices_devices>,
                                                       xpp::generic::signature<decltype(xcb_input_list_input_devices_devices_length), xcb_input_list_input_devices_devices_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_input_list_input_devices_reply_t, xpp::generic::iterator<Connection,
                                              xcb_input_input_info_t,
                                              xpp::generic::signature<decltype(xcb_input_input_info_next), xcb_input_input_info_next>,
                                              xpp::generic::signature<decltype(xcb_input_input_info_sizeof), xcb_input_input_info_sizeof>,
                                              xpp::generic::signature<decltype(xcb_input_list_input_devices_infos_iterator), xcb_input_list_input_devices_infos_iterator>>>
    infos() {
      return xpp::generic::list<Connection,
                                xcb_input_list_input_devices_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_input_input_info_t,
                                                       xpp::generic::signature<decltype(xcb_input_input_info_next), xcb_input_input_info_next>,
                                                       xpp::generic::signature<decltype(xcb_input_input_info_sizeof), xcb_input_input_info_sizeof>,
                                                       xpp::generic::signature<decltype(xcb_input_list_input_devices_infos_iterator), xcb_input_list_input_devices_infos_iterator>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_input_list_input_devices_reply_t, xpp::generic::iterator<Connection,
                                              xcb_str_t,
                                              xpp::generic::signature<decltype(xcb_str_next), xcb_str_next>,
                                              xpp::generic::signature<decltype(xcb_str_sizeof), xcb_str_sizeof>,
                                              xpp::generic::signature<decltype(xcb_input_list_input_devices_names_iterator), xcb_input_list_input_devices_names_iterator>>>
    names() {
      return xpp::generic::list<Connection,
                                xcb_input_list_input_devices_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_str_t,
                                                       xpp::generic::signature<decltype(xcb_str_next), xcb_str_next>,
                                                       xpp::generic::signature<decltype(xcb_str_sizeof), xcb_str_sizeof>,
                                                       xpp::generic::signature<decltype(xcb_input_list_input_devices_names_iterator), xcb_input_list_input_devices_names_iterator>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using list_input_devices = detail::list_input_devices<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_list_input_devices), xcb_input_list_input_devices>>;
}

namespace unchecked {
template<typename Connection>
using list_input_devices = detail::list_input_devices<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_list_input_devices_unchecked), xcb_input_list_input_devices_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::list_input_devices<Connection> list_input_devices(Connection && c, Parameter && ... parameter) {
  return reply::checked::list_input_devices<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::list_input_devices<Connection> list_input_devices_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::list_input_devices<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class open_device : public xpp::generic::reply<open_device<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_open_device_reply), xcb_input_open_device_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<open_device<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_open_device_reply), xcb_input_open_device_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    open_device(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_input_open_device_reply_t, xpp::generic::iterator<Connection,
                                              xcb_input_input_class_info_t,
                                              xpp::generic::signature<decltype(xcb_input_open_device_class_info), xcb_input_open_device_class_info>,
                                              xpp::generic::signature<decltype(xcb_input_open_device_class_info_length), xcb_input_open_device_class_info_length>>>
    class_info() {
      return xpp::generic::list<Connection,
                                xcb_input_open_device_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_input_input_class_info_t,
                                                       xpp::generic::signature<decltype(xcb_input_open_device_class_info), xcb_input_open_device_class_info>,
                                                       xpp::generic::signature<decltype(xcb_input_open_device_class_info_length), xcb_input_open_device_class_info_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using open_device = detail::open_device<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_open_device), xcb_input_open_device>>;
}

namespace unchecked {
template<typename Connection>
using open_device = detail::open_device<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_open_device_unchecked), xcb_input_open_device_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::open_device<Connection> open_device(Connection && c, Parameter && ... parameter) {
  return reply::checked::open_device<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::open_device<Connection> open_device_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::open_device<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void close_device_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_input_close_device_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void close_device(Parameter && ... parameter) {
  xcb_input_close_device(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class set_device_mode : public xpp::generic::reply<set_device_mode<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_set_device_mode_reply), xcb_input_set_device_mode_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<set_device_mode<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_set_device_mode_reply), xcb_input_set_device_mode_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    set_device_mode(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using set_device_mode = detail::set_device_mode<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_set_device_mode), xcb_input_set_device_mode>>;
}

namespace unchecked {
template<typename Connection>
using set_device_mode = detail::set_device_mode<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_set_device_mode_unchecked), xcb_input_set_device_mode_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::set_device_mode<Connection> set_device_mode(Connection && c, Parameter && ... parameter) {
  return reply::checked::set_device_mode<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::set_device_mode<Connection> set_device_mode_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::set_device_mode<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void select_extension_event_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_input_select_extension_event_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void select_extension_event(Parameter && ... parameter) {
  xcb_input_select_extension_event(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_selected_extension_events : public xpp::generic::reply<get_selected_extension_events<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_get_selected_extension_events_reply), xcb_input_get_selected_extension_events_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_selected_extension_events<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_get_selected_extension_events_reply), xcb_input_get_selected_extension_events_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_selected_extension_events(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_input_get_selected_extension_events_reply_t, xpp::generic::iterator<Connection,
                                              xcb_input_event_class_t,
                                              xpp::generic::signature<decltype(xcb_input_get_selected_extension_events_this_classes), xcb_input_get_selected_extension_events_this_classes>,
                                              xpp::generic::signature<decltype(xcb_input_get_selected_extension_events_this_classes_length), xcb_input_get_selected_extension_events_this_classes_length>>>
    this_classes() {
      return xpp::generic::list<Connection,
                                xcb_input_get_selected_extension_events_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_input_event_class_t,
                                                       xpp::generic::signature<decltype(xcb_input_get_selected_extension_events_this_classes), xcb_input_get_selected_extension_events_this_classes>,
                                                       xpp::generic::signature<decltype(xcb_input_get_selected_extension_events_this_classes_length), xcb_input_get_selected_extension_events_this_classes_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_input_get_selected_extension_events_reply_t, xpp::generic::iterator<Connection,
                                              xcb_input_event_class_t,
                                              xpp::generic::signature<decltype(xcb_input_get_selected_extension_events_all_classes), xcb_input_get_selected_extension_events_all_classes>,
                                              xpp::generic::signature<decltype(xcb_input_get_selected_extension_events_all_classes_length), xcb_input_get_selected_extension_events_all_classes_length>>>
    all_classes() {
      return xpp::generic::list<Connection,
                                xcb_input_get_selected_extension_events_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_input_event_class_t,
                                                       xpp::generic::signature<decltype(xcb_input_get_selected_extension_events_all_classes), xcb_input_get_selected_extension_events_all_classes>,
                                                       xpp::generic::signature<decltype(xcb_input_get_selected_extension_events_all_classes_length), xcb_input_get_selected_extension_events_all_classes_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_selected_extension_events = detail::get_selected_extension_events<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_get_selected_extension_events), xcb_input_get_selected_extension_events>>;
}

namespace unchecked {
template<typename Connection>
using get_selected_extension_events = detail::get_selected_extension_events<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_get_selected_extension_events_unchecked), xcb_input_get_selected_extension_events_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_selected_extension_events<Connection> get_selected_extension_events(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_selected_extension_events<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_selected_extension_events<Connection> get_selected_extension_events_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_selected_extension_events<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void change_device_dont_propagate_list_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_input_change_device_dont_propagate_list_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void change_device_dont_propagate_list(Parameter && ... parameter) {
  xcb_input_change_device_dont_propagate_list(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_device_dont_propagate_list : public xpp::generic::reply<get_device_dont_propagate_list<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_get_device_dont_propagate_list_reply), xcb_input_get_device_dont_propagate_list_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_device_dont_propagate_list<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_get_device_dont_propagate_list_reply), xcb_input_get_device_dont_propagate_list_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_device_dont_propagate_list(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_input_get_device_dont_propagate_list_reply_t, xpp::generic::iterator<Connection,
                                              xcb_input_event_class_t,
                                              xpp::generic::signature<decltype(xcb_input_get_device_dont_propagate_list_classes), xcb_input_get_device_dont_propagate_list_classes>,
                                              xpp::generic::signature<decltype(xcb_input_get_device_dont_propagate_list_classes_length), xcb_input_get_device_dont_propagate_list_classes_length>>>
    classes() {
      return xpp::generic::list<Connection,
                                xcb_input_get_device_dont_propagate_list_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_input_event_class_t,
                                                       xpp::generic::signature<decltype(xcb_input_get_device_dont_propagate_list_classes), xcb_input_get_device_dont_propagate_list_classes>,
                                                       xpp::generic::signature<decltype(xcb_input_get_device_dont_propagate_list_classes_length), xcb_input_get_device_dont_propagate_list_classes_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_device_dont_propagate_list = detail::get_device_dont_propagate_list<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_get_device_dont_propagate_list), xcb_input_get_device_dont_propagate_list>>;
}

namespace unchecked {
template<typename Connection>
using get_device_dont_propagate_list = detail::get_device_dont_propagate_list<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_get_device_dont_propagate_list_unchecked), xcb_input_get_device_dont_propagate_list_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_device_dont_propagate_list<Connection> get_device_dont_propagate_list(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_device_dont_propagate_list<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_device_dont_propagate_list<Connection> get_device_dont_propagate_list_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_device_dont_propagate_list<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_device_motion_events : public xpp::generic::reply<get_device_motion_events<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_get_device_motion_events_reply), xcb_input_get_device_motion_events_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_device_motion_events<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_get_device_motion_events_reply), xcb_input_get_device_motion_events_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_device_motion_events(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_input_get_device_motion_events_cookie_t cookie(xcb_connection_t * const c, xcb_timestamp_t start, xcb_timestamp_t stop, uint8_t device_id) {
      return base::cookie(c, start, stop, device_id);
    }

    static xcb_input_get_device_motion_events_cookie_t cookie(xcb_connection_t * const c, uint8_t device_id, xcb_timestamp_t start = XCB_TIME_CURRENT_TIME, xcb_timestamp_t stop = XCB_TIME_CURRENT_TIME) {
      return base::cookie(c, start, stop, device_id);
    }

    xpp::generic::list<Connection, xcb_input_get_device_motion_events_reply_t, xpp::generic::iterator<Connection,
                                              xcb_input_device_time_coord_t,
                                              xpp::generic::signature<decltype(xcb_input_device_time_coord_next), xcb_input_device_time_coord_next>,
                                              xpp::generic::signature<decltype(xcb_input_device_time_coord_sizeof), xcb_input_device_time_coord_sizeof>,
                                              xpp::generic::signature<decltype(xcb_input_get_device_motion_events_events_iterator), xcb_input_get_device_motion_events_events_iterator>>>
    events() {
      return xpp::generic::list<Connection,
                                xcb_input_get_device_motion_events_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_input_device_time_coord_t,
                                                       xpp::generic::signature<decltype(xcb_input_device_time_coord_next), xcb_input_device_time_coord_next>,
                                                       xpp::generic::signature<decltype(xcb_input_device_time_coord_sizeof), xcb_input_device_time_coord_sizeof>,
                                                       xpp::generic::signature<decltype(xcb_input_get_device_motion_events_events_iterator), xcb_input_get_device_motion_events_events_iterator>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_device_motion_events = detail::get_device_motion_events<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_get_device_motion_events), xcb_input_get_device_motion_events>>;
}

namespace unchecked {
template<typename Connection>
using get_device_motion_events = detail::get_device_motion_events<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_get_device_motion_events_unchecked), xcb_input_get_device_motion_events_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_device_motion_events<Connection> get_device_motion_events(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_device_motion_events<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_device_motion_events<Connection> get_device_motion_events_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_device_motion_events<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class change_keyboard_device : public xpp::generic::reply<change_keyboard_device<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_change_keyboard_device_reply), xcb_input_change_keyboard_device_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<change_keyboard_device<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_change_keyboard_device_reply), xcb_input_change_keyboard_device_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    change_keyboard_device(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using change_keyboard_device = detail::change_keyboard_device<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_change_keyboard_device), xcb_input_change_keyboard_device>>;
}

namespace unchecked {
template<typename Connection>
using change_keyboard_device = detail::change_keyboard_device<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_change_keyboard_device_unchecked), xcb_input_change_keyboard_device_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::change_keyboard_device<Connection> change_keyboard_device(Connection && c, Parameter && ... parameter) {
  return reply::checked::change_keyboard_device<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::change_keyboard_device<Connection> change_keyboard_device_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::change_keyboard_device<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class change_pointer_device : public xpp::generic::reply<change_pointer_device<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_change_pointer_device_reply), xcb_input_change_pointer_device_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<change_pointer_device<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_change_pointer_device_reply), xcb_input_change_pointer_device_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    change_pointer_device(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using change_pointer_device = detail::change_pointer_device<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_change_pointer_device), xcb_input_change_pointer_device>>;
}

namespace unchecked {
template<typename Connection>
using change_pointer_device = detail::change_pointer_device<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_change_pointer_device_unchecked), xcb_input_change_pointer_device_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::change_pointer_device<Connection> change_pointer_device(Connection && c, Parameter && ... parameter) {
  return reply::checked::change_pointer_device<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::change_pointer_device<Connection> change_pointer_device_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::change_pointer_device<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class grab_device : public xpp::generic::reply<grab_device<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_grab_device_reply), xcb_input_grab_device_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<grab_device<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_grab_device_reply), xcb_input_grab_device_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    grab_device(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_input_grab_device_cookie_t cookie(xcb_connection_t * const c, xcb_window_t grab_window, xcb_timestamp_t time, uint16_t num_classes, uint8_t this_device_mode, uint8_t other_device_mode, uint8_t owner_events, uint8_t device_id, const xcb_input_event_class_t * classes) {
      return base::cookie(c, grab_window, time, num_classes, this_device_mode, other_device_mode, owner_events, device_id, classes);
    }

    static xcb_input_grab_device_cookie_t cookie(xcb_connection_t * const c, xcb_window_t grab_window, uint16_t num_classes, uint8_t this_device_mode, uint8_t other_device_mode, uint8_t owner_events, uint8_t device_id, const xcb_input_event_class_t * classes, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
      return base::cookie(c, grab_window, time, num_classes, this_device_mode, other_device_mode, owner_events, device_id, classes);
    }
};

}

namespace checked {
template<typename Connection>
using grab_device = detail::grab_device<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_grab_device), xcb_input_grab_device>>;
}

namespace unchecked {
template<typename Connection>
using grab_device = detail::grab_device<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_grab_device_unchecked), xcb_input_grab_device_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::grab_device<Connection> grab_device(Connection && c, Parameter && ... parameter) {
  return reply::checked::grab_device<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::grab_device<Connection> grab_device_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::grab_device<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void ungrab_device_checked(Connection && c, xcb_timestamp_t time, uint8_t device_id) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(std::forward<Connection>(c), xcb_input_ungrab_device_checked(std::forward<Connection>(c), time, device_id));
}

template<typename Connection>
void ungrab_device(Connection && c, xcb_timestamp_t time, uint8_t device_id) {
  xcb_input_ungrab_device(std::forward<Connection>(c), time, device_id);
}

template<typename Connection>
void ungrab_device_checked(Connection && c, uint8_t device_id, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(std::forward<Connection>(c), xcb_input_ungrab_device_checked(std::forward<Connection>(c), time, device_id));
}

template<typename Connection>
void ungrab_device(Connection && c, uint8_t device_id, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xcb_input_ungrab_device(std::forward<Connection>(c), time, device_id);
}

template<typename Connection, typename ... Parameter>
void grab_device_key_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_input_grab_device_key_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void grab_device_key(Parameter && ... parameter) {
  xcb_input_grab_device_key(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void ungrab_device_key_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_input_ungrab_device_key_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void ungrab_device_key(Parameter && ... parameter) {
  xcb_input_ungrab_device_key(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void grab_device_button_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_input_grab_device_button_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void grab_device_button(Parameter && ... parameter) {
  xcb_input_grab_device_button(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void ungrab_device_button_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_input_ungrab_device_button_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void ungrab_device_button(Parameter && ... parameter) {
  xcb_input_ungrab_device_button(std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void allow_device_events_checked(Connection && c, xcb_timestamp_t time, uint8_t mode, uint8_t device_id) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(std::forward<Connection>(c), xcb_input_allow_device_events_checked(std::forward<Connection>(c), time, mode, device_id));
}

template<typename Connection>
void allow_device_events(Connection && c, xcb_timestamp_t time, uint8_t mode, uint8_t device_id) {
  xcb_input_allow_device_events(std::forward<Connection>(c), time, mode, device_id);
}

template<typename Connection>
void allow_device_events_checked(Connection && c, uint8_t mode, uint8_t device_id, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(std::forward<Connection>(c), xcb_input_allow_device_events_checked(std::forward<Connection>(c), time, mode, device_id));
}

template<typename Connection>
void allow_device_events(Connection && c, uint8_t mode, uint8_t device_id, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xcb_input_allow_device_events(std::forward<Connection>(c), time, mode, device_id);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_device_focus : public xpp::generic::reply<get_device_focus<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_get_device_focus_reply), xcb_input_get_device_focus_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_device_focus<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_get_device_focus_reply), xcb_input_get_device_focus_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_device_focus(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType focus(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->focus),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->focus,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using get_device_focus = detail::get_device_focus<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_get_device_focus), xcb_input_get_device_focus>>;
}

namespace unchecked {
template<typename Connection>
using get_device_focus = detail::get_device_focus<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_get_device_focus_unchecked), xcb_input_get_device_focus_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_device_focus<Connection> get_device_focus(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_device_focus<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_device_focus<Connection> get_device_focus_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_device_focus<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void set_device_focus_checked(Connection && c, xcb_window_t focus, xcb_timestamp_t time, uint8_t revert_to, uint8_t device_id) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(std::forward<Connection>(c), xcb_input_set_device_focus_checked(std::forward<Connection>(c), focus, time, revert_to, device_id));
}

template<typename Connection>
void set_device_focus(Connection && c, xcb_window_t focus, xcb_timestamp_t time, uint8_t revert_to, uint8_t device_id) {
  xcb_input_set_device_focus(std::forward<Connection>(c), focus, time, revert_to, device_id);
}

template<typename Connection>
void set_device_focus_checked(Connection && c, xcb_window_t focus, uint8_t revert_to, uint8_t device_id, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(std::forward<Connection>(c), xcb_input_set_device_focus_checked(std::forward<Connection>(c), focus, time, revert_to, device_id));
}

template<typename Connection>
void set_device_focus(Connection && c, xcb_window_t focus, uint8_t revert_to, uint8_t device_id, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xcb_input_set_device_focus(std::forward<Connection>(c), focus, time, revert_to, device_id);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_feedback_control : public xpp::generic::reply<get_feedback_control<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_get_feedback_control_reply), xcb_input_get_feedback_control_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_feedback_control<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_get_feedback_control_reply), xcb_input_get_feedback_control_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_feedback_control(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_input_get_feedback_control_reply_t, xpp::generic::iterator<Connection,
                                              xcb_input_feedback_state_t,
                                              xpp::generic::signature<decltype(xcb_input_feedback_state_next), xcb_input_feedback_state_next>,
                                              xpp::generic::signature<decltype(xcb_input_feedback_state_sizeof), xcb_input_feedback_state_sizeof>,
                                              xpp::generic::signature<decltype(xcb_input_get_feedback_control_feedbacks_iterator), xcb_input_get_feedback_control_feedbacks_iterator>>>
    feedbacks() {
      return xpp::generic::list<Connection,
                                xcb_input_get_feedback_control_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_input_feedback_state_t,
                                                       xpp::generic::signature<decltype(xcb_input_feedback_state_next), xcb_input_feedback_state_next>,
                                                       xpp::generic::signature<decltype(xcb_input_feedback_state_sizeof), xcb_input_feedback_state_sizeof>,
                                                       xpp::generic::signature<decltype(xcb_input_get_feedback_control_feedbacks_iterator), xcb_input_get_feedback_control_feedbacks_iterator>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_feedback_control = detail::get_feedback_control<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_get_feedback_control), xcb_input_get_feedback_control>>;
}

namespace unchecked {
template<typename Connection>
using get_feedback_control = detail::get_feedback_control<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_get_feedback_control_unchecked), xcb_input_get_feedback_control_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_feedback_control<Connection> get_feedback_control(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_feedback_control<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_feedback_control<Connection> get_feedback_control_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_feedback_control<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void change_feedback_control_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_input_change_feedback_control_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void change_feedback_control(Parameter && ... parameter) {
  xcb_input_change_feedback_control(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_device_key_mapping : public xpp::generic::reply<get_device_key_mapping<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_get_device_key_mapping_reply), xcb_input_get_device_key_mapping_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_device_key_mapping<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_get_device_key_mapping_reply), xcb_input_get_device_key_mapping_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_device_key_mapping(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_input_get_device_key_mapping_reply_t, xpp::generic::iterator<Connection,
                                              xcb_keysym_t,
                                              xpp::generic::signature<decltype(xcb_input_get_device_key_mapping_keysyms), xcb_input_get_device_key_mapping_keysyms>,
                                              xpp::generic::signature<decltype(xcb_input_get_device_key_mapping_keysyms_length), xcb_input_get_device_key_mapping_keysyms_length>>>
    keysyms() {
      return xpp::generic::list<Connection,
                                xcb_input_get_device_key_mapping_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_keysym_t,
                                                       xpp::generic::signature<decltype(xcb_input_get_device_key_mapping_keysyms), xcb_input_get_device_key_mapping_keysyms>,
                                                       xpp::generic::signature<decltype(xcb_input_get_device_key_mapping_keysyms_length), xcb_input_get_device_key_mapping_keysyms_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_device_key_mapping = detail::get_device_key_mapping<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_get_device_key_mapping), xcb_input_get_device_key_mapping>>;
}

namespace unchecked {
template<typename Connection>
using get_device_key_mapping = detail::get_device_key_mapping<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_get_device_key_mapping_unchecked), xcb_input_get_device_key_mapping_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_device_key_mapping<Connection> get_device_key_mapping(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_device_key_mapping<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_device_key_mapping<Connection> get_device_key_mapping_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_device_key_mapping<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void change_device_key_mapping_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_input_change_device_key_mapping_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void change_device_key_mapping(Parameter && ... parameter) {
  xcb_input_change_device_key_mapping(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_device_modifier_mapping : public xpp::generic::reply<get_device_modifier_mapping<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_get_device_modifier_mapping_reply), xcb_input_get_device_modifier_mapping_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_device_modifier_mapping<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_get_device_modifier_mapping_reply), xcb_input_get_device_modifier_mapping_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_device_modifier_mapping(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_input_get_device_modifier_mapping_reply_t, xpp::generic::iterator<Connection,
                                              uint8_t,
                                              xpp::generic::signature<decltype(xcb_input_get_device_modifier_mapping_keymaps), xcb_input_get_device_modifier_mapping_keymaps>,
                                              xpp::generic::signature<decltype(xcb_input_get_device_modifier_mapping_keymaps_length), xcb_input_get_device_modifier_mapping_keymaps_length>>>
    keymaps() {
      return xpp::generic::list<Connection,
                                xcb_input_get_device_modifier_mapping_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint8_t,
                                                       xpp::generic::signature<decltype(xcb_input_get_device_modifier_mapping_keymaps), xcb_input_get_device_modifier_mapping_keymaps>,
                                                       xpp::generic::signature<decltype(xcb_input_get_device_modifier_mapping_keymaps_length), xcb_input_get_device_modifier_mapping_keymaps_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_device_modifier_mapping = detail::get_device_modifier_mapping<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_get_device_modifier_mapping), xcb_input_get_device_modifier_mapping>>;
}

namespace unchecked {
template<typename Connection>
using get_device_modifier_mapping = detail::get_device_modifier_mapping<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_get_device_modifier_mapping_unchecked), xcb_input_get_device_modifier_mapping_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_device_modifier_mapping<Connection> get_device_modifier_mapping(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_device_modifier_mapping<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_device_modifier_mapping<Connection> get_device_modifier_mapping_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_device_modifier_mapping<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class set_device_modifier_mapping : public xpp::generic::reply<set_device_modifier_mapping<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_set_device_modifier_mapping_reply), xcb_input_set_device_modifier_mapping_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<set_device_modifier_mapping<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_set_device_modifier_mapping_reply), xcb_input_set_device_modifier_mapping_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    set_device_modifier_mapping(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using set_device_modifier_mapping = detail::set_device_modifier_mapping<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_set_device_modifier_mapping), xcb_input_set_device_modifier_mapping>>;
}

namespace unchecked {
template<typename Connection>
using set_device_modifier_mapping = detail::set_device_modifier_mapping<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_set_device_modifier_mapping_unchecked), xcb_input_set_device_modifier_mapping_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::set_device_modifier_mapping<Connection> set_device_modifier_mapping(Connection && c, Parameter && ... parameter) {
  return reply::checked::set_device_modifier_mapping<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::set_device_modifier_mapping<Connection> set_device_modifier_mapping_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::set_device_modifier_mapping<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_device_button_mapping : public xpp::generic::reply<get_device_button_mapping<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_get_device_button_mapping_reply), xcb_input_get_device_button_mapping_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_device_button_mapping<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_get_device_button_mapping_reply), xcb_input_get_device_button_mapping_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_device_button_mapping(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_input_get_device_button_mapping_reply_t, xpp::generic::iterator<Connection,
                                              uint8_t,
                                              xpp::generic::signature<decltype(xcb_input_get_device_button_mapping_map), xcb_input_get_device_button_mapping_map>,
                                              xpp::generic::signature<decltype(xcb_input_get_device_button_mapping_map_length), xcb_input_get_device_button_mapping_map_length>>>
    map() {
      return xpp::generic::list<Connection,
                                xcb_input_get_device_button_mapping_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint8_t,
                                                       xpp::generic::signature<decltype(xcb_input_get_device_button_mapping_map), xcb_input_get_device_button_mapping_map>,
                                                       xpp::generic::signature<decltype(xcb_input_get_device_button_mapping_map_length), xcb_input_get_device_button_mapping_map_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_device_button_mapping = detail::get_device_button_mapping<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_get_device_button_mapping), xcb_input_get_device_button_mapping>>;
}

namespace unchecked {
template<typename Connection>
using get_device_button_mapping = detail::get_device_button_mapping<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_get_device_button_mapping_unchecked), xcb_input_get_device_button_mapping_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_device_button_mapping<Connection> get_device_button_mapping(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_device_button_mapping<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_device_button_mapping<Connection> get_device_button_mapping_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_device_button_mapping<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class set_device_button_mapping : public xpp::generic::reply<set_device_button_mapping<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_set_device_button_mapping_reply), xcb_input_set_device_button_mapping_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<set_device_button_mapping<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_set_device_button_mapping_reply), xcb_input_set_device_button_mapping_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    set_device_button_mapping(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using set_device_button_mapping = detail::set_device_button_mapping<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_set_device_button_mapping), xcb_input_set_device_button_mapping>>;
}

namespace unchecked {
template<typename Connection>
using set_device_button_mapping = detail::set_device_button_mapping<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_set_device_button_mapping_unchecked), xcb_input_set_device_button_mapping_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::set_device_button_mapping<Connection> set_device_button_mapping(Connection && c, Parameter && ... parameter) {
  return reply::checked::set_device_button_mapping<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::set_device_button_mapping<Connection> set_device_button_mapping_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::set_device_button_mapping<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_device_state : public xpp::generic::reply<query_device_state<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_query_device_state_reply), xcb_input_query_device_state_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_device_state<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_query_device_state_reply), xcb_input_query_device_state_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_device_state(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_input_query_device_state_reply_t, xpp::generic::iterator<Connection,
                                              xcb_input_input_state_t,
                                              xpp::generic::signature<decltype(xcb_input_input_state_next), xcb_input_input_state_next>,
                                              xpp::generic::signature<decltype(xcb_input_input_state_sizeof), xcb_input_input_state_sizeof>,
                                              xpp::generic::signature<decltype(xcb_input_query_device_state_classes_iterator), xcb_input_query_device_state_classes_iterator>>>
    classes() {
      return xpp::generic::list<Connection,
                                xcb_input_query_device_state_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_input_input_state_t,
                                                       xpp::generic::signature<decltype(xcb_input_input_state_next), xcb_input_input_state_next>,
                                                       xpp::generic::signature<decltype(xcb_input_input_state_sizeof), xcb_input_input_state_sizeof>,
                                                       xpp::generic::signature<decltype(xcb_input_query_device_state_classes_iterator), xcb_input_query_device_state_classes_iterator>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using query_device_state = detail::query_device_state<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_query_device_state), xcb_input_query_device_state>>;
}

namespace unchecked {
template<typename Connection>
using query_device_state = detail::query_device_state<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_query_device_state_unchecked), xcb_input_query_device_state_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_device_state<Connection> query_device_state(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_device_state<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_device_state<Connection> query_device_state_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_device_state<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void device_bell_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_input_device_bell_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void device_bell(Parameter && ... parameter) {
  xcb_input_device_bell(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class set_device_valuators : public xpp::generic::reply<set_device_valuators<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_set_device_valuators_reply), xcb_input_set_device_valuators_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<set_device_valuators<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_set_device_valuators_reply), xcb_input_set_device_valuators_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    set_device_valuators(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using set_device_valuators = detail::set_device_valuators<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_set_device_valuators), xcb_input_set_device_valuators>>;
}

namespace unchecked {
template<typename Connection>
using set_device_valuators = detail::set_device_valuators<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_set_device_valuators_unchecked), xcb_input_set_device_valuators_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::set_device_valuators<Connection> set_device_valuators(Connection && c, Parameter && ... parameter) {
  return reply::checked::set_device_valuators<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::set_device_valuators<Connection> set_device_valuators_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::set_device_valuators<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_device_control : public xpp::generic::reply<get_device_control<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_get_device_control_reply), xcb_input_get_device_control_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_device_control<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_get_device_control_reply), xcb_input_get_device_control_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_device_control(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using get_device_control = detail::get_device_control<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_get_device_control), xcb_input_get_device_control>>;
}

namespace unchecked {
template<typename Connection>
using get_device_control = detail::get_device_control<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_get_device_control_unchecked), xcb_input_get_device_control_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_device_control<Connection> get_device_control(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_device_control<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_device_control<Connection> get_device_control_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_device_control<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class change_device_control : public xpp::generic::reply<change_device_control<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_change_device_control_reply), xcb_input_change_device_control_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<change_device_control<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_change_device_control_reply), xcb_input_change_device_control_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    change_device_control(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using change_device_control = detail::change_device_control<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_change_device_control), xcb_input_change_device_control>>;
}

namespace unchecked {
template<typename Connection>
using change_device_control = detail::change_device_control<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_change_device_control_unchecked), xcb_input_change_device_control_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::change_device_control<Connection> change_device_control(Connection && c, Parameter && ... parameter) {
  return reply::checked::change_device_control<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::change_device_control<Connection> change_device_control_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::change_device_control<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class list_device_properties : public xpp::generic::reply<list_device_properties<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_list_device_properties_reply), xcb_input_list_device_properties_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<list_device_properties<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_list_device_properties_reply), xcb_input_list_device_properties_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    list_device_properties(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename Atoms = xcb_atom_t>
    xpp::generic::list<Connection, xcb_input_list_device_properties_reply_t, xpp::generic::iterator<Connection,
                                              Atoms,
                                              xpp::generic::signature<decltype(xcb_input_list_device_properties_atoms), xcb_input_list_device_properties_atoms>,
                                              xpp::generic::signature<decltype(xcb_input_list_device_properties_atoms_length), xcb_input_list_device_properties_atoms_length>>>
    atoms() {
      return xpp::generic::list<Connection,
                                xcb_input_list_device_properties_reply_t,
                                xpp::generic::iterator<Connection,
                                                       Atoms,
                                                       xpp::generic::signature<decltype(xcb_input_list_device_properties_atoms), xcb_input_list_device_properties_atoms>,
                                                       xpp::generic::signature<decltype(xcb_input_list_device_properties_atoms_length), xcb_input_list_device_properties_atoms_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using list_device_properties = detail::list_device_properties<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_list_device_properties), xcb_input_list_device_properties>>;
}

namespace unchecked {
template<typename Connection>
using list_device_properties = detail::list_device_properties<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_list_device_properties_unchecked), xcb_input_list_device_properties_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::list_device_properties<Connection> list_device_properties(Connection && c, Parameter && ... parameter) {
  return reply::checked::list_device_properties<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::list_device_properties<Connection> list_device_properties_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::list_device_properties<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void change_device_property_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_input_change_device_property_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void change_device_property(Parameter && ... parameter) {
  xcb_input_change_device_property(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void delete_device_property_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_input_delete_device_property_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void delete_device_property(Parameter && ... parameter) {
  xcb_input_delete_device_property(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_device_property : public xpp::generic::reply<get_device_property<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_get_device_property_reply), xcb_input_get_device_property_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_device_property<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_get_device_property_reply), xcb_input_get_device_property_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_device_property(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename ReturnType = xcb_atom_t, typename ... Parameter>
    ReturnType type(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->type),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->type,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using get_device_property = detail::get_device_property<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_get_device_property), xcb_input_get_device_property>>;
}

namespace unchecked {
template<typename Connection>
using get_device_property = detail::get_device_property<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_get_device_property_unchecked), xcb_input_get_device_property_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_device_property<Connection> get_device_property(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_device_property<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_device_property<Connection> get_device_property_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_device_property<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class xi_query_pointer : public xpp::generic::reply<xi_query_pointer<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_xi_query_pointer_reply), xcb_input_xi_query_pointer_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<xi_query_pointer<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_xi_query_pointer_reply), xcb_input_xi_query_pointer_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    xi_query_pointer(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_input_xi_query_pointer_reply_t, xpp::generic::iterator<Connection,
                                              uint32_t,
                                              xpp::generic::signature<decltype(xcb_input_xi_query_pointer_buttons), xcb_input_xi_query_pointer_buttons>,
                                              xpp::generic::signature<decltype(xcb_input_xi_query_pointer_buttons_length), xcb_input_xi_query_pointer_buttons_length>>>
    buttons() {
      return xpp::generic::list<Connection,
                                xcb_input_xi_query_pointer_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint32_t,
                                                       xpp::generic::signature<decltype(xcb_input_xi_query_pointer_buttons), xcb_input_xi_query_pointer_buttons>,
                                                       xpp::generic::signature<decltype(xcb_input_xi_query_pointer_buttons_length), xcb_input_xi_query_pointer_buttons_length>>
                               >(this->m_c, this->get());
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->child,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using xi_query_pointer = detail::xi_query_pointer<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_xi_query_pointer), xcb_input_xi_query_pointer>>;
}

namespace unchecked {
template<typename Connection>
using xi_query_pointer = detail::xi_query_pointer<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_xi_query_pointer_unchecked), xcb_input_xi_query_pointer_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::xi_query_pointer<Connection> xi_query_pointer(Connection && c, Parameter && ... parameter) {
  return reply::checked::xi_query_pointer<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::xi_query_pointer<Connection> xi_query_pointer_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::xi_query_pointer<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void xi_warp_pointer_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_input_xi_warp_pointer_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void xi_warp_pointer(Parameter && ... parameter) {
  xcb_input_xi_warp_pointer(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void xi_change_cursor_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_input_xi_change_cursor_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void xi_change_cursor(Parameter && ... parameter) {
  xcb_input_xi_change_cursor(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void xi_change_hierarchy_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_input_xi_change_hierarchy_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void xi_change_hierarchy(Parameter && ... parameter) {
  xcb_input_xi_change_hierarchy(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void xi_set_client_pointer_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_input_xi_set_client_pointer_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void xi_set_client_pointer(Parameter && ... parameter) {
  xcb_input_xi_set_client_pointer(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class xi_get_client_pointer : public xpp::generic::reply<xi_get_client_pointer<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_xi_get_client_pointer_reply), xcb_input_xi_get_client_pointer_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<xi_get_client_pointer<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_xi_get_client_pointer_reply), xcb_input_xi_get_client_pointer_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    xi_get_client_pointer(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using xi_get_client_pointer = detail::xi_get_client_pointer<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_xi_get_client_pointer), xcb_input_xi_get_client_pointer>>;
}

namespace unchecked {
template<typename Connection>
using xi_get_client_pointer = detail::xi_get_client_pointer<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_xi_get_client_pointer_unchecked), xcb_input_xi_get_client_pointer_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::xi_get_client_pointer<Connection> xi_get_client_pointer(Connection && c, Parameter && ... parameter) {
  return reply::checked::xi_get_client_pointer<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::xi_get_client_pointer<Connection> xi_get_client_pointer_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::xi_get_client_pointer<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void xi_select_events_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_input_xi_select_events_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void xi_select_events(Parameter && ... parameter) {
  xcb_input_xi_select_events(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class xi_query_version : public xpp::generic::reply<xi_query_version<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_xi_query_version_reply), xcb_input_xi_query_version_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<xi_query_version<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_xi_query_version_reply), xcb_input_xi_query_version_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    xi_query_version(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using xi_query_version = detail::xi_query_version<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_xi_query_version), xcb_input_xi_query_version>>;
}

namespace unchecked {
template<typename Connection>
using xi_query_version = detail::xi_query_version<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_xi_query_version_unchecked), xcb_input_xi_query_version_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::xi_query_version<Connection> xi_query_version(Connection && c, Parameter && ... parameter) {
  return reply::checked::xi_query_version<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::xi_query_version<Connection> xi_query_version_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::xi_query_version<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class xi_query_device : public xpp::generic::reply<xi_query_device<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_xi_query_device_reply), xcb_input_xi_query_device_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<xi_query_device<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_xi_query_device_reply), xcb_input_xi_query_device_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    xi_query_device(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_input_xi_query_device_reply_t, xpp::generic::iterator<Connection,
                                              xcb_input_xi_device_info_t,
                                              xpp::generic::signature<decltype(xcb_input_xi_device_info_next), xcb_input_xi_device_info_next>,
                                              xpp::generic::signature<decltype(xcb_input_xi_device_info_sizeof), xcb_input_xi_device_info_sizeof>,
                                              xpp::generic::signature<decltype(xcb_input_xi_query_device_infos_iterator), xcb_input_xi_query_device_infos_iterator>>>
    infos() {
      return xpp::generic::list<Connection,
                                xcb_input_xi_query_device_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_input_xi_device_info_t,
                                                       xpp::generic::signature<decltype(xcb_input_xi_device_info_next), xcb_input_xi_device_info_next>,
                                                       xpp::generic::signature<decltype(xcb_input_xi_device_info_sizeof), xcb_input_xi_device_info_sizeof>,
                                                       xpp::generic::signature<decltype(xcb_input_xi_query_device_infos_iterator), xcb_input_xi_query_device_infos_iterator>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using xi_query_device = detail::xi_query_device<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_xi_query_device), xcb_input_xi_query_device>>;
}

namespace unchecked {
template<typename Connection>
using xi_query_device = detail::xi_query_device<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_xi_query_device_unchecked), xcb_input_xi_query_device_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::xi_query_device<Connection> xi_query_device(Connection && c, Parameter && ... parameter) {
  return reply::checked::xi_query_device<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::xi_query_device<Connection> xi_query_device_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::xi_query_device<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void xi_set_focus_checked(Connection && c, xcb_window_t window, xcb_timestamp_t time, xcb_input_device_id_t deviceid) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(std::forward<Connection>(c), xcb_input_xi_set_focus_checked(std::forward<Connection>(c), window, time, deviceid));
}

template<typename Connection>
void xi_set_focus(Connection && c, xcb_window_t window, xcb_timestamp_t time, xcb_input_device_id_t deviceid) {
  xcb_input_xi_set_focus(std::forward<Connection>(c), window, time, deviceid);
}

template<typename Connection>
void xi_set_focus_checked(Connection && c, xcb_window_t window, xcb_input_device_id_t deviceid, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(std::forward<Connection>(c), xcb_input_xi_set_focus_checked(std::forward<Connection>(c), window, time, deviceid));
}

template<typename Connection>
void xi_set_focus(Connection && c, xcb_window_t window, xcb_input_device_id_t deviceid, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xcb_input_xi_set_focus(std::forward<Connection>(c), window, time, deviceid);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class xi_get_focus : public xpp::generic::reply<xi_get_focus<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_xi_get_focus_reply), xcb_input_xi_get_focus_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<xi_get_focus<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_xi_get_focus_reply), xcb_input_xi_get_focus_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    xi_get_focus(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType focus(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->focus),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->focus,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using xi_get_focus = detail::xi_get_focus<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_xi_get_focus), xcb_input_xi_get_focus>>;
}

namespace unchecked {
template<typename Connection>
using xi_get_focus = detail::xi_get_focus<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_xi_get_focus_unchecked), xcb_input_xi_get_focus_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::xi_get_focus<Connection> xi_get_focus(Connection && c, Parameter && ... parameter) {
  return reply::checked::xi_get_focus<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::xi_get_focus<Connection> xi_get_focus_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::xi_get_focus<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class xi_grab_device : public xpp::generic::reply<xi_grab_device<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_xi_grab_device_reply), xcb_input_xi_grab_device_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<xi_grab_device<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_xi_grab_device_reply), xcb_input_xi_grab_device_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    xi_grab_device(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_input_xi_grab_device_cookie_t cookie(xcb_connection_t * const c, xcb_window_t window, xcb_timestamp_t time, xcb_cursor_t cursor, xcb_input_device_id_t deviceid, uint8_t mode, uint8_t paired_device_mode, uint8_t owner_events, uint16_t mask_len, const uint32_t * mask) {
      return base::cookie(c, window, time, cursor, deviceid, mode, paired_device_mode, owner_events, mask_len, mask);
    }

    static xcb_input_xi_grab_device_cookie_t cookie(xcb_connection_t * const c, xcb_window_t window, xcb_cursor_t cursor, xcb_input_device_id_t deviceid, uint8_t mode, uint8_t paired_device_mode, uint8_t owner_events, uint16_t mask_len, const uint32_t * mask, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
      return base::cookie(c, window, time, cursor, deviceid, mode, paired_device_mode, owner_events, mask_len, mask);
    }

    template<typename Mask_Iterator>
    static xcb_input_xi_grab_device_cookie_t cookie(xcb_connection_t * const c, xcb_window_t window, xcb_cursor_t cursor, xcb_input_device_id_t deviceid, uint8_t mode, uint8_t paired_device_mode, uint8_t owner_events, Mask_Iterator mask_begin, Mask_Iterator mask_end, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
      typedef typename value_type<Mask_Iterator, ! std::is_pointer<Mask_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> mask =
        { value_iterator<Mask_Iterator>(mask_begin), value_iterator<Mask_Iterator>(mask_end) };
      
      return base::cookie(c, window, time, cursor, deviceid, mode, paired_device_mode, owner_events, static_cast<uint16_t>(mask.size()), const_cast<const vector_type *>(mask.data()));
    }
};

}

namespace checked {
template<typename Connection>
using xi_grab_device = detail::xi_grab_device<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_xi_grab_device), xcb_input_xi_grab_device>>;
}

namespace unchecked {
template<typename Connection>
using xi_grab_device = detail::xi_grab_device<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_xi_grab_device_unchecked), xcb_input_xi_grab_device_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::xi_grab_device<Connection> xi_grab_device(Connection && c, Parameter && ... parameter) {
  return reply::checked::xi_grab_device<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::xi_grab_device<Connection> xi_grab_device_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::xi_grab_device<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void xi_ungrab_device_checked(Connection && c, xcb_timestamp_t time, xcb_input_device_id_t deviceid) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(std::forward<Connection>(c), xcb_input_xi_ungrab_device_checked(std::forward<Connection>(c), time, deviceid));
}

template<typename Connection>
void xi_ungrab_device(Connection && c, xcb_timestamp_t time, xcb_input_device_id_t deviceid) {
  xcb_input_xi_ungrab_device(std::forward<Connection>(c), time, deviceid);
}

template<typename Connection>
void xi_ungrab_device_checked(Connection && c, xcb_input_device_id_t deviceid, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(std::forward<Connection>(c), xcb_input_xi_ungrab_device_checked(std::forward<Connection>(c), time, deviceid));
}

template<typename Connection>
void xi_ungrab_device(Connection && c, xcb_input_device_id_t deviceid, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xcb_input_xi_ungrab_device(std::forward<Connection>(c), time, deviceid);
}

template<typename Connection>
void xi_allow_events_checked(Connection && c, xcb_timestamp_t time, xcb_input_device_id_t deviceid, uint8_t event_mode, uint32_t touchid, xcb_window_t grab_window) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(std::forward<Connection>(c), xcb_input_xi_allow_events_checked(std::forward<Connection>(c), time, deviceid, event_mode, touchid, grab_window));
}

template<typename Connection>
void xi_allow_events(Connection && c, xcb_timestamp_t time, xcb_input_device_id_t deviceid, uint8_t event_mode, uint32_t touchid, xcb_window_t grab_window) {
  xcb_input_xi_allow_events(std::forward<Connection>(c), time, deviceid, event_mode, touchid, grab_window);
}

template<typename Connection>
void xi_allow_events_checked(Connection && c, xcb_input_device_id_t deviceid, uint8_t event_mode, uint32_t touchid, xcb_window_t grab_window, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(std::forward<Connection>(c), xcb_input_xi_allow_events_checked(std::forward<Connection>(c), time, deviceid, event_mode, touchid, grab_window));
}

template<typename Connection>
void xi_allow_events(Connection && c, xcb_input_device_id_t deviceid, uint8_t event_mode, uint32_t touchid, xcb_window_t grab_window, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xcb_input_xi_allow_events(std::forward<Connection>(c), time, deviceid, event_mode, touchid, grab_window);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class xi_passive_grab_device : public xpp::generic::reply<xi_passive_grab_device<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_xi_passive_grab_device_reply), xcb_input_xi_passive_grab_device_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<xi_passive_grab_device<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_xi_passive_grab_device_reply), xcb_input_xi_passive_grab_device_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    xi_passive_grab_device(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_input_xi_passive_grab_device_cookie_t cookie(xcb_connection_t * const c, xcb_timestamp_t time, xcb_window_t grab_window, xcb_cursor_t cursor, uint32_t detail, xcb_input_device_id_t deviceid, uint16_t num_modifiers, uint16_t mask_len, uint8_t grab_type, uint8_t grab_mode, uint8_t paired_device_mode, uint8_t owner_events, const uint32_t * mask, const uint32_t * modifiers) {
      return base::cookie(c, time, grab_window, cursor, detail, deviceid, num_modifiers, mask_len, grab_type, grab_mode, paired_device_mode, owner_events, mask, modifiers);
    }

    static xcb_input_xi_passive_grab_device_cookie_t cookie(xcb_connection_t * const c, xcb_window_t grab_window, xcb_cursor_t cursor, uint32_t detail, xcb_input_device_id_t deviceid, uint16_t num_modifiers, uint16_t mask_len, uint8_t grab_type, uint8_t grab_mode, uint8_t paired_device_mode, uint8_t owner_events, const uint32_t * mask, const uint32_t * modifiers, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
      return base::cookie(c, time, grab_window, cursor, detail, deviceid, num_modifiers, mask_len, grab_type, grab_mode, paired_device_mode, owner_events, mask, modifiers);
    }

    xpp::generic::list<Connection, xcb_input_xi_passive_grab_device_reply_t, xpp::generic::iterator<Connection,
                                              xcb_input_grab_modifier_info_t,
                                              xpp::generic::signature<decltype(xcb_input_xi_passive_grab_device_modifiers), xcb_input_xi_passive_grab_device_modifiers>,
                                              xpp::generic::signature<decltype(xcb_input_xi_passive_grab_device_modifiers_length), xcb_input_xi_passive_grab_device_modifiers_length>>>
    modifiers() {
      return xpp::generic::list<Connection,
                                xcb_input_xi_passive_grab_device_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_input_grab_modifier_info_t,
                                                       xpp::generic::signature<decltype(xcb_input_xi_passive_grab_device_modifiers), xcb_input_xi_passive_grab_device_modifiers>,
                                                       xpp::generic::signature<decltype(xcb_input_xi_passive_grab_device_modifiers_length), xcb_input_xi_passive_grab_device_modifiers_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using xi_passive_grab_device = detail::xi_passive_grab_device<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_xi_passive_grab_device), xcb_input_xi_passive_grab_device>>;
}

namespace unchecked {
template<typename Connection>
using xi_passive_grab_device = detail::xi_passive_grab_device<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_xi_passive_grab_device_unchecked), xcb_input_xi_passive_grab_device_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::xi_passive_grab_device<Connection> xi_passive_grab_device(Connection && c, Parameter && ... parameter) {
  return reply::checked::xi_passive_grab_device<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::xi_passive_grab_device<Connection> xi_passive_grab_device_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::xi_passive_grab_device<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void xi_passive_ungrab_device_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_input_xi_passive_ungrab_device_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void xi_passive_ungrab_device(Parameter && ... parameter) {
  xcb_input_xi_passive_ungrab_device(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class xi_list_properties : public xpp::generic::reply<xi_list_properties<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_xi_list_properties_reply), xcb_input_xi_list_properties_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<xi_list_properties<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_xi_list_properties_reply), xcb_input_xi_list_properties_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    xi_list_properties(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename Properties = xcb_atom_t>
    xpp::generic::list<Connection, xcb_input_xi_list_properties_reply_t, xpp::generic::iterator<Connection,
                                              Properties,
                                              xpp::generic::signature<decltype(xcb_input_xi_list_properties_properties), xcb_input_xi_list_properties_properties>,
                                              xpp::generic::signature<decltype(xcb_input_xi_list_properties_properties_length), xcb_input_xi_list_properties_properties_length>>>
    properties() {
      return xpp::generic::list<Connection,
                                xcb_input_xi_list_properties_reply_t,
                                xpp::generic::iterator<Connection,
                                                       Properties,
                                                       xpp::generic::signature<decltype(xcb_input_xi_list_properties_properties), xcb_input_xi_list_properties_properties>,
                                                       xpp::generic::signature<decltype(xcb_input_xi_list_properties_properties_length), xcb_input_xi_list_properties_properties_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using xi_list_properties = detail::xi_list_properties<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_xi_list_properties), xcb_input_xi_list_properties>>;
}

namespace unchecked {
template<typename Connection>
using xi_list_properties = detail::xi_list_properties<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_xi_list_properties_unchecked), xcb_input_xi_list_properties_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::xi_list_properties<Connection> xi_list_properties(Connection && c, Parameter && ... parameter) {
  return reply::checked::xi_list_properties<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::xi_list_properties<Connection> xi_list_properties_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::xi_list_properties<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void xi_change_property_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_input_xi_change_property_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void xi_change_property(Parameter && ... parameter) {
  xcb_input_xi_change_property(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void xi_delete_property_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_input_xi_delete_property_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void xi_delete_property(Parameter && ... parameter) {
  xcb_input_xi_delete_property(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class xi_get_property : public xpp::generic::reply<xi_get_property<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_xi_get_property_reply), xcb_input_xi_get_property_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<xi_get_property<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_xi_get_property_reply), xcb_input_xi_get_property_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    xi_get_property(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename ReturnType = xcb_atom_t, typename ... Parameter>
    ReturnType type(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->type),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->type,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using xi_get_property = detail::xi_get_property<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_xi_get_property), xcb_input_xi_get_property>>;
}

namespace unchecked {
template<typename Connection>
using xi_get_property = detail::xi_get_property<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_xi_get_property_unchecked), xcb_input_xi_get_property_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::xi_get_property<Connection> xi_get_property(Connection && c, Parameter && ... parameter) {
  return reply::checked::xi_get_property<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::xi_get_property<Connection> xi_get_property_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::xi_get_property<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class xi_get_selected_events : public xpp::generic::reply<xi_get_selected_events<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_input_xi_get_selected_events_reply), xcb_input_xi_get_selected_events_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<xi_get_selected_events<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_input_xi_get_selected_events_reply), xcb_input_xi_get_selected_events_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    xi_get_selected_events(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_input_xi_get_selected_events_reply_t, xpp::generic::iterator<Connection,
                                              xcb_input_event_mask_t,
                                              xpp::generic::signature<decltype(xcb_input_event_mask_next), xcb_input_event_mask_next>,
                                              xpp::generic::signature<decltype(xcb_input_event_mask_sizeof), xcb_input_event_mask_sizeof>,
                                              xpp::generic::signature<decltype(xcb_input_xi_get_selected_events_masks_iterator), xcb_input_xi_get_selected_events_masks_iterator>>>
    masks() {
      return xpp::generic::list<Connection,
                                xcb_input_xi_get_selected_events_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_input_event_mask_t,
                                                       xpp::generic::signature<decltype(xcb_input_event_mask_next), xcb_input_event_mask_next>,
                                                       xpp::generic::signature<decltype(xcb_input_event_mask_sizeof), xcb_input_event_mask_sizeof>,
                                                       xpp::generic::signature<decltype(xcb_input_xi_get_selected_events_masks_iterator), xcb_input_xi_get_selected_events_masks_iterator>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using xi_get_selected_events = detail::xi_get_selected_events<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_input_xi_get_selected_events), xcb_input_xi_get_selected_events>>;
}

namespace unchecked {
template<typename Connection>
using xi_get_selected_events = detail::xi_get_selected_events<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_input_xi_get_selected_events_unchecked), xcb_input_xi_get_selected_events_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::xi_get_selected_events<Connection> xi_get_selected_events(Connection && c, Parameter && ... parameter) {
  return reply::checked::xi_get_selected_events<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::xi_get_selected_events<Connection> xi_get_selected_events_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::xi_get_selected_events<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void xi_barrier_release_pointer_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_input_xi_barrier_release_pointer_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void xi_barrier_release_pointer(Parameter && ... parameter) {
  xcb_input_xi_barrier_release_pointer(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void send_extension_event_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::input::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_input_send_extension_event_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void send_extension_event(Parameter && ... parameter) {
  xcb_input_send_extension_event(std::forward<Parameter>(parameter) ...);
}



































template<typename Derived, typename Connection>
class interface
{
  protected:
    virtual Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

  public:
    typedef xpp::input::extension extension;


    virtual ~interface() = default;

    const interface<Derived, Connection> &
    input() {
      return *this;
    }

    template<typename ... Parameter>
    auto get_extension_version(Parameter && ... parameter) const -> reply::checked::get_extension_version<Connection> {
      return xpp::input::get_extension_version(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_extension_version_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_extension_version<Connection> {
      return xpp::input::get_extension_version_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto list_input_devices(Parameter && ... parameter) const -> reply::checked::list_input_devices<Connection> {
      return xpp::input::list_input_devices(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto list_input_devices_unchecked(Parameter && ... parameter) const -> reply::unchecked::list_input_devices<Connection> {
      return xpp::input::list_input_devices_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto open_device(Parameter && ... parameter) const -> reply::checked::open_device<Connection> {
      return xpp::input::open_device(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto open_device_unchecked(Parameter && ... parameter) const -> reply::unchecked::open_device<Connection> {
      return xpp::input::open_device_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void close_device_checked(Parameter && ... parameter) const {
      xpp::input::close_device_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void close_device(Parameter && ... parameter) const {
      xpp::input::close_device(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto set_device_mode(Parameter && ... parameter) const -> reply::checked::set_device_mode<Connection> {
      return xpp::input::set_device_mode(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto set_device_mode_unchecked(Parameter && ... parameter) const -> reply::unchecked::set_device_mode<Connection> {
      return xpp::input::set_device_mode_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void select_extension_event_checked(Parameter && ... parameter) const {
      xpp::input::select_extension_event_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void select_extension_event(Parameter && ... parameter) const {
      xpp::input::select_extension_event(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_selected_extension_events(Parameter && ... parameter) const -> reply::checked::get_selected_extension_events<Connection> {
      return xpp::input::get_selected_extension_events(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_selected_extension_events_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_selected_extension_events<Connection> {
      return xpp::input::get_selected_extension_events_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void change_device_dont_propagate_list_checked(Parameter && ... parameter) const {
      xpp::input::change_device_dont_propagate_list_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void change_device_dont_propagate_list(Parameter && ... parameter) const {
      xpp::input::change_device_dont_propagate_list(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_device_dont_propagate_list(Parameter && ... parameter) const -> reply::checked::get_device_dont_propagate_list<Connection> {
      return xpp::input::get_device_dont_propagate_list(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_device_dont_propagate_list_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_device_dont_propagate_list<Connection> {
      return xpp::input::get_device_dont_propagate_list_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_device_motion_events(Parameter && ... parameter) const -> reply::checked::get_device_motion_events<Connection> {
      return xpp::input::get_device_motion_events(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_device_motion_events_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_device_motion_events<Connection> {
      return xpp::input::get_device_motion_events_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto change_keyboard_device(Parameter && ... parameter) const -> reply::checked::change_keyboard_device<Connection> {
      return xpp::input::change_keyboard_device(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto change_keyboard_device_unchecked(Parameter && ... parameter) const -> reply::unchecked::change_keyboard_device<Connection> {
      return xpp::input::change_keyboard_device_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto change_pointer_device(Parameter && ... parameter) const -> reply::checked::change_pointer_device<Connection> {
      return xpp::input::change_pointer_device(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto change_pointer_device_unchecked(Parameter && ... parameter) const -> reply::unchecked::change_pointer_device<Connection> {
      return xpp::input::change_pointer_device_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto grab_device(Parameter && ... parameter) const -> reply::checked::grab_device<Connection> {
      return xpp::input::grab_device(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto grab_device_unchecked(Parameter && ... parameter) const -> reply::unchecked::grab_device<Connection> {
      return xpp::input::grab_device_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void ungrab_device_checked(Parameter && ... parameter) const {
      xpp::input::ungrab_device_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void ungrab_device(Parameter && ... parameter) const {
      xpp::input::ungrab_device(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void grab_device_key_checked(Parameter && ... parameter) const {
      xpp::input::grab_device_key_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void grab_device_key(Parameter && ... parameter) const {
      xpp::input::grab_device_key(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void ungrab_device_key_checked(Parameter && ... parameter) const {
      xpp::input::ungrab_device_key_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void ungrab_device_key(Parameter && ... parameter) const {
      xpp::input::ungrab_device_key(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void grab_device_button_checked(Parameter && ... parameter) const {
      xpp::input::grab_device_button_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void grab_device_button(Parameter && ... parameter) const {
      xpp::input::grab_device_button(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void ungrab_device_button_checked(Parameter && ... parameter) const {
      xpp::input::ungrab_device_button_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void ungrab_device_button(Parameter && ... parameter) const {
      xpp::input::ungrab_device_button(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void allow_device_events_checked(Parameter && ... parameter) const {
      xpp::input::allow_device_events_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void allow_device_events(Parameter && ... parameter) const {
      xpp::input::allow_device_events(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_device_focus(Parameter && ... parameter) const -> reply::checked::get_device_focus<Connection> {
      return xpp::input::get_device_focus(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_device_focus_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_device_focus<Connection> {
      return xpp::input::get_device_focus_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_device_focus_checked(Parameter && ... parameter) const {
      xpp::input::set_device_focus_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_device_focus(Parameter && ... parameter) const {
      xpp::input::set_device_focus(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_feedback_control(Parameter && ... parameter) const -> reply::checked::get_feedback_control<Connection> {
      return xpp::input::get_feedback_control(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_feedback_control_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_feedback_control<Connection> {
      return xpp::input::get_feedback_control_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void change_feedback_control_checked(Parameter && ... parameter) const {
      xpp::input::change_feedback_control_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void change_feedback_control(Parameter && ... parameter) const {
      xpp::input::change_feedback_control(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_device_key_mapping(Parameter && ... parameter) const -> reply::checked::get_device_key_mapping<Connection> {
      return xpp::input::get_device_key_mapping(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_device_key_mapping_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_device_key_mapping<Connection> {
      return xpp::input::get_device_key_mapping_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void change_device_key_mapping_checked(Parameter && ... parameter) const {
      xpp::input::change_device_key_mapping_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void change_device_key_mapping(Parameter && ... parameter) const {
      xpp::input::change_device_key_mapping(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_device_modifier_mapping(Parameter && ... parameter) const -> reply::checked::get_device_modifier_mapping<Connection> {
      return xpp::input::get_device_modifier_mapping(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_device_modifier_mapping_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_device_modifier_mapping<Connection> {
      return xpp::input::get_device_modifier_mapping_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto set_device_modifier_mapping(Parameter && ... parameter) const -> reply::checked::set_device_modifier_mapping<Connection> {
      return xpp::input::set_device_modifier_mapping(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto set_device_modifier_mapping_unchecked(Parameter && ... parameter) const -> reply::unchecked::set_device_modifier_mapping<Connection> {
      return xpp::input::set_device_modifier_mapping_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_device_button_mapping(Parameter && ... parameter) const -> reply::checked::get_device_button_mapping<Connection> {
      return xpp::input::get_device_button_mapping(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_device_button_mapping_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_device_button_mapping<Connection> {
      return xpp::input::get_device_button_mapping_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto set_device_button_mapping(Parameter && ... parameter) const -> reply::checked::set_device_button_mapping<Connection> {
      return xpp::input::set_device_button_mapping(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto set_device_button_mapping_unchecked(Parameter && ... parameter) const -> reply::unchecked::set_device_button_mapping<Connection> {
      return xpp::input::set_device_button_mapping_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_device_state(Parameter && ... parameter) const -> reply::checked::query_device_state<Connection> {
      return xpp::input::query_device_state(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_device_state_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_device_state<Connection> {
      return xpp::input::query_device_state_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void device_bell_checked(Parameter && ... parameter) const {
      xpp::input::device_bell_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void device_bell(Parameter && ... parameter) const {
      xpp::input::device_bell(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto set_device_valuators(Parameter && ... parameter) const -> reply::checked::set_device_valuators<Connection> {
      return xpp::input::set_device_valuators(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto set_device_valuators_unchecked(Parameter && ... parameter) const -> reply::unchecked::set_device_valuators<Connection> {
      return xpp::input::set_device_valuators_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_device_control(Parameter && ... parameter) const -> reply::checked::get_device_control<Connection> {
      return xpp::input::get_device_control(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_device_control_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_device_control<Connection> {
      return xpp::input::get_device_control_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto change_device_control(Parameter && ... parameter) const -> reply::checked::change_device_control<Connection> {
      return xpp::input::change_device_control(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto change_device_control_unchecked(Parameter && ... parameter) const -> reply::unchecked::change_device_control<Connection> {
      return xpp::input::change_device_control_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto list_device_properties(Parameter && ... parameter) const -> reply::checked::list_device_properties<Connection> {
      return xpp::input::list_device_properties(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto list_device_properties_unchecked(Parameter && ... parameter) const -> reply::unchecked::list_device_properties<Connection> {
      return xpp::input::list_device_properties_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void change_device_property_checked(Parameter && ... parameter) const {
      xpp::input::change_device_property_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void change_device_property(Parameter && ... parameter) const {
      xpp::input::change_device_property(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void delete_device_property_checked(Parameter && ... parameter) const {
      xpp::input::delete_device_property_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void delete_device_property(Parameter && ... parameter) const {
      xpp::input::delete_device_property(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_device_property(Parameter && ... parameter) const -> reply::checked::get_device_property<Connection> {
      return xpp::input::get_device_property(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_device_property_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_device_property<Connection> {
      return xpp::input::get_device_property_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto xi_query_pointer(Parameter && ... parameter) const -> reply::checked::xi_query_pointer<Connection> {
      return xpp::input::xi_query_pointer(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto xi_query_pointer_unchecked(Parameter && ... parameter) const -> reply::unchecked::xi_query_pointer<Connection> {
      return xpp::input::xi_query_pointer_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void xi_warp_pointer_checked(Parameter && ... parameter) const {
      xpp::input::xi_warp_pointer_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void xi_warp_pointer(Parameter && ... parameter) const {
      xpp::input::xi_warp_pointer(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void xi_change_cursor_checked(Parameter && ... parameter) const {
      xpp::input::xi_change_cursor_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void xi_change_cursor(Parameter && ... parameter) const {
      xpp::input::xi_change_cursor(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void xi_change_hierarchy_checked(Parameter && ... parameter) const {
      xpp::input::xi_change_hierarchy_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void xi_change_hierarchy(Parameter && ... parameter) const {
      xpp::input::xi_change_hierarchy(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void xi_set_client_pointer_checked(Parameter && ... parameter) const {
      xpp::input::xi_set_client_pointer_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void xi_set_client_pointer(Parameter && ... parameter) const {
      xpp::input::xi_set_client_pointer(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto xi_get_client_pointer(Parameter && ... parameter) const -> reply::checked::xi_get_client_pointer<Connection> {
      return xpp::input::xi_get_client_pointer(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto xi_get_client_pointer_unchecked(Parameter && ... parameter) const -> reply::unchecked::xi_get_client_pointer<Connection> {
      return xpp::input::xi_get_client_pointer_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void xi_select_events_checked(Parameter && ... parameter) const {
      xpp::input::xi_select_events_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void xi_select_events(Parameter && ... parameter) const {
      xpp::input::xi_select_events(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto xi_query_version(Parameter && ... parameter) const -> reply::checked::xi_query_version<Connection> {
      return xpp::input::xi_query_version(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto xi_query_version_unchecked(Parameter && ... parameter) const -> reply::unchecked::xi_query_version<Connection> {
      return xpp::input::xi_query_version_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto xi_query_device(Parameter && ... parameter) const -> reply::checked::xi_query_device<Connection> {
      return xpp::input::xi_query_device(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto xi_query_device_unchecked(Parameter && ... parameter) const -> reply::unchecked::xi_query_device<Connection> {
      return xpp::input::xi_query_device_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void xi_set_focus_checked(Parameter && ... parameter) const {
      xpp::input::xi_set_focus_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void xi_set_focus(Parameter && ... parameter) const {
      xpp::input::xi_set_focus(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto xi_get_focus(Parameter && ... parameter) const -> reply::checked::xi_get_focus<Connection> {
      return xpp::input::xi_get_focus(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto xi_get_focus_unchecked(Parameter && ... parameter) const -> reply::unchecked::xi_get_focus<Connection> {
      return xpp::input::xi_get_focus_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto xi_grab_device(Parameter && ... parameter) const -> reply::checked::xi_grab_device<Connection> {
      return xpp::input::xi_grab_device(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto xi_grab_device_unchecked(Parameter && ... parameter) const -> reply::unchecked::xi_grab_device<Connection> {
      return xpp::input::xi_grab_device_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void xi_ungrab_device_checked(Parameter && ... parameter) const {
      xpp::input::xi_ungrab_device_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void xi_ungrab_device(Parameter && ... parameter) const {
      xpp::input::xi_ungrab_device(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void xi_allow_events_checked(Parameter && ... parameter) const {
      xpp::input::xi_allow_events_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void xi_allow_events(Parameter && ... parameter) const {
      xpp::input::xi_allow_events(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto xi_passive_grab_device(Parameter && ... parameter) const -> reply::checked::xi_passive_grab_device<Connection> {
      return xpp::input::xi_passive_grab_device(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto xi_passive_grab_device_unchecked(Parameter && ... parameter) const -> reply::unchecked::xi_passive_grab_device<Connection> {
      return xpp::input::xi_passive_grab_device_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void xi_passive_ungrab_device_checked(Parameter && ... parameter) const {
      xpp::input::xi_passive_ungrab_device_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void xi_passive_ungrab_device(Parameter && ... parameter) const {
      xpp::input::xi_passive_ungrab_device(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto xi_list_properties(Parameter && ... parameter) const -> reply::checked::xi_list_properties<Connection> {
      return xpp::input::xi_list_properties(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto xi_list_properties_unchecked(Parameter && ... parameter) const -> reply::unchecked::xi_list_properties<Connection> {
      return xpp::input::xi_list_properties_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void xi_change_property_checked(Parameter && ... parameter) const {
      xpp::input::xi_change_property_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void xi_change_property(Parameter && ... parameter) const {
      xpp::input::xi_change_property(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void xi_delete_property_checked(Parameter && ... parameter) const {
      xpp::input::xi_delete_property_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void xi_delete_property(Parameter && ... parameter) const {
      xpp::input::xi_delete_property(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto xi_get_property(Parameter && ... parameter) const -> reply::checked::xi_get_property<Connection> {
      return xpp::input::xi_get_property(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto xi_get_property_unchecked(Parameter && ... parameter) const -> reply::unchecked::xi_get_property<Connection> {
      return xpp::input::xi_get_property_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto xi_get_selected_events(Parameter && ... parameter) const -> reply::checked::xi_get_selected_events<Connection> {
      return xpp::input::xi_get_selected_events(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto xi_get_selected_events_unchecked(Parameter && ... parameter) const -> reply::unchecked::xi_get_selected_events<Connection> {
      return xpp::input::xi_get_selected_events_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void xi_barrier_release_pointer_checked(Parameter && ... parameter) const {
      xpp::input::xi_barrier_release_pointer_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void xi_barrier_release_pointer(Parameter && ... parameter) const {
      xpp::input::xi_barrier_release_pointer(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void send_extension_event_checked(Parameter && ... parameter) const {
      xpp::input::send_extension_event_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void send_extension_event(Parameter && ... parameter) const {
      xpp::input::send_extension_event(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


};

namespace event {

template<typename Connection>
class dispatcher {
  public:
    typedef xpp::input::extension extension;

    template<typename C>
    dispatcher(C && c, uint8_t first_event) : m_c(std::forward<C>(c)) , m_first_event(first_event) {}

    template<typename C>
    dispatcher(C && c, const xpp::input::extension & extension) : dispatcher(std::forward<C>(c), extension->first_event) {}

    template<typename Handler>
    bool operator()(Handler handler, const std::shared_ptr<xcb_generic_event_t> & event) const {
      switch ((event->response_type & ~0x80) - m_first_event) {

        case XCB_INPUT_DEVICE_VALUATOR:
          handler(xpp::input::event::device_valuator<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_DEVICE_KEY_PRESS:
          handler(xpp::input::event::device_key_press<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_DEVICE_KEY_RELEASE:
          handler(xpp::input::event::device_key_release<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_DEVICE_BUTTON_PRESS:
          handler(xpp::input::event::device_button_press<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_DEVICE_BUTTON_RELEASE:
          handler(xpp::input::event::device_button_release<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_DEVICE_MOTION_NOTIFY:
          handler(xpp::input::event::device_motion_notify<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_DEVICE_FOCUS_IN:
          handler(xpp::input::event::device_focus_in<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_DEVICE_FOCUS_OUT:
          handler(xpp::input::event::device_focus_out<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_PROXIMITY_IN:
          handler(xpp::input::event::proximity_in<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_PROXIMITY_OUT:
          handler(xpp::input::event::proximity_out<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_DEVICE_STATE_NOTIFY:
          handler(xpp::input::event::device_state_notify<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_DEVICE_MAPPING_NOTIFY:
          handler(xpp::input::event::device_mapping_notify<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_CHANGE_DEVICE_NOTIFY:
          handler(xpp::input::event::change_device_notify<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_DEVICE_KEY_STATE_NOTIFY:
          handler(xpp::input::event::device_key_state_notify<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_DEVICE_BUTTON_STATE_NOTIFY:
          handler(xpp::input::event::device_button_state_notify<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_DEVICE_PRESENCE_NOTIFY:
          handler(xpp::input::event::device_presence_notify<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_DEVICE_PROPERTY_NOTIFY:
          handler(xpp::input::event::device_property_notify<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_DEVICE_CHANGED:
          handler(xpp::input::event::device_changed<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_KEY_PRESS:
          handler(xpp::input::event::key_press<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_KEY_RELEASE:
          handler(xpp::input::event::key_release<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_BUTTON_PRESS:
          handler(xpp::input::event::button_press<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_BUTTON_RELEASE:
          handler(xpp::input::event::button_release<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_MOTION:
          handler(xpp::input::event::motion<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_ENTER:
          handler(xpp::input::event::enter<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_LEAVE:
          handler(xpp::input::event::leave<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_FOCUS_IN:
          handler(xpp::input::event::focus_in<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_FOCUS_OUT:
          handler(xpp::input::event::focus_out<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_HIERARCHY:
          handler(xpp::input::event::hierarchy<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_PROPERTY:
          handler(xpp::input::event::property<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_RAW_KEY_PRESS:
          handler(xpp::input::event::raw_key_press<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_RAW_KEY_RELEASE:
          handler(xpp::input::event::raw_key_release<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_RAW_BUTTON_PRESS:
          handler(xpp::input::event::raw_button_press<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_RAW_BUTTON_RELEASE:
          handler(xpp::input::event::raw_button_release<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_RAW_MOTION:
          handler(xpp::input::event::raw_motion<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_TOUCH_BEGIN:
          handler(xpp::input::event::touch_begin<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_TOUCH_UPDATE:
          handler(xpp::input::event::touch_update<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_TOUCH_END:
          handler(xpp::input::event::touch_end<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_TOUCH_OWNERSHIP:
          handler(xpp::input::event::touch_ownership<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_RAW_TOUCH_BEGIN:
          handler(xpp::input::event::raw_touch_begin<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_RAW_TOUCH_UPDATE:
          handler(xpp::input::event::raw_touch_update<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_RAW_TOUCH_END:
          handler(xpp::input::event::raw_touch_end<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_BARRIER_HIT:
          handler(xpp::input::event::barrier_hit<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_BARRIER_LEAVE:
          handler(xpp::input::event::barrier_leave<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_GESTURE_PINCH_BEGIN:
          handler(xpp::input::event::gesture_pinch_begin<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_GESTURE_PINCH_UPDATE:
          handler(xpp::input::event::gesture_pinch_update<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_GESTURE_PINCH_END:
          handler(xpp::input::event::gesture_pinch_end<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_GESTURE_SWIPE_BEGIN:
          handler(xpp::input::event::gesture_swipe_begin<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_GESTURE_SWIPE_UPDATE:
          handler(xpp::input::event::gesture_swipe_update<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_INPUT_GESTURE_SWIPE_END:
          handler(xpp::input::event::gesture_swipe_end<Connection>(m_c, m_first_event, event));
          return true;

      };

      return false;
    }

  protected:
    Connection m_c;
    uint8_t m_first_event;
};

}

namespace error {

class dispatcher {
  public:
    typedef xpp::input::extension extension;

    dispatcher(uint8_t first_error) : m_first_error(first_error) {}

    dispatcher(const xpp::input::extension & extension) : dispatcher(extension->first_error) {}

    void operator()(const std::shared_ptr<xcb_generic_error_t> & error) const {
      switch (error->error_code - m_first_error) {

        case XCB_INPUT_DEVICE: // 0
          throw xpp::input::error::device(error);

        case XCB_INPUT_EVENT: // 1
          throw xpp::input::error::event(error);

        case XCB_INPUT_MODE: // 2
          throw xpp::input::error::mode(error);

        case XCB_INPUT_DEVICE_BUSY: // 3
          throw xpp::input::error::device_busy(error);

        case XCB_INPUT_CLASS: // 4
          throw xpp::input::error::_class(error);

      };
    }

  protected:
    uint8_t m_first_error;
};

}


};

#endif // XPP_INPUT_HPP
