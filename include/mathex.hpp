/*
  Copyright (c) 2023 Caps Lock

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef MATHEX_HPP
#define MATHEX_HPP

#include "mathex.h"
#include <functional>
#include <string>
#include <unordered_map>

namespace mathex
{
    using function = std::function<mx_error(double[], int, double &)>;

    static mx_error wrapper_function(double args[], int num_args, double *result, void *data)
    {
        auto func = reinterpret_cast<function *>(data);
        return (*func)(args, num_args, *result);
    }

    class Config
    {
    public:
        Config(mx_flag flags = static_cast<mx_flag>(MX_DEFAULT))
        {
            this->config = mx_create(flags);
        }

        ~Config()
        {
            mx_free(this->config);
        }

        mx_error addVariable(const std::string &name, const double &value)
        {
            return mx_add_variable(this->config, name.c_str(), &value);
        }

        mx_error addConstant(const std::string &name, double value)
        {
            return mx_add_constant(this->config, name.c_str(), value);
        }

        mx_error addFunction(const std::string &name, function apply)
        {
            this->functions[name] = apply;
            return mx_add_function(this->config, name.c_str(), wrapper_function, &this->functions[name]);
        }

        mx_error remove(const std::string &name)
        {
            this->functions.erase(name);
            return mx_remove(this->config, name.c_str());
        }

        mx_error evaluate(const std::string &expression, double &result)
        {
            return mx_evaluate(this->config, expression.c_str(), &result);
        }

    private:
        mx_config *config;
        std::unordered_map<std::string, function> functions;
    };
}

#endif /* MATHEX_HPP */
