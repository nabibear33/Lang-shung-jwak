package jwak.parser

enum Expr:
  // 자료형
  case Num(v: Int)

  // 변수
  case Var(n: String)

  // 연산
  case Add(left: Expr, right: Expr)
  case Sub(left: Expr, right: Expr)
  case Mul(left: Expr, right: Expr)
  case Div(left: Expr, right: Expr)

  // 선언문
  case Assign(v: Var, e: Expr)

  case PrintAscii(v: Var)
  case PrintValue(v: Var)
  case ReadValue(v: Var)

  case If(code: Expr, condition: Expr)
  case Goto(lines: Int)
