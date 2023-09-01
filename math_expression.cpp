#include <iostream>
#include <functional>
 
template<typename O>
struct math_object_base
{
    O& self() { return static_cast<O&>(*this); }
    const O& self() const { return static_cast<const O&>(*this); }
};
 
template<typename E>
struct expression : math_object_base<E> {};
 
struct constant : expression<constant>
{
    constant(double value) : value(value) {}
 
    double operator()(double x) const { return value; }
 
private:
    double value;
};
 
struct variable : expression<variable>
{
    double operator()(double x) const { return x; }
};
 
template<typename E>
struct negate : expression<negate<E>>
{
    negate(const expression<E>& expr) : expr(expr.self()) {};
 
    double operator()(double x) const { return -expr(x); }
 
private:
    const E expr;
};
 
template<typename E>
negate<E> operator-(const expression<E>& expr)
{
    return negate<E>(expr);
}
 
template<typename E1, typename OP, typename E2>
struct binary_expression : expression<binary_expression<E1, OP, E2>>
{
    binary_expression(const expression<E1>& expr1, const OP& op, const expression<E2>& expr2) :
        expr1(expr1.self()), op(op), expr2(expr2.self()) {}
 
    double operator()(double x) const
    {
        return op(expr1(x), expr2(x));
    }
 
private:
    const E1 expr1;
    const OP op;
    const E2 expr2;
};
 
#define DEFINE_BIN_OP(oper, OP)\
template<class E1, class E2> \
binary_expression<E1, OP<double>, E2> operator oper \
(const expression<E1> &expr1, const expression<E2> &expr2){ \
    return binary_expression<E1, OP<double>, E2> \
    (expr1, OP<double>(), expr2); \
}\
template<class E1> \
binary_expression<E1, OP<double>, constant> operator oper \
(const expression<E1> &expr1, double value){ \
    return binary_expression<E1, OP<double>, constant> \
    (expr1, OP<double>(), constant(value)); \
}\
template<class E2> \
binary_expression<constant, OP<double>, E2> operator oper \
(double value, const expression<E2> &expr2){ \
    return binary_expression<constant, OP<double>, E2> \
    (constant(value), OP<double>(), expr2); \
} 
 
DEFINE_BIN_OP(+, std::plus)
DEFINE_BIN_OP(-, std::minus)
DEFINE_BIN_OP(*, std::multiplies)
DEFINE_BIN_OP(/, std::divides)
 
template<class E>
struct func_expression : expression<func_expression<E>>
{
    typedef double (*func_t)(double);
 
    func_expression(const expression<E>& expr, func_t func) :
        expr(expr.self()), func(func) {}
 
    double operator()(double x) const 
    {
        return func(expr(x));
    }
 
private:
    const E expr;
    func_t func;
};
 
#define DEFINE_FUNC(func) \
template<typename E> \
func_expression<E> func(const expression<E> &expr){ \
    return func_expression<E>(expr, func); \
}
 
DEFINE_FUNC(sin)
DEFINE_FUNC(cos)
DEFINE_FUNC(exp)
DEFINE_FUNC(log)
DEFINE_FUNC(sqrt)
DEFINE_FUNC(abs)
 
template<class E>
void f(const expression<E>& expr0)
{
    const E& expr = expr0.self();
    std::cout << expr(3) << std::endl;
    std::cout << expr(1.5) << std::endl;
}
 
int main(int argc, char* argv[])
{
    variable x;
    f(cos(x) / x + 2);
}