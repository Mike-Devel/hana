/*!
@file
Defines `boost::hana::Map`.

@copyright Louis Dionne 2014
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_MAP_HPP
#define BOOST_HANA_MAP_HPP

#include <boost/hana/fwd/map.hpp>

#include <boost/hana/comparable.hpp>
#include <boost/hana/core/convert.hpp>
#include <boost/hana/core/datatype.hpp>
#include <boost/hana/core/is_a.hpp>
#include <boost/hana/core/models.hpp>
#include <boost/hana/core/when.hpp>
#include <boost/hana/detail/create.hpp>
#include <boost/hana/detail/std/forward.hpp>
#include <boost/hana/detail/std/move.hpp>
#include <boost/hana/foldable.hpp>
#include <boost/hana/functional/compose.hpp>
#include <boost/hana/functional/demux.hpp>
#include <boost/hana/functional/partial.hpp>
#include <boost/hana/functor.hpp>
#include <boost/hana/list.hpp>
#include <boost/hana/logical.hpp>
#include <boost/hana/product.hpp>
#include <boost/hana/record.hpp>
#include <boost/hana/searchable.hpp>
#include <boost/hana/tuple.hpp>


namespace boost { namespace hana {
    // Remove warnings generated by poor confused Doxygen
    //! @cond

    //////////////////////////////////////////////////////////////////////////
    // map
    //////////////////////////////////////////////////////////////////////////
    template <typename ...Pairs>
    constexpr decltype(auto) _make_map::operator()(Pairs&& ...pairs) const {
        return detail::create<_map>{}(
                        hana::tuple(detail::std::forward<Pairs>(pairs)...));
    }

    //////////////////////////////////////////////////////////////////////////
    // keys
    //////////////////////////////////////////////////////////////////////////
    template <typename Map>
    constexpr decltype(auto) _keys::operator()(Map&& map) const {
        return hana::transform(detail::std::forward<Map>(map).storage, first);
    }

    //////////////////////////////////////////////////////////////////////////
    // values
    //////////////////////////////////////////////////////////////////////////
    template <typename Map>
    constexpr decltype(auto) _values::operator()(Map&& map) const {
        return hana::transform(detail::std::forward<Map>(map).storage, second);
    }

    //! @endcond

    //////////////////////////////////////////////////////////////////////////
    // Operators
    //////////////////////////////////////////////////////////////////////////
    template <>
    struct enabled_operators<Map>
        : Comparable
    { };

    //////////////////////////////////////////////////////////////////////////
    // Comparable
    //////////////////////////////////////////////////////////////////////////
    template <>
    struct models<Comparable(Map)>
        : detail::std::true_type
    { };

    template <>
    struct equal_impl<Map, Map> {
        template <typename M1, typename M2>
        static constexpr auto apply(M1 const& m1, M2 const& m2) {
            return hana::and_(
                hana::equal(hana::length(m1.storage), hana::length(m2.storage)),
                hana::all(hana::keys(m1), hana::demux(equal)(
                    hana::partial(lookup, m1),
                    hana::partial(lookup, m2)
                ))
            );
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // Searchable
    //////////////////////////////////////////////////////////////////////////
    template <>
    struct models<Searchable(Map)>
        : detail::std::true_type
    { };

    template <>
    struct find_impl<Map> {
        template <typename M, typename Pred>
        static constexpr auto apply(M&& map, Pred&& pred) {
            return hana::transform(
                hana::find(detail::std::forward<M>(map).storage,
                    hana::compose(detail::std::forward<Pred>(pred), first)),
                second
            );
        }
    };

    template <>
    struct any_impl<Map> {
        template <typename M, typename Pred>
        static constexpr auto apply(M map, Pred pred)
        { return hana::any(hana::keys(map), pred); }
    };

    //////////////////////////////////////////////////////////////////////////
    // Conversions
    //////////////////////////////////////////////////////////////////////////
    template <typename R>
    struct to_impl<Map, R, when<is_a<Record, R>()>> {
        template <typename X>
        static constexpr decltype(auto) apply(X&& x) {
            auto extract = [x(detail::std::forward<X>(x))](auto&& member) -> decltype(auto) {
                using P = typename datatype<decltype(member)>::type;
                return make<P>(
                    hana::first(detail::std::forward<decltype(member)>(member)),
                    hana::second(detail::std::forward<decltype(member)>(member))(x)
                );
            };
            return to<Map>(hana::transform(members<R>, detail::std::move(extract)));
        }
    };

    template <typename F>
    struct to_impl<Map, F, when<is_a<Foldable, F>() && !is_a<Record, F>()>> {
        template <typename Xs>
        static constexpr decltype(auto) apply(Xs&& xs)
        { return hana::unpack(detail::std::forward<Xs>(xs), map); }
    };

    template <typename L>
    struct to_impl<L, Map, when<is_a<List, L>()>> {
        template <typename M>
        static constexpr decltype(auto) apply(M&& m)
        { return to<L>(detail::std::forward<M>(m).storage); }
    };
}} // end namespace boost::hana

#endif // !BOOST_HANA_MAP_HPP
