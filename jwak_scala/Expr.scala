package jwak.parser

import jwak.parser.Expr.Var

enum Expr:
  case Noop

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

  case If(condition: Expr, code: Expr)
  case Goto(lines: Int)

  override def toString: String = this match
    case Num(v) => s"$v"
    case Var(n) => s"$$${n.size}"

    case Add(l, r) => s"($l + $r)"
    case Sub(l, r) => s"($l - $r)"
    case Mul(l, r) => s"($l * $r)"
    case Div(l, r) => s"($l / $r)"

    case Assign(v, e) => s"$v = $e"

    case PrintAscii(v) => s"ascii $v"
    case PrintValue(v) => s"value $v"
    case ReadValue(v)  => s"read $v"

    case If(condition, code) => s"if $condition == 0 { $code }"
    case Goto(lines)         => s"goto ${lines.signed}"
    case Noop                => ""

extension (x: Int)
  inline def signed = x.sign match
    case 1 => s"+$x"
    case _ => s"$x"

given Ordering[Expr.Var]:
  def compare(x: Var, y: Var): Int = y.n.compareTo(x.n)
