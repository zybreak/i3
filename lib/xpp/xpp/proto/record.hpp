#ifndef XPP_RECORD_HPP
#define XPP_RECORD_HPP

#include <string>
#include <vector>

#include <xcb/record.h>

#include "xpp/generic.hpp"

namespace xpp::record {

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
  : public xpp::generic::extension<extension, &xcb_record_id>
 {
  public:
    using base = xpp::generic::extension<extension, &xcb_record_id>;
    using base::base;
    template<typename Derived, typename Connection>
    using interface = xpp::record::interface<Derived, Connection>;
    template<typename Connection>
    using event_dispatcher = xpp::record::event::dispatcher<Connection>;
    using error_dispatcher = xpp::record::error::dispatcher;
};


namespace error {
class bad_context : public xpp::generic::error<bad_context, xcb_record_bad_context_error_t> {
  public:
    using xpp::generic::error<bad_context, xcb_record_bad_context_error_t>::error;

    virtual ~bad_context() = default;

    static uint8_t opcode() {
      return XCB_RECORD_BAD_CONTEXT;
    }
    
    static uint8_t opcode(uint8_t first_error) {
      return first_error + opcode();
    }
    
    static uint8_t opcode(const xpp::record::extension & extension) {
      return opcode(extension->first_error);
    }

    static std::string description() {
      return std::string("XCB_RECORD_BAD_CONTEXT");
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
                               xpp::generic::signature<decltype(xcb_record_query_version_reply), xcb_record_query_version_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_record_query_version_reply), xcb_record_query_version_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_version(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_record_query_version), xcb_record_query_version>>;
}

namespace unchecked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_record_query_version_unchecked), xcb_record_query_version_unchecked>>;
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
void create_context_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::record::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_record_create_context_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void create_context(Parameter && ... parameter) {
  xcb_record_create_context(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void register_clients_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::record::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_record_register_clients_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void register_clients(Parameter && ... parameter) {
  xcb_record_register_clients(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void unregister_clients_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::record::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_record_unregister_clients_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void unregister_clients(Parameter && ... parameter) {
  xcb_record_unregister_clients(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_context : public xpp::generic::reply<get_context<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_record_get_context_reply), xcb_record_get_context_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_context<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_record_get_context_reply), xcb_record_get_context_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_context(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_record_get_context_reply_t, xpp::generic::iterator<Connection,
                                              xcb_record_client_info_t,
                                              xpp::generic::signature<decltype(xcb_record_client_info_next), xcb_record_client_info_next>,
                                              xpp::generic::signature<decltype(xcb_record_client_info_sizeof), xcb_record_client_info_sizeof>,
                                              xpp::generic::signature<decltype(xcb_record_get_context_intercepted_clients_iterator), xcb_record_get_context_intercepted_clients_iterator>>>
    intercepted_clients() {
      return xpp::generic::list<Connection,
                                xcb_record_get_context_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_record_client_info_t,
                                                       xpp::generic::signature<decltype(xcb_record_client_info_next), xcb_record_client_info_next>,
                                                       xpp::generic::signature<decltype(xcb_record_client_info_sizeof), xcb_record_client_info_sizeof>,
                                                       xpp::generic::signature<decltype(xcb_record_get_context_intercepted_clients_iterator), xcb_record_get_context_intercepted_clients_iterator>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_context = detail::get_context<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_record_get_context), xcb_record_get_context>>;
}

namespace unchecked {
template<typename Connection>
using get_context = detail::get_context<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_record_get_context_unchecked), xcb_record_get_context_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_context<Connection> get_context(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_context<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_context<Connection> get_context_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_context<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class enable_context : public xpp::generic::reply<enable_context<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_record_enable_context_reply), xcb_record_enable_context_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<enable_context<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_record_enable_context_reply), xcb_record_enable_context_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    enable_context(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_record_enable_context_reply_t, xpp::generic::iterator<Connection,
                                              uint8_t,
                                              xpp::generic::signature<decltype(xcb_record_enable_context_data), xcb_record_enable_context_data>,
                                              xpp::generic::signature<decltype(xcb_record_enable_context_data_length), xcb_record_enable_context_data_length>>>
    data() {
      return xpp::generic::list<Connection,
                                xcb_record_enable_context_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint8_t,
                                                       xpp::generic::signature<decltype(xcb_record_enable_context_data), xcb_record_enable_context_data>,
                                                       xpp::generic::signature<decltype(xcb_record_enable_context_data_length), xcb_record_enable_context_data_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using enable_context = detail::enable_context<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_record_enable_context), xcb_record_enable_context>>;
}

namespace unchecked {
template<typename Connection>
using enable_context = detail::enable_context<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_record_enable_context_unchecked), xcb_record_enable_context_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::enable_context<Connection> enable_context(Connection && c, Parameter && ... parameter) {
  return reply::checked::enable_context<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::enable_context<Connection> enable_context_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::enable_context<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void disable_context_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::record::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_record_disable_context_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void disable_context(Parameter && ... parameter) {
  xcb_record_disable_context(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void free_context_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::record::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_record_free_context_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void free_context(Parameter && ... parameter) {
  xcb_record_free_context(std::forward<Parameter>(parameter) ...);
}
























template<typename Derived, typename Connection>
class context
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_record_context_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~context() = default;

    template<typename ... Parameter>
    void create_checked(Parameter && ... parameter) const {
      xpp::record::create_context_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create(Parameter && ... parameter) const {
      xpp::record::create_context(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void register_clients_checked(Parameter && ... parameter) const {
      xpp::record::register_clients_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void register_clients(Parameter && ... parameter) const {
      xpp::record::register_clients(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void unregister_clients_checked(Parameter && ... parameter) const {
      xpp::record::unregister_clients_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void unregister_clients(Parameter && ... parameter) const {
      xpp::record::unregister_clients(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get(Parameter && ... parameter) const -> reply::checked::get_context<Connection> {
      return xpp::record::get_context(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_context<Connection> {
      return xpp::record::get_context_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto enable(Parameter && ... parameter) const -> reply::checked::enable_context<Connection> {
      return xpp::record::enable_context(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto enable_unchecked(Parameter && ... parameter) const -> reply::unchecked::enable_context<Connection> {
      return xpp::record::enable_context_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void disable_checked(Parameter && ... parameter) const {
      xpp::record::disable_context_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void disable(Parameter && ... parameter) const {
      xpp::record::disable_context(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void free_checked(Parameter && ... parameter) const {
      xpp::record::free_context_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void free(Parameter && ... parameter) const {
      xpp::record::free_context(connection(),
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
    typedef xpp::record::extension extension;


    virtual ~interface() = default;

    const interface<Derived, Connection> &
    record() {
      return *this;
    }

    template<typename ... Parameter>
    auto query_version(Parameter && ... parameter) const -> reply::checked::query_version<Connection> {
      return xpp::record::query_version(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_version_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_version<Connection> {
      return xpp::record::query_version_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_context_checked(Parameter && ... parameter) const {
      xpp::record::create_context_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_context(Parameter && ... parameter) const {
      xpp::record::create_context(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void register_clients_checked(Parameter && ... parameter) const {
      xpp::record::register_clients_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void register_clients(Parameter && ... parameter) const {
      xpp::record::register_clients(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void unregister_clients_checked(Parameter && ... parameter) const {
      xpp::record::unregister_clients_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void unregister_clients(Parameter && ... parameter) const {
      xpp::record::unregister_clients(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_context(Parameter && ... parameter) const -> reply::checked::get_context<Connection> {
      return xpp::record::get_context(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_context_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_context<Connection> {
      return xpp::record::get_context_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto enable_context(Parameter && ... parameter) const -> reply::checked::enable_context<Connection> {
      return xpp::record::enable_context(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto enable_context_unchecked(Parameter && ... parameter) const -> reply::unchecked::enable_context<Connection> {
      return xpp::record::enable_context_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void disable_context_checked(Parameter && ... parameter) const {
      xpp::record::disable_context_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void disable_context(Parameter && ... parameter) const {
      xpp::record::disable_context(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void free_context_checked(Parameter && ... parameter) const {
      xpp::record::free_context_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void free_context(Parameter && ... parameter) const {
      xpp::record::free_context(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


};

namespace event {

template<typename Connection>
class dispatcher {
  public:
    typedef xpp::record::extension extension;

    template<typename C>
    dispatcher(C && c, uint8_t first_event) : m_c(std::forward<C>(c)) , m_first_event(first_event) {}

    template<typename C>
    dispatcher(C && c, const xpp::record::extension & extension) : dispatcher(std::forward<C>(c), extension->first_event) {}

    template<typename Handler>
    bool operator()(Handler, const std::shared_ptr<xcb_generic_event_t> &) const {
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
    typedef xpp::record::extension extension;

    dispatcher(uint8_t first_error) : m_first_error(first_error) {}

    dispatcher(const xpp::record::extension & extension) : dispatcher(extension->first_error) {}

    void operator()(const std::shared_ptr<xcb_generic_error_t> & error) const {
      switch (error->error_code - m_first_error) {

        case XCB_RECORD_BAD_CONTEXT: // 0
          throw xpp::record::error::bad_context(error);

      };
    }

  protected:
    uint8_t m_first_error;
};

}


};

#endif // XPP_RECORD_HPP
