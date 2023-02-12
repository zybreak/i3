#ifndef XPP_X_PRINT_HPP
#define XPP_X_PRINT_HPP

#include <string>
#include <vector>

#include <xcb/xprint.h>

#include "xpp/generic.hpp"

namespace xpp::x_print {

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
  : public xpp::generic::extension<extension, &xcb_x_print_id>
 {
  public:
    using base = xpp::generic::extension<extension, &xcb_x_print_id>;
    using base::base;
    template<typename Derived, typename Connection>
    using interface = xpp::x_print::interface<Derived, Connection>;
    template<typename Connection>
    using event_dispatcher = xpp::x_print::event::dispatcher<Connection>;
    using error_dispatcher = xpp::x_print::error::dispatcher;
};

namespace event {
template<typename Connection>
class notify : public xpp::generic::event<xcb_x_print_notify_event_t> {
  public:
    typedef xpp::x_print::extension extension;

    typedef xpp::generic::event<xcb_x_print_notify_event_t> base;

    template<typename C>
    notify(C && c,
           uint8_t first_event,
           const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~notify() = default;

    static uint8_t opcode() {
      return XCB_X_PRINT_NOTIFY;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::x_print::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_X_PRINT_NOTIFY");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_x_print_pcontext_t, typename ... Parameter>
    ReturnType context(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->context),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->context,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class attribut_notify : public xpp::generic::event<xcb_x_print_attribut_notify_event_t> {
  public:
    typedef xpp::x_print::extension extension;

    typedef xpp::generic::event<xcb_x_print_attribut_notify_event_t> base;

    template<typename C>
    attribut_notify(C && c,
                    uint8_t first_event,
                    const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~attribut_notify() = default;

    static uint8_t opcode() {
      return XCB_X_PRINT_ATTRIBUT_NOTIFY;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::x_print::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_X_PRINT_ATTRIBUT_NOTIFY");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_x_print_pcontext_t, typename ... Parameter>
    ReturnType context(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->context),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->context,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}



namespace error {
class bad_context : public xpp::generic::error<bad_context, xcb_x_print_bad_context_error_t> {
  public:
    using xpp::generic::error<bad_context, xcb_x_print_bad_context_error_t>::error;

    virtual ~bad_context() = default;

    static uint8_t opcode() {
      return XCB_X_PRINT_BAD_CONTEXT;
    }
    
    static uint8_t opcode(uint8_t first_error) {
      return first_error + opcode();
    }
    
    static uint8_t opcode(const xpp::x_print::extension & extension) {
      return opcode(extension->first_error);
    }

    static std::string description() {
      return std::string("XCB_X_PRINT_BAD_CONTEXT");
    }

  protected:
    uint8_t m_first_error;
};
} 


namespace error {
class bad_sequence : public xpp::generic::error<bad_sequence, xcb_x_print_bad_sequence_error_t> {
  public:
    using xpp::generic::error<bad_sequence, xcb_x_print_bad_sequence_error_t>::error;

    virtual ~bad_sequence() = default;

    static uint8_t opcode() {
      return XCB_X_PRINT_BAD_SEQUENCE;
    }
    
    static uint8_t opcode(uint8_t first_error) {
      return first_error + opcode();
    }
    
    static uint8_t opcode(const xpp::x_print::extension & extension) {
      return opcode(extension->first_error);
    }

    static std::string description() {
      return std::string("XCB_X_PRINT_BAD_SEQUENCE");
    }

  protected:
    uint8_t m_first_error;
};
} 


namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class print_query_version : public xpp::generic::reply<print_query_version<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_x_print_print_query_version_reply), xcb_x_print_print_query_version_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<print_query_version<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_x_print_print_query_version_reply), xcb_x_print_print_query_version_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    print_query_version(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using print_query_version = detail::print_query_version<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_x_print_print_query_version), xcb_x_print_print_query_version>>;
}

namespace unchecked {
template<typename Connection>
using print_query_version = detail::print_query_version<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_x_print_print_query_version_unchecked), xcb_x_print_print_query_version_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::print_query_version<Connection> print_query_version(Connection && c, Parameter && ... parameter) {
  return reply::checked::print_query_version<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::print_query_version<Connection> print_query_version_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::print_query_version<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class print_get_printer_list : public xpp::generic::reply<print_get_printer_list<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_x_print_print_get_printer_list_reply), xcb_x_print_print_get_printer_list_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<print_get_printer_list<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_x_print_print_get_printer_list_reply), xcb_x_print_print_get_printer_list_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    print_get_printer_list(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_x_print_print_get_printer_list_reply_t, xpp::generic::iterator<Connection,
                                              xcb_x_print_printer_t,
                                              xpp::generic::signature<decltype(xcb_x_print_printer_next), xcb_x_print_printer_next>,
                                              xpp::generic::signature<decltype(xcb_x_print_printer_sizeof), xcb_x_print_printer_sizeof>,
                                              xpp::generic::signature<decltype(xcb_x_print_print_get_printer_list_printers_iterator), xcb_x_print_print_get_printer_list_printers_iterator>>>
    printers() {
      return xpp::generic::list<Connection,
                                xcb_x_print_print_get_printer_list_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_x_print_printer_t,
                                                       xpp::generic::signature<decltype(xcb_x_print_printer_next), xcb_x_print_printer_next>,
                                                       xpp::generic::signature<decltype(xcb_x_print_printer_sizeof), xcb_x_print_printer_sizeof>,
                                                       xpp::generic::signature<decltype(xcb_x_print_print_get_printer_list_printers_iterator), xcb_x_print_print_get_printer_list_printers_iterator>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using print_get_printer_list = detail::print_get_printer_list<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_x_print_print_get_printer_list), xcb_x_print_print_get_printer_list>>;
}

namespace unchecked {
template<typename Connection>
using print_get_printer_list = detail::print_get_printer_list<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_x_print_print_get_printer_list_unchecked), xcb_x_print_print_get_printer_list_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::print_get_printer_list<Connection> print_get_printer_list(Connection && c, Parameter && ... parameter) {
  return reply::checked::print_get_printer_list<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::print_get_printer_list<Connection> print_get_printer_list_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::print_get_printer_list<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void print_rehash_printer_list_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x_print::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_x_print_print_rehash_printer_list_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void print_rehash_printer_list(Parameter && ... parameter) {
  xcb_x_print_print_rehash_printer_list(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void create_context_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x_print::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_x_print_create_context_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void create_context(Parameter && ... parameter) {
  xcb_x_print_create_context(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void print_set_context_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x_print::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_x_print_print_set_context_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void print_set_context(Parameter && ... parameter) {
  xcb_x_print_print_set_context(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class print_get_context : public xpp::generic::reply<print_get_context<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_x_print_print_get_context_reply), xcb_x_print_print_get_context_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<print_get_context<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_x_print_print_get_context_reply), xcb_x_print_print_get_context_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    print_get_context(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using print_get_context = detail::print_get_context<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_x_print_print_get_context), xcb_x_print_print_get_context>>;
}

namespace unchecked {
template<typename Connection>
using print_get_context = detail::print_get_context<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_x_print_print_get_context_unchecked), xcb_x_print_print_get_context_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::print_get_context<Connection> print_get_context(Connection && c, Parameter && ... parameter) {
  return reply::checked::print_get_context<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::print_get_context<Connection> print_get_context_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::print_get_context<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void print_destroy_context_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x_print::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_x_print_print_destroy_context_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void print_destroy_context(Parameter && ... parameter) {
  xcb_x_print_print_destroy_context(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class print_get_screen_of_context : public xpp::generic::reply<print_get_screen_of_context<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_x_print_print_get_screen_of_context_reply), xcb_x_print_print_get_screen_of_context_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<print_get_screen_of_context<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_x_print_print_get_screen_of_context_reply), xcb_x_print_print_get_screen_of_context_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    print_get_screen_of_context(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


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
};

}

namespace checked {
template<typename Connection>
using print_get_screen_of_context = detail::print_get_screen_of_context<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_x_print_print_get_screen_of_context), xcb_x_print_print_get_screen_of_context>>;
}

namespace unchecked {
template<typename Connection>
using print_get_screen_of_context = detail::print_get_screen_of_context<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_x_print_print_get_screen_of_context_unchecked), xcb_x_print_print_get_screen_of_context_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::print_get_screen_of_context<Connection> print_get_screen_of_context(Connection && c, Parameter && ... parameter) {
  return reply::checked::print_get_screen_of_context<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::print_get_screen_of_context<Connection> print_get_screen_of_context_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::print_get_screen_of_context<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void print_start_job_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x_print::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_x_print_print_start_job_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void print_start_job(Parameter && ... parameter) {
  xcb_x_print_print_start_job(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void print_end_job_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x_print::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_x_print_print_end_job_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void print_end_job(Parameter && ... parameter) {
  xcb_x_print_print_end_job(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void print_start_doc_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x_print::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_x_print_print_start_doc_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void print_start_doc(Parameter && ... parameter) {
  xcb_x_print_print_start_doc(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void print_end_doc_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x_print::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_x_print_print_end_doc_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void print_end_doc(Parameter && ... parameter) {
  xcb_x_print_print_end_doc(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void print_put_document_data_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x_print::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_x_print_print_put_document_data_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void print_put_document_data(Parameter && ... parameter) {
  xcb_x_print_print_put_document_data(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class print_get_document_data : public xpp::generic::reply<print_get_document_data<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_x_print_print_get_document_data_reply), xcb_x_print_print_get_document_data_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<print_get_document_data<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_x_print_print_get_document_data_reply), xcb_x_print_print_get_document_data_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    print_get_document_data(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_x_print_print_get_document_data_reply_t, xpp::generic::iterator<Connection,
                                              uint8_t,
                                              xpp::generic::signature<decltype(xcb_x_print_print_get_document_data_data), xcb_x_print_print_get_document_data_data>,
                                              xpp::generic::signature<decltype(xcb_x_print_print_get_document_data_data_length), xcb_x_print_print_get_document_data_data_length>>>
    data() {
      return xpp::generic::list<Connection,
                                xcb_x_print_print_get_document_data_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint8_t,
                                                       xpp::generic::signature<decltype(xcb_x_print_print_get_document_data_data), xcb_x_print_print_get_document_data_data>,
                                                       xpp::generic::signature<decltype(xcb_x_print_print_get_document_data_data_length), xcb_x_print_print_get_document_data_data_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using print_get_document_data = detail::print_get_document_data<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_x_print_print_get_document_data), xcb_x_print_print_get_document_data>>;
}

namespace unchecked {
template<typename Connection>
using print_get_document_data = detail::print_get_document_data<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_x_print_print_get_document_data_unchecked), xcb_x_print_print_get_document_data_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::print_get_document_data<Connection> print_get_document_data(Connection && c, Parameter && ... parameter) {
  return reply::checked::print_get_document_data<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::print_get_document_data<Connection> print_get_document_data_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::print_get_document_data<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void print_start_page_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x_print::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_x_print_print_start_page_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void print_start_page(Parameter && ... parameter) {
  xcb_x_print_print_start_page(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void print_end_page_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x_print::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_x_print_print_end_page_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void print_end_page(Parameter && ... parameter) {
  xcb_x_print_print_end_page(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void print_select_input_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x_print::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_x_print_print_select_input_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void print_select_input(Parameter && ... parameter) {
  xcb_x_print_print_select_input(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class print_input_selected : public xpp::generic::reply<print_input_selected<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_x_print_print_input_selected_reply), xcb_x_print_print_input_selected_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<print_input_selected<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_x_print_print_input_selected_reply), xcb_x_print_print_input_selected_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    print_input_selected(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using print_input_selected = detail::print_input_selected<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_x_print_print_input_selected), xcb_x_print_print_input_selected>>;
}

namespace unchecked {
template<typename Connection>
using print_input_selected = detail::print_input_selected<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_x_print_print_input_selected_unchecked), xcb_x_print_print_input_selected_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::print_input_selected<Connection> print_input_selected(Connection && c, Parameter && ... parameter) {
  return reply::checked::print_input_selected<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::print_input_selected<Connection> print_input_selected_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::print_input_selected<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class print_get_attributes : public xpp::generic::reply<print_get_attributes<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_x_print_print_get_attributes_reply), xcb_x_print_print_get_attributes_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<print_get_attributes<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_x_print_print_get_attributes_reply), xcb_x_print_print_get_attributes_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    print_get_attributes(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_x_print_print_get_attributes_reply_t, xpp::generic::iterator<Connection,
                                              xcb_x_print_string8_t,
                                              xpp::generic::signature<decltype(xcb_x_print_print_get_attributes_attributes), xcb_x_print_print_get_attributes_attributes>,
                                              xpp::generic::signature<decltype(xcb_x_print_print_get_attributes_attributes_length), xcb_x_print_print_get_attributes_attributes_length>>>
    attributes() {
      return xpp::generic::list<Connection,
                                xcb_x_print_print_get_attributes_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_x_print_string8_t,
                                                       xpp::generic::signature<decltype(xcb_x_print_print_get_attributes_attributes), xcb_x_print_print_get_attributes_attributes>,
                                                       xpp::generic::signature<decltype(xcb_x_print_print_get_attributes_attributes_length), xcb_x_print_print_get_attributes_attributes_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using print_get_attributes = detail::print_get_attributes<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_x_print_print_get_attributes), xcb_x_print_print_get_attributes>>;
}

namespace unchecked {
template<typename Connection>
using print_get_attributes = detail::print_get_attributes<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_x_print_print_get_attributes_unchecked), xcb_x_print_print_get_attributes_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::print_get_attributes<Connection> print_get_attributes(Connection && c, Parameter && ... parameter) {
  return reply::checked::print_get_attributes<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::print_get_attributes<Connection> print_get_attributes_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::print_get_attributes<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class print_get_one_attributes : public xpp::generic::reply<print_get_one_attributes<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_x_print_print_get_one_attributes_reply), xcb_x_print_print_get_one_attributes_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<print_get_one_attributes<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_x_print_print_get_one_attributes_reply), xcb_x_print_print_get_one_attributes_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    print_get_one_attributes(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_x_print_print_get_one_attributes_reply_t, xpp::generic::iterator<Connection,
                                              xcb_x_print_string8_t,
                                              xpp::generic::signature<decltype(xcb_x_print_print_get_one_attributes_value), xcb_x_print_print_get_one_attributes_value>,
                                              xpp::generic::signature<decltype(xcb_x_print_print_get_one_attributes_value_length), xcb_x_print_print_get_one_attributes_value_length>>>
    value() {
      return xpp::generic::list<Connection,
                                xcb_x_print_print_get_one_attributes_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_x_print_string8_t,
                                                       xpp::generic::signature<decltype(xcb_x_print_print_get_one_attributes_value), xcb_x_print_print_get_one_attributes_value>,
                                                       xpp::generic::signature<decltype(xcb_x_print_print_get_one_attributes_value_length), xcb_x_print_print_get_one_attributes_value_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using print_get_one_attributes = detail::print_get_one_attributes<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_x_print_print_get_one_attributes), xcb_x_print_print_get_one_attributes>>;
}

namespace unchecked {
template<typename Connection>
using print_get_one_attributes = detail::print_get_one_attributes<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_x_print_print_get_one_attributes_unchecked), xcb_x_print_print_get_one_attributes_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::print_get_one_attributes<Connection> print_get_one_attributes(Connection && c, Parameter && ... parameter) {
  return reply::checked::print_get_one_attributes<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::print_get_one_attributes<Connection> print_get_one_attributes_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::print_get_one_attributes<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void print_set_attributes_checked(Connection && c, xcb_x_print_pcontext_t context, uint32_t stringLen, uint8_t pool, uint8_t rule, uint32_t attributes_len, const xcb_x_print_string8_t * attributes) {
  xpp::generic::check<Connection, xpp::x_print::error::dispatcher>(std::forward<Connection>(c), xcb_x_print_print_set_attributes_checked(std::forward<Connection>(c), context, stringLen, pool, rule, attributes_len, attributes));
}

template<typename Connection>
void print_set_attributes(Connection && c, xcb_x_print_pcontext_t context, uint32_t stringLen, uint8_t pool, uint8_t rule, uint32_t attributes_len, const xcb_x_print_string8_t * attributes) {
  xcb_x_print_print_set_attributes(std::forward<Connection>(c), context, stringLen, pool, rule, attributes_len, attributes);
}

template<typename Connection, typename Attributes_Iterator>
void print_set_attributes_checked(Connection && c, xcb_x_print_pcontext_t context, uint32_t stringLen, uint8_t pool, uint8_t rule, Attributes_Iterator attributes_begin, Attributes_Iterator attributes_end) {
      typedef typename value_type<Attributes_Iterator, ! std::is_pointer<Attributes_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> attributes =
        { value_iterator<Attributes_Iterator>(attributes_begin), value_iterator<Attributes_Iterator>(attributes_end) };
      
  xpp::generic::check<Connection, xpp::x_print::error::dispatcher>(std::forward<Connection>(c), xcb_x_print_print_set_attributes_checked(std::forward<Connection>(c), context, stringLen, pool, rule, static_cast<uint32_t>(attributes.size()), const_cast<const vector_type *>(attributes.data())));
}

template<typename Connection, typename Attributes_Iterator>
void print_set_attributes(Connection && c, xcb_x_print_pcontext_t context, uint32_t stringLen, uint8_t pool, uint8_t rule, Attributes_Iterator attributes_begin, Attributes_Iterator attributes_end) {
      typedef typename value_type<Attributes_Iterator, ! std::is_pointer<Attributes_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> attributes =
        { value_iterator<Attributes_Iterator>(attributes_begin), value_iterator<Attributes_Iterator>(attributes_end) };
      
  xcb_x_print_print_set_attributes(std::forward<Connection>(c), context, stringLen, pool, rule, static_cast<uint32_t>(attributes.size()), const_cast<const vector_type *>(attributes.data()));
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class print_get_page_dimensions : public xpp::generic::reply<print_get_page_dimensions<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_x_print_print_get_page_dimensions_reply), xcb_x_print_print_get_page_dimensions_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<print_get_page_dimensions<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_x_print_print_get_page_dimensions_reply), xcb_x_print_print_get_page_dimensions_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    print_get_page_dimensions(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using print_get_page_dimensions = detail::print_get_page_dimensions<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_x_print_print_get_page_dimensions), xcb_x_print_print_get_page_dimensions>>;
}

namespace unchecked {
template<typename Connection>
using print_get_page_dimensions = detail::print_get_page_dimensions<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_x_print_print_get_page_dimensions_unchecked), xcb_x_print_print_get_page_dimensions_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::print_get_page_dimensions<Connection> print_get_page_dimensions(Connection && c, Parameter && ... parameter) {
  return reply::checked::print_get_page_dimensions<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::print_get_page_dimensions<Connection> print_get_page_dimensions_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::print_get_page_dimensions<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class print_query_screens : public xpp::generic::reply<print_query_screens<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_x_print_print_query_screens_reply), xcb_x_print_print_query_screens_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<print_query_screens<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_x_print_print_query_screens_reply), xcb_x_print_print_query_screens_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    print_query_screens(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename Roots = xcb_window_t>
    xpp::generic::list<Connection, xcb_x_print_print_query_screens_reply_t, xpp::generic::iterator<Connection,
                                              Roots,
                                              xpp::generic::signature<decltype(xcb_x_print_print_query_screens_roots), xcb_x_print_print_query_screens_roots>,
                                              xpp::generic::signature<decltype(xcb_x_print_print_query_screens_roots_length), xcb_x_print_print_query_screens_roots_length>>>
    roots() {
      return xpp::generic::list<Connection,
                                xcb_x_print_print_query_screens_reply_t,
                                xpp::generic::iterator<Connection,
                                                       Roots,
                                                       xpp::generic::signature<decltype(xcb_x_print_print_query_screens_roots), xcb_x_print_print_query_screens_roots>,
                                                       xpp::generic::signature<decltype(xcb_x_print_print_query_screens_roots_length), xcb_x_print_print_query_screens_roots_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using print_query_screens = detail::print_query_screens<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_x_print_print_query_screens), xcb_x_print_print_query_screens>>;
}

namespace unchecked {
template<typename Connection>
using print_query_screens = detail::print_query_screens<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_x_print_print_query_screens_unchecked), xcb_x_print_print_query_screens_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::print_query_screens<Connection> print_query_screens(Connection && c, Parameter && ... parameter) {
  return reply::checked::print_query_screens<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::print_query_screens<Connection> print_query_screens_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::print_query_screens<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class print_set_image_resolution : public xpp::generic::reply<print_set_image_resolution<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_x_print_print_set_image_resolution_reply), xcb_x_print_print_set_image_resolution_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<print_set_image_resolution<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_x_print_print_set_image_resolution_reply), xcb_x_print_print_set_image_resolution_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    print_set_image_resolution(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using print_set_image_resolution = detail::print_set_image_resolution<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_x_print_print_set_image_resolution), xcb_x_print_print_set_image_resolution>>;
}

namespace unchecked {
template<typename Connection>
using print_set_image_resolution = detail::print_set_image_resolution<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_x_print_print_set_image_resolution_unchecked), xcb_x_print_print_set_image_resolution_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::print_set_image_resolution<Connection> print_set_image_resolution(Connection && c, Parameter && ... parameter) {
  return reply::checked::print_set_image_resolution<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::print_set_image_resolution<Connection> print_set_image_resolution_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::print_set_image_resolution<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class print_get_image_resolution : public xpp::generic::reply<print_get_image_resolution<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_x_print_print_get_image_resolution_reply), xcb_x_print_print_get_image_resolution_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<print_get_image_resolution<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_x_print_print_get_image_resolution_reply), xcb_x_print_print_get_image_resolution_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    print_get_image_resolution(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using print_get_image_resolution = detail::print_get_image_resolution<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_x_print_print_get_image_resolution), xcb_x_print_print_get_image_resolution>>;
}

namespace unchecked {
template<typename Connection>
using print_get_image_resolution = detail::print_get_image_resolution<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_x_print_print_get_image_resolution_unchecked), xcb_x_print_print_get_image_resolution_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::print_get_image_resolution<Connection> print_get_image_resolution(Connection && c, Parameter && ... parameter) {
  return reply::checked::print_get_image_resolution<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::print_get_image_resolution<Connection> print_get_image_resolution_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::print_get_image_resolution<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}






















template<typename Derived, typename Connection>
class pcontext
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_x_print_pcontext_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~pcontext() = default;

    template<typename ... Parameter>
    auto print_get_document_data(Parameter && ... parameter) const -> reply::checked::print_get_document_data<Connection> {
      return xpp::x_print::print_get_document_data(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto print_get_document_data_unchecked(Parameter && ... parameter) const -> reply::unchecked::print_get_document_data<Connection> {
      return xpp::x_print::print_get_document_data_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void print_select_input_checked(Parameter && ... parameter) const {
      xpp::x_print::print_select_input_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void print_select_input(Parameter && ... parameter) const {
      xpp::x_print::print_select_input(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto print_input_selected(Parameter && ... parameter) const -> reply::checked::print_input_selected<Connection> {
      return xpp::x_print::print_input_selected(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto print_input_selected_unchecked(Parameter && ... parameter) const -> reply::unchecked::print_input_selected<Connection> {
      return xpp::x_print::print_input_selected_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto print_get_attributes(Parameter && ... parameter) const -> reply::checked::print_get_attributes<Connection> {
      return xpp::x_print::print_get_attributes(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto print_get_attributes_unchecked(Parameter && ... parameter) const -> reply::unchecked::print_get_attributes<Connection> {
      return xpp::x_print::print_get_attributes_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto print_get_one_attributes(Parameter && ... parameter) const -> reply::checked::print_get_one_attributes<Connection> {
      return xpp::x_print::print_get_one_attributes(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto print_get_one_attributes_unchecked(Parameter && ... parameter) const -> reply::unchecked::print_get_one_attributes<Connection> {
      return xpp::x_print::print_get_one_attributes_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void print_set_attributes_checked(Parameter && ... parameter) const {
      xpp::x_print::print_set_attributes_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void print_set_attributes(Parameter && ... parameter) const {
      xpp::x_print::print_set_attributes(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto print_get_page_dimensions(Parameter && ... parameter) const -> reply::checked::print_get_page_dimensions<Connection> {
      return xpp::x_print::print_get_page_dimensions(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto print_get_page_dimensions_unchecked(Parameter && ... parameter) const -> reply::unchecked::print_get_page_dimensions<Connection> {
      return xpp::x_print::print_get_page_dimensions_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto print_set_image_resolution(Parameter && ... parameter) const -> reply::checked::print_set_image_resolution<Connection> {
      return xpp::x_print::print_set_image_resolution(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto print_set_image_resolution_unchecked(Parameter && ... parameter) const -> reply::unchecked::print_set_image_resolution<Connection> {
      return xpp::x_print::print_set_image_resolution_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto print_get_image_resolution(Parameter && ... parameter) const -> reply::checked::print_get_image_resolution<Connection> {
      return xpp::x_print::print_get_image_resolution(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto print_get_image_resolution_unchecked(Parameter && ... parameter) const -> reply::unchecked::print_get_image_resolution<Connection> {
      return xpp::x_print::print_get_image_resolution_unchecked(
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
    typedef xpp::x_print::extension extension;


    virtual ~interface() = default;

    const interface<Derived, Connection> &
    x_print() {
      return *this;
    }

    template<typename ... Parameter>
    auto print_query_version(Parameter && ... parameter) const -> reply::checked::print_query_version<Connection> {
      return xpp::x_print::print_query_version(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto print_query_version_unchecked(Parameter && ... parameter) const -> reply::unchecked::print_query_version<Connection> {
      return xpp::x_print::print_query_version_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto print_get_printer_list(Parameter && ... parameter) const -> reply::checked::print_get_printer_list<Connection> {
      return xpp::x_print::print_get_printer_list(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto print_get_printer_list_unchecked(Parameter && ... parameter) const -> reply::unchecked::print_get_printer_list<Connection> {
      return xpp::x_print::print_get_printer_list_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void print_rehash_printer_list_checked(Parameter && ... parameter) const {
      xpp::x_print::print_rehash_printer_list_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void print_rehash_printer_list(Parameter && ... parameter) const {
      xpp::x_print::print_rehash_printer_list(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_context_checked(Parameter && ... parameter) const {
      xpp::x_print::create_context_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_context(Parameter && ... parameter) const {
      xpp::x_print::create_context(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void print_set_context_checked(Parameter && ... parameter) const {
      xpp::x_print::print_set_context_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void print_set_context(Parameter && ... parameter) const {
      xpp::x_print::print_set_context(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto print_get_context(Parameter && ... parameter) const -> reply::checked::print_get_context<Connection> {
      return xpp::x_print::print_get_context(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto print_get_context_unchecked(Parameter && ... parameter) const -> reply::unchecked::print_get_context<Connection> {
      return xpp::x_print::print_get_context_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void print_destroy_context_checked(Parameter && ... parameter) const {
      xpp::x_print::print_destroy_context_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void print_destroy_context(Parameter && ... parameter) const {
      xpp::x_print::print_destroy_context(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto print_get_screen_of_context(Parameter && ... parameter) const -> reply::checked::print_get_screen_of_context<Connection> {
      return xpp::x_print::print_get_screen_of_context(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto print_get_screen_of_context_unchecked(Parameter && ... parameter) const -> reply::unchecked::print_get_screen_of_context<Connection> {
      return xpp::x_print::print_get_screen_of_context_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void print_start_job_checked(Parameter && ... parameter) const {
      xpp::x_print::print_start_job_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void print_start_job(Parameter && ... parameter) const {
      xpp::x_print::print_start_job(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void print_end_job_checked(Parameter && ... parameter) const {
      xpp::x_print::print_end_job_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void print_end_job(Parameter && ... parameter) const {
      xpp::x_print::print_end_job(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void print_start_doc_checked(Parameter && ... parameter) const {
      xpp::x_print::print_start_doc_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void print_start_doc(Parameter && ... parameter) const {
      xpp::x_print::print_start_doc(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void print_end_doc_checked(Parameter && ... parameter) const {
      xpp::x_print::print_end_doc_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void print_end_doc(Parameter && ... parameter) const {
      xpp::x_print::print_end_doc(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void print_put_document_data_checked(Parameter && ... parameter) const {
      xpp::x_print::print_put_document_data_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void print_put_document_data(Parameter && ... parameter) const {
      xpp::x_print::print_put_document_data(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto print_get_document_data(Parameter && ... parameter) const -> reply::checked::print_get_document_data<Connection> {
      return xpp::x_print::print_get_document_data(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto print_get_document_data_unchecked(Parameter && ... parameter) const -> reply::unchecked::print_get_document_data<Connection> {
      return xpp::x_print::print_get_document_data_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void print_start_page_checked(Parameter && ... parameter) const {
      xpp::x_print::print_start_page_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void print_start_page(Parameter && ... parameter) const {
      xpp::x_print::print_start_page(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void print_end_page_checked(Parameter && ... parameter) const {
      xpp::x_print::print_end_page_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void print_end_page(Parameter && ... parameter) const {
      xpp::x_print::print_end_page(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void print_select_input_checked(Parameter && ... parameter) const {
      xpp::x_print::print_select_input_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void print_select_input(Parameter && ... parameter) const {
      xpp::x_print::print_select_input(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto print_input_selected(Parameter && ... parameter) const -> reply::checked::print_input_selected<Connection> {
      return xpp::x_print::print_input_selected(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto print_input_selected_unchecked(Parameter && ... parameter) const -> reply::unchecked::print_input_selected<Connection> {
      return xpp::x_print::print_input_selected_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto print_get_attributes(Parameter && ... parameter) const -> reply::checked::print_get_attributes<Connection> {
      return xpp::x_print::print_get_attributes(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto print_get_attributes_unchecked(Parameter && ... parameter) const -> reply::unchecked::print_get_attributes<Connection> {
      return xpp::x_print::print_get_attributes_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto print_get_one_attributes(Parameter && ... parameter) const -> reply::checked::print_get_one_attributes<Connection> {
      return xpp::x_print::print_get_one_attributes(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto print_get_one_attributes_unchecked(Parameter && ... parameter) const -> reply::unchecked::print_get_one_attributes<Connection> {
      return xpp::x_print::print_get_one_attributes_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void print_set_attributes_checked(Parameter && ... parameter) const {
      xpp::x_print::print_set_attributes_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void print_set_attributes(Parameter && ... parameter) const {
      xpp::x_print::print_set_attributes(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto print_get_page_dimensions(Parameter && ... parameter) const -> reply::checked::print_get_page_dimensions<Connection> {
      return xpp::x_print::print_get_page_dimensions(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto print_get_page_dimensions_unchecked(Parameter && ... parameter) const -> reply::unchecked::print_get_page_dimensions<Connection> {
      return xpp::x_print::print_get_page_dimensions_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto print_query_screens(Parameter && ... parameter) const -> reply::checked::print_query_screens<Connection> {
      return xpp::x_print::print_query_screens(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto print_query_screens_unchecked(Parameter && ... parameter) const -> reply::unchecked::print_query_screens<Connection> {
      return xpp::x_print::print_query_screens_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto print_set_image_resolution(Parameter && ... parameter) const -> reply::checked::print_set_image_resolution<Connection> {
      return xpp::x_print::print_set_image_resolution(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto print_set_image_resolution_unchecked(Parameter && ... parameter) const -> reply::unchecked::print_set_image_resolution<Connection> {
      return xpp::x_print::print_set_image_resolution_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto print_get_image_resolution(Parameter && ... parameter) const -> reply::checked::print_get_image_resolution<Connection> {
      return xpp::x_print::print_get_image_resolution(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto print_get_image_resolution_unchecked(Parameter && ... parameter) const -> reply::unchecked::print_get_image_resolution<Connection> {
      return xpp::x_print::print_get_image_resolution_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


};

namespace event {

template<typename Connection>
class dispatcher {
  public:
    typedef xpp::x_print::extension extension;

    template<typename C>
    dispatcher(C && c, uint8_t first_event) : m_c(std::forward<C>(c)) , m_first_event(first_event) {}

    template<typename C>
    dispatcher(C && c, const xpp::x_print::extension & extension) : dispatcher(std::forward<C>(c), extension->first_event) {}

    template<typename Handler>
    bool operator()(Handler handler, const std::shared_ptr<xcb_generic_event_t> & event) const {
      switch ((event->response_type & ~0x80) - m_first_event) {

        case XCB_X_PRINT_NOTIFY:
          handler(xpp::x_print::event::notify<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_X_PRINT_ATTRIBUT_NOTIFY:
          handler(xpp::x_print::event::attribut_notify<Connection>(m_c, m_first_event, event));
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
    typedef xpp::x_print::extension extension;

    dispatcher(uint8_t first_error) : m_first_error(first_error) {}

    dispatcher(const xpp::x_print::extension & extension) : dispatcher(extension->first_error) {}

    void operator()(const std::shared_ptr<xcb_generic_error_t> & error) const {
      switch (error->error_code - m_first_error) {

        case XCB_X_PRINT_BAD_CONTEXT: // 0
          throw xpp::x_print::error::bad_context(error);

        case XCB_X_PRINT_BAD_SEQUENCE: // 1
          throw xpp::x_print::error::bad_sequence(error);

      };
    }

  protected:
    uint8_t m_first_error;
};

}


};

#endif // XPP_X_PRINT_HPP
