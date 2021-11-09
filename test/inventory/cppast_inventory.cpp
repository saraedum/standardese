// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_entity_kind.hpp>

#include "../../external/catch/single_include/catch2/catch.hpp"

#include "../util/cpp_file.hpp"
#include "../util/logger.hpp"

#include "../../standardese/inventory/cppast_inventory.hpp"
#include "../../standardese/inventory/symbols.hpp"

namespace standardese::test::inventory
{

using standardese::inventory::symbols;
using standardese::inventory::cppast_inventory;

TEST_CASE("Function Lookup in MarkDown Links", "[cppast_inventory]")
{
    auto logger = util::logger::throwing_logger();

    SECTION("Functions are Found With or Without Their Arguments")
    {
        const util::cpp_file header("void f(int arg);");

        cppast_inventory inventory({header}, header);
        symbols symbols{inventory};

        CHECK(symbols.find("f(int)"));
        CHECK(symbols.find("f"));
        CHECK(symbols.find("::f"));
        CHECK(symbols.find("::f(int)"));

        CHECK(!symbols.find("f()"));
        CHECK(!symbols.find("f(int arg)"));
        CHECK(!symbols.find("f::f"));
    } 

    SECTION("Functions are Found With or Without Their Template Arguments")
    {
        const util::cpp_file header("template <typename T> void f(T arg);");

        cppast_inventory inventory({header}, header);
        symbols symbols{inventory};

        CHECK(symbols.find("f"));
        CHECK(symbols.find("f(T)"));
        CHECK(symbols.find("f<T>"));
        CHECK(symbols.find("f<T>(T)"));
    }

    SECTION("Functions are Found Across Namespaces")
    {
        const util::cpp_file header(R"(
            namespace a {
                namespace b {
                    void f();
                }
            }

            namespace b {
                void f(int);
            }
            )");

        cppast_inventory inventory({header}, header);
        symbols symbols{inventory};

        const auto& a = header["a"];
        const auto& b = header["b"];
      
        const auto target =[](const auto& anchor) {
          return anchor.value().accept([&](auto&& target) -> type_safe::object_ref<const cppast::cpp_entity> {
            using T = std::decay_t<decltype(target)>;
            if constexpr (std::is_same_v<T, model::link_target::cppast_target>) {
              return target.target;
            } else {
              throw std::logic_error("name did not resolve to a cppast entity");
            }
          });
        };

        SECTION("When there are Multiple Matches the Relative Starting Point Matters")
        {
            CHECK(target(symbols.find("b::f")) == target(symbols.find("::b::f")));
            CHECK(target(symbols.find("b::f", a)) == target(symbols.find("::a::b::f")));
        }

        SECTION("A Relative Lookup is not Greedy, it can back up to an Outer Scope")
        {
            CHECK(target(symbols.find("b::f(int)", a)) == target(symbols.find("::b::f")));
        }
    }

    SECTION("Functions can be Found Using Their Arguments")
    {
        const util::cpp_file header(R"(
            struct X {
                void f(bool arg) noexcept;
                void f(int arg, char brg) const;
                void f(int, char, bool);
            };
            )");

        cppast_inventory inventory({header}, header);
        symbols symbols{inventory};

        SECTION("Using Signature")
        {
            // noexcept is not part of the signature
            CHECK(symbols.find("X::f(bool)"));

            // const is part of the signature
            CHECK(!symbols.find("X::f(int, char)"));
            CHECK(symbols.find("X::f(int, char)const"));
            
            CHECK(symbols.find("X::f(int, char, bool)"));
        }

        SECTION("Using Argument Names")
        {
            CHECK(symbols.find("X::f(arg)"));
            CHECK(symbols.find("X::f(arg, brg)"));

            // Unnamed arguments cannot be used in this symbols.
            CHECK(!symbols.find("X::f(,,)"));
        }
    }
}

TEST_CASE("Parameter Lookup in MarkDown Links", "[cppast_inventory]")
{
    SECTION("Parameters can be Mentioned Directly in Function Scope")
    {
        const util::cpp_file header("void f(int arg);");

        cppast_inventory inventory({header}, header);
        symbols symbols{inventory};

        CHECK(!symbols.find("arg"));
        CHECK(symbols.find("arg", header["f"]));
    }

    SECTION("Parameters can be Referenced with a `.` Outside Function Scope")
    {
        const util::cpp_file header(R"(
            struct X {
                void f(int arg);
            };
            )");

        cppast_inventory inventory({header}, header);
        symbols symbols{inventory};

        CHECK(symbols.find("X::f.arg"));
        CHECK(symbols.find("::X::f.arg"));
        CHECK(!symbols.find("arg"));
        CHECK(symbols.find("f.arg", header["X"]));
        CHECK(!symbols.find("f.arg"));

        // This does not make too much sense but we do not want to enforce any
        // standards that do not bring much functionality (it would help us
        // distinguish the file header.hpp from the member header::hpp but that
        // seems a bit constructed.) So we just treat . and :: equally.
        CHECK(symbols.find("X.f.arg"));
    }

    SECTION("Relative Parameter Lookup is not Greedy, it can back up to an Outer Scope")
    {
        const util::cpp_file header(R"(
            void f(int brg);

            struct X {
                void f(int arg);
            };
            )");

        cppast_inventory inventory({header}, header);
        symbols symbols{inventory};

        CHECK(symbols.find("f.brg", header["X"]));
    }
}

TEST_CASE("Operator Lookup in MarkDown Links", "[inventory]")
{
    SECTION("Operators can be Looked Up Like any Other Function")
    {
        const util::cpp_file header(R"(
            struct X {
                X operator-() const;

                X& operator*();
                const X& operator*() const;

                X operator+(const X&) const;
                X operator-(const X&) const;
                X operator*(const X&) const;
                X operator/(const X&) const;

                X& operator+=(const X&);
                X& operator-=(const X&);
                X& operator*=(const X&);
                X& operator/=(const X&);

                bool operator<(const X&) const;
                bool operator<=(const X&) const;
                bool operator==(const X&) const;
                bool operator>=(const X&) const;
                bool operator>(const X&) const;
                bool operator!=(const X&) const;

                X& operator=(const X&);
                X& operator=(X&&);

                explicit operator bool() const;
            };
            )");

        cppast_inventory inventory({header}, header);
        symbols symbols{inventory};

        SECTION("Lookups with the Proper Names Work")
        {
            CHECK(!symbols.find("::X::operator--"));

            CHECK(symbols.find("::X::operator-"));
            CHECK(symbols.find("::X::operator+"));
            CHECK(symbols.find("::X::operator*"));
            CHECK(symbols.find("::X::operator/"));

            CHECK(symbols.find("::X::operator-="));
            CHECK(symbols.find("::X::operator+="));
            CHECK(symbols.find("::X::operator*="));
            CHECK(symbols.find("::X::operator/="));

            CHECK(symbols.find("::X::operator<"));
            CHECK(symbols.find("::X::operator<="));
            CHECK(symbols.find("::X::operator=="));
            CHECK(symbols.find("::X::operator>="));
            CHECK(symbols.find("::X::operator>"));
            CHECK(symbols.find("::X::operator!="));

            CHECK(symbols.find("::X::operator bool"));
        }

        SECTION("We are Liberal About Whitespace")
        {
            CHECK(symbols.find(":: X :: operator+ = "));

            // As a side-effect, if there is a function `operatorbool()`, then
            // there is no way to distinguish the two without using
            // \unique_name.
            CHECK(symbols.find("::X::operatorbool"));
        }

        SECTION("We can use Arguments to Distinguish Overloads")
        {
            CHECK(symbols.find("::X::operator-"));
            CHECK(symbols.find("::X::operator-()"));
            CHECK(symbols.find("::X::operator-() const"));
            CHECK(symbols.find("::X::operator-(X const&) const"));

            // We have to follow cppast's spelling of signatures. So this does not work.
            CHECK(!symbols.find("::X::operator-(const X&) const"));
        }
    }
}

TEST_CASE("Template Parameter Lookup in MarkDown Links", "[cppast_inventory]")
{
    const util::cpp_file header(R"(
        template <typename T>
        struct X {};

        template <typename S>
        void f();
        )");

    cppast_inventory inventory({header}, header);
    symbols symbols{inventory};

    SECTION("Template Parameters can be Mentioned Directly in the Corresponding Scope")
    {
        CHECK(symbols.find("T", header["X"]));
        CHECK(symbols.find("S", header["f"]));
    }

    SECTION("Template Parameters can be Looked up from other Scopes")
    {
        CHECK(symbols.find("X.T"));
        CHECK(symbols.find("f.S"));
    }
}

TEST_CASE("Type Lookup in MarkDown Links", "[cppast_inventory]") {
    const util::cpp_file header(R"(
        template <typename T>
        struct X {};

        struct Y {};
        )");

    cppast_inventory inventory({header}, header);
    symbols symbols{inventory};

    SECTION("Types can be Found with and without their Template Parameters") {
        CHECK(symbols.find("X"));
        CHECK(symbols.find("X<T>"));
    }
}

}
