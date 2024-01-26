#include <gtest/gtest.h>

#include <sstream>

#include "xos.h"

namespace {

constexpr char kHelloWorldStr[] =
    "main( () => () )\n"
    "  out \"Hello World\"";

TEST(Lexer, HelloWorld) {
  std::stringstream ss(kHelloWorldStr);
  xos::Lexer lexer(ss);

  ASSERT_EQ(lexer.Lex().get(),
            xos::Token(xos::Token::identifier, 1, 1, "main"));
  ASSERT_EQ(lexer.Lex().get(), xos::Token(xos::Token::lparen, 1, 5));
  ASSERT_EQ(lexer.Lex().get(), xos::Token(xos::Token::lparen, 1, 7));
  ASSERT_EQ(lexer.Lex().get(), xos::Token(xos::Token::rparen, 1, 8));
  ASSERT_EQ(lexer.Lex().get(), xos::Token(xos::Token::arrow, 1, 10));
  ASSERT_EQ(lexer.Lex().get(), xos::Token(xos::Token::lparen, 1, 13));
  ASSERT_EQ(lexer.Lex().get(), xos::Token(xos::Token::rparen, 1, 14));
  ASSERT_EQ(lexer.Lex().get(), xos::Token(xos::Token::rparen, 1, 16));
  ASSERT_EQ(lexer.Lex().get(), xos::Token(xos::Token::out, 2, 3));
  ASSERT_EQ(lexer.Lex().get(),
            xos::Token(xos::Token::string, 2, 7, "Hello World"));
  ASSERT_EQ(lexer.Lex().get(), xos::Token(xos::Token::eof, 2, 20));
}

TEST(Lexer, UnfinishedArrowToken) {
  std::stringstream ss("=");
  xos::Lexer lexer(ss);

  auto result = lexer.Lex();
  ASSERT_TRUE(result.hasError());
  ASSERT_EQ(result.getError(), "Expected '=>' at row 1, col 1");
}

TEST(Lexer, UnhandledCharacter) {
  std::stringstream ss("%");
  xos::Lexer lexer(ss);

  auto result = lexer.Lex();
  ASSERT_TRUE(result.hasError());
  ASSERT_EQ(result.getError(), "Unhandled character '%'");
}

TEST(Parser, ParseStr) {
  std::stringstream ss("\"Hello World\"");
  xos::Lexer lexer(ss);
  xos::Parser parser(lexer);

  auto result = parser.parseStr();
  ASSERT_NE(result.get(), nullptr);
  ASSERT_EQ(result.get()->getStr(), "Hello World");
}

TEST(Parser, ParseStrError) {
  std::stringstream ss("Hello World");
  xos::Lexer lexer(ss);
  xos::Parser parser(lexer);

  auto result = parser.parseStr();
  ASSERT_TRUE(result.hasError());
  ASSERT_EQ(result.getError(), "Expected string on row 1, col 1");
}

TEST(Parser, ParseOut) {
  std::stringstream ss("out \"Hello World\"");
  xos::Lexer lexer(ss);
  xos::Parser parser(lexer);

  auto result = parser.parseOut();
  ASSERT_NE(result.get(), nullptr);
  ASSERT_EQ(result.get()->getExpr(), xos::ast::Str("Hello World"));
}

TEST(Result, ErrorBuilder) {
  std::string msg("msg");
  xos::Result<int> res = xos::Result<int>::BuildError()
                         << "Test error " << 1 << " " << msg;
  ASSERT_TRUE(res.hasError());
  ASSERT_EQ(res.getError(), "Test error 1 msg");
}

TEST(Result, IndependentStorageSize) {
  constexpr size_t kLargeSize = 1024;
  using large_t = char[kLargeSize];

  auto err = xos::Result<large_t>::Error("err");
  static_assert(
      sizeof(err) < sizeof(large_t),
      "The size of Result<T> should be independent from the size of T.");
}

TEST(Result, ResultDoesntRequireDefaultCtor) {
  struct S {
    S() = delete;
    int x, y;
  };

  [[maybe_unused]] auto err = xos::Result<S>::Error("err");
}

namespace rtti {
enum Kind {
  KA = 0,
  KB,
  KC,
  KD,
  KE,
  KSeparate,
};

/* The heirarchy is:
 *
 *        A
 *       / \
 *      B   D
 *     / \ /
 *    C   E
 */
struct A;
struct B;
struct C;
struct D;

struct A {
  virtual Kind getKind() const { return KA; }
  // If it can be cast to A, then this will always be true. The compiler will
  // reject casting of Separate to this.
  static bool classof(const A&) { return true; }
};
struct B : public A {
  Kind getKind() const override { return KB; }
  static bool classof(const A& a);
};
struct C : public B {
  Kind getKind() const override { return KC; }
  static bool classof(const A& a) { return a.getKind() == KC; }
};
struct D : public A {
  Kind getKind() const override { return KD; }
  static bool classof(const A& a);
};
struct E : public B, public D {
  Kind getKind() const override { return KE; }
  static bool classof(const A& a) { return a.getKind() == KE; }
};

bool B::classof(const A& a) {
  return a.getKind() == KB || C::classof(a) || E::classof(a);
}
bool D::classof(const A& a) { return a.getKind() == KD || E::classof(a); }
}  // namespace rtti

TEST(RTTI, dyn_cast) {
  rtti::A a;
  rtti::B b;
  rtti::C c;
  rtti::D d;

  // Note we can't explicitly check E here since we'd run into the diamond of
  // death where we'd first need to explicitly say E is either B or D before
  // getting to A.
  ASSERT_EQ(xos::dyn_cast<rtti::A>(a)->getKind(), rtti::KA);
  ASSERT_EQ(xos::dyn_cast<rtti::A>(b)->getKind(), rtti::KB);
  ASSERT_EQ(xos::dyn_cast<rtti::A>(c)->getKind(), rtti::KC);
  ASSERT_EQ(xos::dyn_cast<rtti::A>(d)->getKind(), rtti::KD);

  ASSERT_EQ(xos::dyn_cast<rtti::B>(a), nullptr);
  ASSERT_EQ(xos::dyn_cast<rtti::B>(b)->getKind(), rtti::KB);
  ASSERT_EQ(xos::dyn_cast<rtti::B>(c)->getKind(), rtti::KC);

  ASSERT_EQ(xos::dyn_cast<rtti::C>(a), nullptr);
  ASSERT_EQ(xos::dyn_cast<rtti::C>(b), nullptr);
  ASSERT_EQ(xos::dyn_cast<rtti::C>(c)->getKind(), rtti::KC);

  ASSERT_EQ(xos::dyn_cast<rtti::D>(a), nullptr);
  ASSERT_EQ(xos::dyn_cast<rtti::D>(d)->getKind(), rtti::KD);
}

TEST(RTTI, isa) {
  rtti::A a;
  rtti::B b;
  rtti::C c;
  rtti::D d;

  ASSERT_TRUE(xos::isa<rtti::A>(a));
  ASSERT_TRUE(xos::isa<rtti::A>(b));
  ASSERT_TRUE(xos::isa<rtti::A>(c));
  ASSERT_TRUE(xos::isa<rtti::A>(d));

  ASSERT_FALSE(xos::isa<rtti::B>(a));
  ASSERT_TRUE(xos::isa<rtti::B>(b));
  ASSERT_TRUE(xos::isa<rtti::B>(c));

  ASSERT_FALSE(xos::isa<rtti::C>(a));
  ASSERT_FALSE(xos::isa<rtti::C>(b));
  ASSERT_TRUE(xos::isa<rtti::C>(c));

  ASSERT_FALSE(xos::isa<rtti::D>(a));
  ASSERT_TRUE(xos::isa<rtti::D>(d));
}

TEST(RTTI, cast) {
  rtti::A a;
  rtti::B b;
  rtti::C c;
  rtti::D d;

  ASSERT_EQ(xos::cast<rtti::A>(a).getKind(), rtti::KA);
  ASSERT_EQ(xos::cast<rtti::A>(b).getKind(), rtti::KB);
  ASSERT_EQ(xos::cast<rtti::A>(c).getKind(), rtti::KC);
  ASSERT_EQ(xos::cast<rtti::A>(d).getKind(), rtti::KD);

  ASSERT_EQ(xos::cast<rtti::B>(b).getKind(), rtti::KB);
  ASSERT_EQ(xos::cast<rtti::B>(c).getKind(), rtti::KC);

  ASSERT_EQ(xos::cast<rtti::C>(c).getKind(), rtti::KC);

  ASSERT_EQ(xos::cast<rtti::D>(d).getKind(), rtti::KD);
}

}  // namespace
