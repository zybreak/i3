#ifndef XPP_GENERIC_REQUEST_HPP
#define XPP_GENERIC_REQUEST_HPP

#include <array>
#include <memory>
#include <cstdlib>
#include <xcb/xcb.h>
#include "error.hpp"
#include "signature.hpp"

namespace xpp::generic {

    template<typename Connection, typename Dispatcher>
    void check(Connection &&c, const xcb_void_cookie_t &cookie) {
        xcb_generic_error_t *error = xcb_request_check(std::forward<Connection>(c), cookie);
        if (error) {
            dispatch(std::forward<Connection>(c), std::shared_ptr<xcb_generic_error_t>(error, std::free));
        }
    }

    struct checked_tag {
    };
    struct unchecked_tag {
    };

    template<typename ... Types>
    class reply;

    template<typename Derived, typename Connection, typename Check, typename Reply, typename Cookie, Reply *(&ReplyFunction)(xcb_connection_t *, Cookie, xcb_generic_error_t **), typename...CookieParameter, Cookie(&CookieFunction)(CookieParameter...)>
    class reply<Derived, Connection, Check, xpp::generic::signature<Reply *(xcb_connection_t *, Cookie, xcb_generic_error_t **), ReplyFunction>, xpp::generic::signature<Cookie(CookieParameter...), CookieFunction> > {
    public:
        template<typename C, typename ... Parameter>
        reply(C &&c, Parameter &&... parameter) : m_c(std::forward<C>(c)), m_cookie(Derived::cookie(std::forward<C>(c), std::forward<Parameter>(parameter) ...)) {
        }

        operator bool() {
            return m_reply.operator bool();
        }

        const Reply &operator*() {
            return *get();
        }

        Reply *operator->() {
            return get().get();
        }

        const std::shared_ptr<Reply> &get() {
            if (!m_reply) {
                m_reply = get(Check());
            }
            return m_reply;
        }

        template<typename ... Parameter>
        static Cookie cookie(Parameter &&... parameter) {
            return CookieFunction(std::forward<Parameter>(parameter) ...);
        }

    protected:
        Connection m_c;
        Cookie m_cookie;
        std::shared_ptr<Reply> m_reply;

        std::shared_ptr<Reply> get(checked_tag) {
            xcb_generic_error_t *error = nullptr;
            auto reply = std::shared_ptr<Reply>(ReplyFunction(m_c, m_cookie, &error), std::free);
            if (error) {
                dispatch(m_c, std::shared_ptr<xcb_generic_error_t>(error, std::free));
            }
            return reply;
        }

        std::shared_ptr<Reply> get(unchecked_tag) {
            return std::shared_ptr<Reply>(ReplyFunction(m_c, m_cookie, nullptr), std::free);
        }
    };

}

#endif // XPP_GENERIC_REQUEST_HPP
