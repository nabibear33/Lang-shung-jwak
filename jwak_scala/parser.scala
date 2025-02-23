package jwak.parser

import parsley.Parsley
import parsley.Parsley.*
import parsley.expr.chain
import parsley.combinator.*
import parsley.character.*
import parsley.syntax.character.{charLift, stringLift}
import parsley.errors.combinator.ErrorMethods

def symbol(str: String): Parsley[String] = atomic(string(str))

val 주석: Parsley[Unit] = symbol("#") ~> manyTill(item, endOfLine).void

/** @see
  *   https://github.com/nabibear33/Lang-shung-jwak?tab=readme-ov-file#문법
  */
val 무시: Parsley[Unit] = many(
  whitespace.void <|> 주석.void <|> oneOf('?', '!', '.', ' ').void,
).void.hide

def lexeme[A](p: Parsley[A]): Parsley[A] = 무시 ~> p <~ 무시
def token[A](p: Parsley[A]): Parsley[A] = lexeme(atomic(p))

/** @see
  *   https://github.com/nabibear33/Lang-shung-jwak?tab=readme-ov-file#교주님
  */
val 교주님 = "교주님".void <~ (무시 <|> eof)

/** @see
  *   https://github.com/nabibear33/Lang-shung-jwak?tab=readme-ov-file#자료형
  */
val 자료형: Parsley[Expr] = token {
  '좍'.map(_ => Expr.Num(1))
    <|> ('좌' ~> countMany('아') <~ '악')
      .map(chars => Expr.Num(chars + 2))
}

val 변수: Parsley[Expr.Var] = token {
  val 슝 = "슝".map[Expr.Var](Expr.Var.apply(_))
  val 슈웅 = ('슈' ~> countMany('우') <~ '웅')
    .map[Expr.Var](chars => varAt(chars + 2))

  슝 <|> 슈웅
}

val 값 = 자료형 <|> 변수

val op: Parsley[(Expr, Expr) => Expr] =
  some('~').as(Expr.Add.apply)
    <|> some(';').as(Expr.Sub.apply)
    <|> some(',').as(Expr.Mul.apply)
    <|> some('@').as(Expr.Div.apply)

/** @see https://github.com/nabibear33/Lang-shung-jwak?tab=readme-ov-file#연산 */
val 연산: Parsley[Expr] = chain.left1(값)(token(op))

val 표현식: Parsley[Expr] = 연산 <|> 값

/** @see
  *   https://github.com/nabibear33/Lang-shung-jwak?tab=readme-ov-file#변수-선언
  */
lazy val `변수 선언`: Parsley[Expr] =
  (변수 <~> (eof <|> endOfLine.void <|> ~표현식)).map {
    case (v, e: Unit) => Expr.Assign(v, Expr.Num(0))
    case (v, e: Expr) => Expr.Assign(v, e)
  }

/** n번째 변수 이름을 반환합니다. */
def varAt(at: Int): Expr.Var = at match
  case 0 => throw new IllegalArgumentException("1번째 변수부터 존재합니다.")
  case 1 => Expr.Var("슝")
  case n => Expr.Var(s"슈${"우" * (n - 2)}웅")

val ㅋ = token(countMany('ㅋ'))
val 따잇 = token(some('따') ~> '잇')

/** @see
  *   https://github.com/nabibear33/Lang-shung-jwak?tab=readme-ov-file#출력
  */
lazy val 출력: Parsley[Expr] =
  (token("비비") ~> ㅋ
    <~> option("보호막") <~> ㅋ
    <~> 따잇 ~> ㅋ)
    .map { case (((a, shield), b), c) =>
      val ㅋ = varAt(a + b + c)
      if shield.isDefined then Expr.PrintValue(ㅋ) else Expr.PrintAscii(ㅋ)
    }

/** @see
  *   https://github.com/nabibear33/Lang-shung-jwak?tab=readme-ov-file#입력
  */
lazy val 입력: Parsley[Expr] =
  ((token("순수") ~> ㅋ) <~> (따잇 ~> ㅋ))
    .map((a, b) => Expr.ReadValue(varAt(a + b)))

/** @see
  *   https://github.com/nabibear33/Lang-shung-jwak?tab=readme-ov-file#if문
  */
lazy val if문 =
  ((~선언문 <~ token("하는재미")) <~> 표현식)
    .map((code, condition) => Expr.If(condition, code))

/** @see
  *   https://github.com/nabibear33/Lang-shung-jwak?tab=readme-ov-file#goto문
  */
lazy val goto문: Parsley[Expr] =
  (range(1, 2)(token("에잇")).map(_.size) <~> countMany('ㅋ')).map {
    case (_, n) if n == 0 =>
      throw new IllegalArgumentException(s"goto 0은 무한 루프에 빠집니다.")
    case (1, n) => Expr.Goto(-n)
    case (2, n) => Expr.Goto(n)
    case _      => throw new IllegalArgumentException("에잇은 1번 또는 2번 사용해야 합니다.")
  }

lazy val 선언문: Parsley[Expr] = goto문 <|> 입력 <|> 출력 <|> `변수 선언`

val 코드 = atomic(if문) <|> 입력 <|> 출력 <|> `변수 선언` <|> goto문

// val 코드 = 교주님 ~> many(선언문 <~ 무시 <~ endOfLine.void)
