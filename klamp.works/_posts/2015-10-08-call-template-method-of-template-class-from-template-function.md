---
layout: post
title: Calling a C++ Template Method of a Template Class from a Template Function
date: 2015-10-09 19:01:53
categories: 
---
One thing I love about C++ is that no matter how long you've been doing it, you will never cease running up against bizarre corner cases and general language weirdness.

Imagine the following simple program:

    #include <boost/property_tree/json_parser.hpp>
    #include <sstream>
    #include <iostream>
    #include <string>

    int main()
    {
        std::stringstream ss;
        ss << "{ \"Animals\": [ "
                    "{\"name\": \"bunnies\"},"
                    "{\"name\": \"foxes\"}]}";

        boost::property_tree::ptree pt;
        read_json(ss, pt);

        for (const auto &v : pt.get_child("Animals"))
            std::cout << v.second.get<std::string>("name") << std::endl;
    }

Take note that a type of `v` here is actually `std::pair<const std::basic_string<char>, boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char>, std::less<std::basic_string<char>>>>` (thank goodness for the `auto` keyword) which is, of course, a instantiation of a templated class.

Now I can see myself needing to call `v.second.get<std::string>()` several times here, maybe with different nodes besides `name`. So why not make a helper function.

    template <typename T>
    std::string get_string(const T &pt, const std::string &node)
    {
        return pt.second.get<std::string>(node);
    }

Since `pt` was that long crazy type we had before we'll just use a template for the function parameter and let the compiler do the work. Ok, but does it compile?

    $ g++ -std=c++11 test.cpp
    test.cpp:11:37: error: expected primary-expression before ‘>’ token
         return pt.second.get<std::string>(node);

Wait, what? Unfortunately I've been doing this long enough to find one or two compiler bugs of my own. Nothing is certain in this world, not even compilers; Lets try a different one.

    $ clang++ -std=c++11 test.cpp
    test.cpp:9:22: error: use 'template' keyword to treat 'get' as a dependent template name

Okay, so this one is telling us how to fix it but not what's wrong. Indeed, following the advice will result in successful compilation.

    template <typename T>
    std::string get_string(const T &pt, const std::string &node)
    {
        return pt.second.template get<std::string>(node);
    }

The question is why?

Put simply a dependant template name is a name whose definition depends on template parameters i.e. `std::string` in our `get<std::string>` example above. Now Clang is telling us to use some special syntax to to treat `get` as a dependant template name; so clearly, that's not what the compiler thinks it is when we try to compile the original code. Indeed this is confirmed by the current [Working Draft](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4296.pdf) of the standard. From Section 14.2/4.

> When the name of a member template specialization appears after . or -> in a postfix-expression or after a nested-name-specifier in a qualified-id, and the object expression of the postfix-expression is type-dependent or the nested-name-specifier in the qualified-id refers to a dependent type, but the name is not a member of the current instantiation (14.6.2.1), the member template name must be prefixed by the keyword template. Otherwise the name is assumed to name a non-template.

So if we don't add the `template` keyword it is assumed to name a non-template. The only other use for `<` and `>` in C++ are as comparison operators. When we think about it like that, GCC's error message makes much more sense now.
