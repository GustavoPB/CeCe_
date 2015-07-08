#include "core/ExpressionParser.hpp"
#include "core/Range.hpp"
#include "core/Log.hpp"

#include <cmath>
#include <algorithm>

class ExpressionParser
{
    
private:

    const String operators = "+-*/^();<> \t\v\r\n\b";
    Map<String, float> parameters;
    IteratorRange<const char*> iterator;
    
    float signum(float source)
    {
        return (0 < source) - (source < 0);
    }
    
    float add()
    {
        float value = multiply();
        while (true)
        {
            if (iterator.front() == '+')
            {
                iterator.advanceBegin();
                value = value + multiply();
            }
            else if(iterator.front() == '-')
            {
                iterator.advanceBegin();
                value = value - multiply();
            }
            else
                break;
        }
        return value;
    }
        
    float multiply()
    {
        float value = power();
        while (true)
        {
            if (iterator.front() == '*')
            {
                iterator.advanceBegin();
                value = value * power();
            }
            else if(iterator.front() == '/')
            {
                iterator.advanceBegin();
                value = value * power();
            }
            else
                break;
        }
        return value;
    }

    float power()
    {
        float value = parenthesis();
        while (true)
        {
            if (iterator.front() == '^')
            {
                iterator.advanceBegin();
                float exp = parenthesis();
                if (value == 0 && exp == 0)
                    return NAN;
                else
                    value = std::pow(value, exp);
            }
            else
                break;
        }
        return value;
    }

    float parenthesis()
    {
        if (iterator.front() == '(')
        {
            iterator.advanceBegin();
            float value = add();
            if (iterator.front() == ')')
            {
                iterator.advanceBegin();
                return value;
            }
            else
                throw std::runtime_error("Parenthesis error.");
        }
        else
            return function();
    }

    float function()
    {
        char* end;
        float value = strtof(iterator.begin(), &end);
        if (iterator.begin() != end)
        {
            iterator = makeRange<const char*>(end, iterator.end());
            return value;
        }
        String local;
        while (std::find(std::begin(operators), std::end(operators), iterator.front()) == std::end(operators) && iterator.isEmpty());
        {
            local += iterator.front();
            iterator.advanceBegin();
        }
        if (local == "Sin" || local == "sin")
            return std::sin(parenthesis());
        else if (local == "Cos" || local == "cos")
            return std::cos(parenthesis());
        else if (local == "Tan" || local == "tan")
            return std::tan(parenthesis());
        else if (local == "Sinh" || local == "sinh")
            return std::sinh(parenthesis());
        else if (local == "Cosh" || local == "cosh")
            return std::cosh(parenthesis());
        else if (local == "Tanh" || local == "tanh")
            return std::tanh(parenthesis());
        else if (local == "Sqrt" || local == "sqrt")
            return std::sqrt(parenthesis());
        else if (local == "Log" || local == "log")
            return std::log10(parenthesis());
        else if (local == "Ln" || local == "ln")
            return std::log(parenthesis());
        else if (local == "Sgn" || local == "sgn")
            return signum(parenthesis());
        else if (local == "Abs" || local == "abs")
            return std::abs(parenthesis());
        else
        {
            for (auto const &it : parameters)
            {
                if (local == it.first)
                    return it.second;
            }
        }
        throw std::runtime_error("Invalid expression.");
    }
    
public:

    ExpressionParser(const Map<String, float>& param, IteratorRange<const char*> range) NOEXCEPT
    : parameters(param), iterator(range)
    {
        // Nothing to do
    }
    
    float parse()
    {
        return add();
    }
};

float parseExpression(IteratorRange<const char*>& range, const Map<String,float>& parameters)
{
    return ExpressionParser(parameters, range).parse();
}