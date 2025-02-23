package jwak.interpreter

import jwak.parser.Expr
import scala.annotation.tailrec
import scala.collection.mutable.Map
import jwak.io.*

given JwakIO = StdOutIO

case class MutableInterpreter(
    vars: Map[Expr.Var, Int] = Map.empty,
    lines: Vector[Expr] = Vector.empty,
    cursor: Int = 0,
)(using io: JwakIO):
  import Expr.*

  def currentLine: Expr = lines(cursor)

  def eval(e: Expr): Int =
    // println(s"$cursor: $e")
    e match
      case v: Var => vars(v)
      case Num(v) => v

      case Add(l, r) => eval(l) + eval(r)
      case Sub(l, r) => eval(l) - eval(r)
      case Mul(l, r) => eval(l) * eval(r)
      case Div(l, r) => eval(l) / eval(r)

      case Assign(v, e) =>
        val newVal = eval(e)
        vars(v) = newVal
        newVal

      case PrintAscii(v) =>
        io.printAscii(eval(v))
        0

      case PrintValue(v) =>
        io.printValue(eval(v))
        0

      case ReadValue(v) =>
        val input = io.readValue(v)
        vars(v) = input
        input

      case If(code, condition) =>
        if eval(condition) == 0 then eval(code) else 0

      case Goto(diff) =>
        val next = this.copy(cursor = cursor + diff)
        next.eval(next.currentLine)
