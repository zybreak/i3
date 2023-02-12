#ifndef XPP_SHM_HPP
#define XPP_SHM_HPP

#include <string>
#include <vector>

#include <xcb/shm.h>

#include "xpp/generic.hpp"

namespace xpp::shm {

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
  : public xpp::generic::extension<extension, &xcb_shm_id>
 {
  public:
    using base = xpp::generic::extension<extension, &xcb_shm_id>;
    using base::base;
    template<typename Derived, typename Connection>
    using interface = xpp::shm::interface<Derived, Connection>;
    template<typename Connection>
    using event_dispatcher = xpp::shm::event::dispatcher<Connection>;
    using error_dispatcher = xpp::shm::error::dispatcher;
};

namespace event {
template<typename Connection>
class completion : public xpp::generic::event<xcb_shm_completion_event_t> {
  public:
    typedef xpp::shm::extension extension;

    typedef xpp::generic::event<xcb_shm_completion_event_t> base;

    template<typename C>
    completion(C && c,
               uint8_t first_event,
               const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~completion() = default;

    static uint8_t opcode() {
      return XCB_SHM_COMPLETION;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::shm::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_SHM_COMPLETION");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_drawable_t, typename ... Parameter>
    ReturnType drawable(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->drawable),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->drawable,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_shm_seg_t, typename ... Parameter>
    ReturnType shmseg(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->shmseg),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->shmseg,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}



namespace error {
class bad_seg : public xpp::generic::error<bad_seg, xcb_value_error_t> {
  public:
    using xpp::generic::error<bad_seg, xcb_value_error_t>::error;

    virtual ~bad_seg() = default;

    static uint8_t opcode() {
      return XCB_SHM_BAD_SEG;
    }
    
    static uint8_t opcode(uint8_t first_error) {
      return first_error + opcode();
    }
    
    static uint8_t opcode(const xpp::shm::extension & extension) {
      return opcode(extension->first_error);
    }

    static std::string description() {
      return std::string("XCB_SHM_BAD_SEG");
    }

  protected:
    uint8_t m_first_error;
};
} 


namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_version : public xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_shm_query_version_reply), xcb_shm_query_version_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_shm_query_version_reply), xcb_shm_query_version_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_version(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_shm_query_version), xcb_shm_query_version>>;
}

namespace unchecked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_shm_query_version_unchecked), xcb_shm_query_version_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_version<Connection> query_version(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_version<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_version<Connection> query_version_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_version<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void attach_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::shm::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_shm_attach_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void attach(Parameter && ... parameter) {
  xcb_shm_attach(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void detach_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::shm::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_shm_detach_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void detach(Parameter && ... parameter) {
  xcb_shm_detach(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void put_image_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::shm::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_shm_put_image_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void put_image(Parameter && ... parameter) {
  xcb_shm_put_image(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_image : public xpp::generic::reply<get_image<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_shm_get_image_reply), xcb_shm_get_image_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_image<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_shm_get_image_reply), xcb_shm_get_image_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_image(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using get_image = detail::get_image<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_shm_get_image), xcb_shm_get_image>>;
}

namespace unchecked {
template<typename Connection>
using get_image = detail::get_image<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_shm_get_image_unchecked), xcb_shm_get_image_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_image<Connection> get_image(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_image<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_image<Connection> get_image_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_image<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void create_pixmap_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::shm::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_shm_create_pixmap_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void create_pixmap(Parameter && ... parameter) {
  xcb_shm_create_pixmap(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void attach_fd_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::shm::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_shm_attach_fd_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void attach_fd(Parameter && ... parameter) {
  xcb_shm_attach_fd(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class create_segment : public xpp::generic::reply<create_segment<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_shm_create_segment_reply), xcb_shm_create_segment_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<create_segment<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_shm_create_segment_reply), xcb_shm_create_segment_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    create_segment(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using create_segment = detail::create_segment<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_shm_create_segment), xcb_shm_create_segment>>;
}

namespace unchecked {
template<typename Connection>
using create_segment = detail::create_segment<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_shm_create_segment_unchecked), xcb_shm_create_segment_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::create_segment<Connection> create_segment(Connection && c, Parameter && ... parameter) {
  return reply::checked::create_segment<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::create_segment<Connection> create_segment_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::create_segment<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}




















template<typename Derived, typename Connection>
class seg
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_shm_seg_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~seg() = default;

    template<typename ... Parameter>
    void attach_checked(Parameter && ... parameter) const {
      xpp::shm::attach_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void attach(Parameter && ... parameter) const {
      xpp::shm::attach(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void detach_checked(Parameter && ... parameter) const {
      xpp::shm::detach_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void detach(Parameter && ... parameter) const {
      xpp::shm::detach(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void attach_fd_checked(Parameter && ... parameter) const {
      xpp::shm::attach_fd_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void attach_fd(Parameter && ... parameter) const {
      xpp::shm::attach_fd(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto createment(Parameter && ... parameter) const -> reply::checked::create_segment<Connection> {
      return xpp::shm::create_segment(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto createment_unchecked(Parameter && ... parameter) const -> reply::unchecked::create_segment<Connection> {
      return xpp::shm::create_segment_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }



};















template<typename Derived, typename Connection>
class interface
{
  protected:
    virtual Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

  public:
    typedef xpp::shm::extension extension;


    virtual ~interface() = default;

    const interface<Derived, Connection> &
    shm() {
      return *this;
    }

    template<typename ... Parameter>
    auto query_version(Parameter && ... parameter) const -> reply::checked::query_version<Connection> {
      return xpp::shm::query_version(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_version_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_version<Connection> {
      return xpp::shm::query_version_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void attach_checked(Parameter && ... parameter) const {
      xpp::shm::attach_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void attach(Parameter && ... parameter) const {
      xpp::shm::attach(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void detach_checked(Parameter && ... parameter) const {
      xpp::shm::detach_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void detach(Parameter && ... parameter) const {
      xpp::shm::detach(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void put_image_checked(Parameter && ... parameter) const {
      xpp::shm::put_image_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void put_image(Parameter && ... parameter) const {
      xpp::shm::put_image(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_image(Parameter && ... parameter) const -> reply::checked::get_image<Connection> {
      return xpp::shm::get_image(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_image_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_image<Connection> {
      return xpp::shm::get_image_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_pixmap_checked(Parameter && ... parameter) const {
      xpp::shm::create_pixmap_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_pixmap(Parameter && ... parameter) const {
      xpp::shm::create_pixmap(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void attach_fd_checked(Parameter && ... parameter) const {
      xpp::shm::attach_fd_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void attach_fd(Parameter && ... parameter) const {
      xpp::shm::attach_fd(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto create_segment(Parameter && ... parameter) const -> reply::checked::create_segment<Connection> {
      return xpp::shm::create_segment(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto create_segment_unchecked(Parameter && ... parameter) const -> reply::unchecked::create_segment<Connection> {
      return xpp::shm::create_segment_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


};

namespace event {

template<typename Connection>
class dispatcher {
  public:
    typedef xpp::shm::extension extension;

    template<typename C>
    dispatcher(C && c, uint8_t first_event) : m_c(std::forward<C>(c)) , m_first_event(first_event) {}

    template<typename C>
    dispatcher(C && c, const xpp::shm::extension & extension) : dispatcher(std::forward<C>(c), extension->first_event) {}

    template<typename Handler>
    bool operator()(Handler handler, const std::shared_ptr<xcb_generic_event_t> & event) const {
      switch ((event->response_type & ~0x80) - m_first_event) {

        case XCB_SHM_COMPLETION:
          handler(xpp::shm::event::completion<Connection>(m_c, m_first_event, event));
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
    typedef xpp::shm::extension extension;

    dispatcher(uint8_t first_error) : m_first_error(first_error) {}

    dispatcher(const xpp::shm::extension & extension) : dispatcher(extension->first_error) {}

    void operator()(const std::shared_ptr<xcb_generic_error_t> & error) const {
      switch (error->error_code - m_first_error) {

        case XCB_SHM_BAD_SEG: // 0
          throw xpp::shm::error::bad_seg(error);

      };
    }

  protected:
    uint8_t m_first_error;
};

}


};

#endif // XPP_SHM_HPP
