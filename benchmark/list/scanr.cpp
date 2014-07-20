<% render(instance) %>
<%= includes((0..x).size) %>

#include <boost/hana/list/list.hpp>


template <typename ...>
struct result { };

struct f {
    template <typename X, typename State>
    constexpr result<X, State> operator()(X, State) const { return {}; }
};

struct state { };
template <int> struct x { };

int main() {
    auto go = boost::hana::scanr(f{}, state{},
        <%= list(
            (0..x).map { |i| "x<#{i}>" },
            (0..x).map { |i| "x<#{i}>{}" }
        ) %>
    );
}
