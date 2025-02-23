package jwak.interpreter

import jwak.parser.{Expr, given}
import scala.annotation.tailrec
import scala.collection.mutable.Map
import jwak.io.*
import jwak.logger.*

given JwakIO = StdOutIO
given JwakLogger = NoopLogger

case class MutableInterpreter(
    vars: Map[Expr.Var, Int] = Map.empty,
    lines: Vector[Expr] = Vector.empty,
    var cursor: Int = 0,
    var count: Int = 0,
)(using io: JwakIO, log: JwakLogger):
  import Expr.*

  val MaxCommandCount = 1_000_000

  def currentLine: Expr = lines(cursor)

  def eval(): Unit =
    while cursor < lines.length
    do eval(currentLine)

  private inline def addCount() =
    count += 1
    if count > MaxCommandCount then
      throw new Exception(s"RecursionError: Line ${cursor + 1}에서 타임 패러독스!")

  private inline def next[A](v: A) =
    cursor += 1
    addCount()
    v

  def pretty =
    s"[$cursor|${vars.toVector.sortBy(_._1).map((k, v) => s"$k: $v").mkString(", ")}]"

  def eval(e: Expr): Int =
    e match
      case v: Var => vars(v)
      case Num(v) => v

      case Add(l, r) => eval(l) + eval(r)
      case Sub(l, r) => eval(l) - eval(r)
      case Mul(l, r) => eval(l) * eval(r)
      case Div(l, r) => eval(l) / eval(r)

      case Assign(v, e) =>
        val newVal = eval(e)
        log.print(s"$pretty $v = $newVal")
        vars(v) = newVal
        next(newVal)

      case PrintAscii(v) =>
        log.print(s"$pretty ascii $v")
        io.printAscii(eval(v))
        next(0)

      case PrintValue(v) =>
        log.print(s"$pretty value $v")
        io.printValue(eval(v))
        next(0)

      case ReadValue(v) =>
        val input = io.readValue(v)
        log.print(s"$pretty $v <- $input")
        vars(v) = input
        next(input)

      case If(condition, code) =>
        next {
          log.print(s"condition: $condition, code: $code")
          val res = eval(condition)
          log.print(s"$pretty if $condition (= $res) == 0 then $code")
          if res == 0 then eval(code) else 0
        }

      case Goto(diff) =>
        val prevCursor = cursor
        cursor += diff
        log.print(s"$pretty goto $prevCursor -> $cursor ($diff)")
        0

      case Noop => next(0)
