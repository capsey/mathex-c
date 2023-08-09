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
#include <map>
#include <string>
#include <type_traits>

namespace mathex {
    /**
     * @brief Evaluation parameters.
     */
    enum class Flags : std::underlying_type<mx_flag>::type {
        None = MX_NONE,                           // Disable all parameters.
        ImplicitParentheses = MX_IMPLICIT_PARENS, // Enable implicit parentheses.
        ImplicitMultiplication = MX_IMPLICIT_MUL, // Enable implicit multiplication.
        ScientificNotation = MX_SCI_NOTATION,     // Enable numbers in scientific notation.
        Addition = MX_ENABLE_ADD,                 // Enable addition operator.
        Substraction = MX_ENABLE_SUB,             // Enable substraction operator.
        Multiplication = MX_ENABLE_MUL,           // Enable multiplication operator.
        Division = MX_ENABLE_DIV,                 // Enable division operator.
        Exponentiation = MX_ENABLE_POW,           // Enable exponentiation operator.
        Modulus = MX_ENABLE_MOD,                  // Enable modulus operator.
        Identity = MX_ENABLE_POS,                 // Enable unary identity operator.
        Negation = MX_ENABLE_NEG,                 // Enable unary negation operator.
    };

    /**
     * @brief Default parameters. Does not include exponentiation and modulus operators.
     */
    constexpr Flags DefaultFlags = static_cast<Flags>(MX_DEFAULT);

    inline constexpr Flags operator+(Flags a, Flags b) {
        return static_cast<Flags>(static_cast<std::underlying_type<Flags>::type>(a) | static_cast<std::underlying_type<Flags>::type>(b));
    }

    inline constexpr Flags operator-(Flags a, Flags b) {
        return static_cast<Flags>(static_cast<std::underlying_type<Flags>::type>(a) & ~static_cast<std::underlying_type<Flags>::type>(b));
    }

    /**
     * @brief Error codes.
     */
    enum class Error : std::underlying_type<mx_error>::type {
        Success = MX_SUCCESS,                // Parsed successfully.
        IllegalName = MX_ERR_ILLEGAL_NAME,   // Name of variable/function contains illegal characters.
        AlreadyDefined = MX_ERR_ALREADY_DEF, // Trying to add a variable/function that already exists.
        OutOfMemory = MX_ERR_NO_MEMORY,      // Out of memory.
        DivisionByZero = MX_ERR_DIV_ZERO,    // Division by zero.
        SyntaxError = MX_ERR_SYNTAX,         // Expression syntax is invalid.
        Undefined = MX_ERR_UNDEFINED,        // Function or variable name not found.
        InvalidArgs = MX_ERR_INVALID_ARGS,   // Arguments validation failed.
        IncorrectArgsNum = MX_ERR_ARGS_NUM,  // Incorrect number of arguments.
    };

    /**
     * @brief Parsed successfully.
     */
    constexpr Error Success = Error::Success;

    /**
     * @brief Type of function or functor for adding into the config.
     */
    using Function = std::function<Error(double[], int, double &)>;

    static mx_error wrapper_function(double args[], int num_args, double *result, void *data) {
        Function *func = reinterpret_cast<Function *>(data);
        return static_cast<mx_error>((*func)(args, num_args, *result));
    }

    /**
     * @brief Configuration for parsing.
     */
    class Config {
    public:
        /**
         * @brief Creates empty configuration object with given parsing parameters.
         *
         * @param flags Evaluation flags.
         */
        Config(Flags flags = DefaultFlags) {
            this->config = mx_create(static_cast<mx_flag>(flags));
        }

        ~Config() {
            mx_free(this->config);
        }

        /**
         * @brief Inserts a variable into the configuration object to be available for use in the expressions.
         *
         * @param name Name of the variable. (should only contain letters, digits or underscore and cannot start with a digit)
         * @param value Reference to value of the variable. Lifetime of a reference is responsibility of a caller.
         *
         * @return Returns `mathex::Success`, or error code if failed to insert.
         */
        Error addVariable(const std::string &name, const double &value) {
            return static_cast<Error>(mx_add_variable(this->config, name.c_str(), &value));
        }

        /**
         * @brief Inserts a constant into the configuration object to be available for use in the expressions.
         *
         * @param name Name of the variable. (should only contain letters, digits or underscore and cannot start with a digit)
         * @param value Value of a constant variable.
         *
         * @return Returns `mathex::Success`, or error code if failed to insert.
         */
        Error addConstant(const std::string &name, double value) {
            return static_cast<Error>(mx_add_constant(this->config, name.c_str(), value));
        }

        /**
         * @brief Inserts a function into the configuration object to be available for use in the expressions.
         *
         * @param name Name of the function. (should only contain letters, digits or underscore and cannot start with a digit)
         * @param apply Function that takes the arguments, writes the result to the given reference and returns `mathex::Success` or appropriate error code.
         *
         * @return Returns `mathex::Success`, or error code if failed to insert.
         */
        Error addFunction(const std::string &name, Function apply) {
            this->functions[name] = apply;
            return static_cast<Error>(mx_add_function(this->config, name.c_str(), wrapper_function, &this->functions[name]));
        }

        /**
         * @brief Removes a variable or a function with given name that was added using `addVariable`, `addConstant` or `addFunction`.
         *
         * @param name Name of the variable or function to remove.
         *
         * @return Returns `mathex::Success`, or error code if failed to remove.
         */
        Error remove(const std::string &name) {
            this->functions.erase(name);
            return static_cast<Error>(mx_remove(this->config, name.c_str()));
        }

        /**
         * @brief Takes mathematical expression and evaluates its numerical value.
         *
         * Result of the evaluation is written into a `result` reference. If evaluation failed, returns error code.
         *
         * @param expression String to evaluate.
         * @param result Reference to write evaluation result to.
         *
         * @return Returns `mathex::Success`, or error code if expression contains any errors.
         */
        Error evaluate(const std::string &expression, double &result) const {
            return static_cast<Error>(mx_evaluate(this->config, expression.c_str(), &result));
        }

    private:
        mx_config *config;
        std::map<std::string, Function> functions;
    };
}

#endif /* MATHEX_HPP */
