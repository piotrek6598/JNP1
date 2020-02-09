#ifndef FIBIN_H
#define FIBIN_H


#include <iostream>
#include <assert.h>

///
/// In any structure (T) type unrollType<VType, Environment> is base type for
/// type T with VType as values' type and Environment as an variables' environment.
/// It means, that for T representing Number unrollType is a NumberWrapper
/// containing valus, for T representing Function is FunctionWrapper.
/// If one parameter is skipped, eg. unrollType<VType> it means that type is same
/// for all environments.
/// Variables are represted by it's id (positive integer).
///


using varId = uint32_t;
using fibIndex = uint64_t;

namespace details {

// Gets base type of type T with Params as T::unrollType template params.
template <typename T, typename ...Params>
using GetTemplateUnrollType = typename T::template unrollType<Params...>;

// Gets base type of type T without any params.
template <typename T>
using GetUnrollType = typename T::unrollType;

// Computes Fib<n> values using ValueType as value's type.
template <fibIndex n, typename ValueType>
struct FibWrapper {
    static_assert (n >= 2);
    static constexpr ValueType value =
            FibWrapper<n - 1, ValueType>::value + FibWrapper<n - 2, ValueType>::value;
};

template <typename ValueType>
struct FibWrapper<0, ValueType> {
    static constexpr ValueType value = 0;
};

template <typename ValueType>
struct FibWrapper<1, ValueType> {
    static constexpr ValueType value = 1;
};

// Provides wrapper for number containing value of type ValueType.
template <typename ValueType, ValueType val>
struct NumberWrapper {
    static constexpr ValueType value = val;
};

// Provides wrapper for bool values.
template <bool value>
struct BoolWrapper {
    static constexpr bool bvalue = value;
};

// If operation with condition as bool type.
template <bool condition, typename Then, typename Else>
struct IfHelper {
    using unrollType = Then;
};

template <typename Then, typename Else>
struct IfHelper<false, Then, Else> {
    using unrollType = Else;
};


// Binds Variable of given id to it's value.
template <varId id, typename Value>
struct Binding {};

// Environment is set of variables and it's value.
template <typename ...Bindings>
struct Env {};

// Empty environment, it's default environment.
using EmptyEnv = Env<>;

// Template for searching variables in Environment.
template <varId id, typename Environment>
struct FindVar{};

// If this binding represent searched variable, provides it's value.
template <varId id, typename Value, typename ...Bindings>
struct FindVar <id, Env<Binding<id, Value>, Bindings...>> {
    using unrollType = Value;
};

// Otherwise search in remaining bindings.
template <varId id, typename FirstBinding, typename ...Bindings>
struct FindVar<id, Env<FirstBinding, Bindings...>> {
    using unrollType = GetUnrollType<FindVar<id, Env<Bindings...>>>;
};

template <class Env1, class Env2>
struct MergeEnv {};

template <class ...Bindings1, class ...Bindings2>
struct MergeEnv<Env<Bindings1...>, Env<Bindings2...>> {
    using unrollType = Env <Bindings1..., Bindings2...>;
};

// Provides wraper for functions.
// Wrapper represents function of one variable (represented by it's id) with
// given body, value type and environment.
// Using apply method computes Body expression with given environment and
// one added binding for used variable.
template <varId id, typename Body, typename ValueType, typename Environment>
struct FunctionWrapper {
    template <typename Arg>
    using apply = GetTemplateUnrollType<Body, ValueType,
    GetUnrollType<MergeEnv<Env<Binding<id, Arg>>, Environment>>>;
};

}

// Fibin bool types.
struct True {
    template <typename ValueType>
    using unrollType = details::BoolWrapper<true>;
};
struct False {
    template <typename ValueType>
    using unrollType = details::BoolWrapper<false>;
};

// Fibin Fib type.
template <fibIndex n>
struct Fib {
    template <typename ValueType>
    using unrollType = details::NumberWrapper<
    ValueType, details::FibWrapper<n,ValueType>::value>;
};

// Fibin literals.
template <typename Arg>
struct Lit {
    template <typename VType, typename Environment>
    using unrollType = details::GetTemplateUnrollType<Arg, VType>;
};

// If operation. Convert Condition type to bool, types Then and Else to base types
// and use IfHelper.
template <typename Condition, typename Then, typename Else>
struct If {
    template <typename VType, typename Environment>
    using unrollType = details::GetTemplateUnrollType<
    details::GetUnrollType<details::IfHelper<
    details::GetTemplateUnrollType<Condition, VType, Environment>::bvalue,
    Then,
    Else
    >>, VType, Environment>;
};

// Eq operation. Conputes given expression to value and compare it's.
// Returns BoolWraper with comparision value.
template <typename Left, typename Right>
struct Eq {
    template <typename VType, typename Environment>
    using unrollType = details::BoolWrapper<
    details::GetTemplateUnrollType<Left, VType, Environment>::value ==
    details::GetTemplateUnrollType<Right, VType, Environment>::value
    >;
};

// Converting varaible name to id. If name is forbidden return 0.
constexpr varId Var(const char *name) {
    varId id = 0;
    for (unsigned i = 0; name[i] != 0; i++){
        if (i >= 6)
            return 0;
        id *= 37;
        if ('0' <= name[i] && name[i] <= '9')
            id += name[i] - '0' + 1;
        else if ('A' <= name[i] && name[i] <= 'Z')
            id += name[i] - 'A' + 11;
        else if ('a' <= name[i] && name[i] <= 'z')
            id += name[i] - 'a' + 11;
        else
            return 0;
    }
    return id;
}

// Return value of given variable's id.
template <varId id>
struct Ref {
    static_assert(id > 0);
    template <typename VType, typename Environment>
    using unrollType = details::GetUnrollType<details::FindVar<id, Environment>>;
};

// Computes expression with new variable.
template <varId id, typename Value, typename Expression>
struct Let {
    static_assert(id > 0);
    template <typename VType, typename Environment>
    using unrollType = details::GetTemplateUnrollType<
    Expression, VType, details::GetUnrollType<
    details::MergeEnv<details::Env<details::Binding<id,
    details::GetTemplateUnrollType<Value, VType, Environment>>>, Environment>>>;
};

// Fibin Lambda type.
template <varId id, typename Body>
struct Lambda {
    static_assert(id > 0);
    template <typename VType, typename Environment>
    using unrollType = details::FunctionWrapper<id, Body, VType, Environment>;
};


// Invokes given function with given argument using apply.
template <typename Fun, typename Arg>
struct Invoke {
    template <typename VType, typename Environment>
    using unrollType =
    typename details::GetTemplateUnrollType<Fun, VType, Environment>::template apply<
    details::GetTemplateUnrollType<Arg, VType, Environment>>;
};

// Makes Fold on parameters of sum, by making two arguments sum recursively.
template <typename First, typename Second, typename ...Args>
struct Sum {
    template <typename VType, typename Environment>
    using unrollType = details::NumberWrapper<VType,
    details::GetTemplateUnrollType<First, VType, Environment>::value +
    Sum<Second, Args...>::template unrollType<VType, Environment>::value>;
};

// Sum with two parameters.
template <typename First, typename Second>
struct Sum<First, Second> {
    template <typename VType, typename Environment>
    using unrollType = details::NumberWrapper<VType,
    details::GetTemplateUnrollType<First,VType,Environment>::value +
    details::GetTemplateUnrollType<Second,VType,Environment>::value>;
};

// Adds to arg Fib<1> by calling sum of those elements.
template <typename Arg>
struct Inc1 {
    template <typename VType, typename Environment>
    using unrollType = details::GetTemplateUnrollType<
    Sum<Arg,Lit<Fib<1>>>, VType, Environment>;
};

// Adds to arg Fib<1> by calling sum of those elements.
template <typename Arg>
struct Inc10 {
    template <typename VType, typename Environment>
    using unrollType = details::GetTemplateUnrollType<
    Sum<Arg, Lit<Fib<10>>>, VType, Environment>;
};


// Class Fibin for getting value from expression.
template <typename ValueType>
class Fibin {
public:
    // Is callable when ValueType is integral. Returns value of expression.
    template <typename Expression, typename T = ValueType>
    typename std::enable_if<std::is_integral<T>::value, T>::type
    static constexpr eval(){
        return details::GetTemplateUnrollType<
                Expression, ValueType, details::EmptyEnv>::value;
    }

    // Is callable when ValueType is not integral.
    // Prints information about unsupported type.
    template <typename Expression, typename T = ValueType>
    typename std::enable_if<!std::is_integral<T>::value, void>::type
    static constexpr eval(){
        std::cout << "Fibin doesn't support: " << typeid(T).name() << std::endl;
    }
};

#endif // FIBIN_H

