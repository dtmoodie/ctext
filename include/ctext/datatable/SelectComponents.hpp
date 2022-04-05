#ifndef CTEXT_SELECT_COMPONENTS_HPP
#define CTEXT_SELECT_COMPONENTS_HPP
#include <ct/VariadicTypedef.hpp>
#include <ct/type_traits.hpp>
#include <ct/reflect.hpp>

namespace ct
{
    namespace ext
    {
        struct Component
        {
        };

        template <class T>
        struct SelectComponents;

        template <class T>
        struct SelectComponents<VariadicTypedef<T>>
        {
            using type = typename AppendIf<IsBase<Base<Component>, Derived<T>>::value, T, ct::VariadicTypedef<>>::type;
        };

        template <class T, class... U>
        struct SelectComponents<VariadicTypedef<T, U...>>
        {
            using super = SelectComponents<VariadicTypedef<U...>>;
            using type = typename AppendIf<IsBase<Base<Component>, Derived<T>>::value, T, typename super::type>::type;
        };
    }
}

#endif // CTEXT_SELECT_COMPONENTS_HPP
