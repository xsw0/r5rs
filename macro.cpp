// #define T(a, b) a
// #define F(a, b) b
// #define not(x) x(F, T)

// #define IF(cond, t, f) cond(t, f)

// #define zero(f, x) x
// #define one(f, x) f(x)
// #define two(f, x) f(f(x))
// #define three(f, x) f(two(x))

// #define inc(num, f, x) num(f, f(x))

// #define get0(a0, ...) a0
// #define get1(a0, a1, ...) a1
// #define get2(a0, a1, a2, ...) a2

// #define is_empty(...) get1(__VA_OPT__(,) F, T)

// #define EMPTY

// #define force(id, ...) id (__VA_ARGS__)
// #define delay(id, ...) id EMPTY () (__VA_ARGS__)

// #define wrap_() wrap
// // #define wrap(F, v, ...) IF(is_empty(__VA_ARGS__), F(v), delay(wrap_, __VA_ARGS__))

// #define cons(a, b) a, b

// #define wrap(f, v, ...) IF(is_empty(__VA_ARGS__), f(v), delay(wrap_, f __VA_ARGS__))

// #define ID(...) __VA_ARGS__
// #define show(a) #a

// wrap(g, a, b)


// #define WRAPPER(func, ...) ((rc = (func)(__VA_ARGS__)), LogRet(#func, __FILE__, __func__, __LINE__, rc), rc)

// WRAPPER(a, b, c, d, e, f)
