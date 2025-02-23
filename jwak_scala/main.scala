package jwak

import java.io.File
import scala.io.Source.fromFile

import parsley.debug.combinator.*
import parsley.debug.PrintView

import jwak.parser.*
import jwak.io.*
import jwak.interpreter.{given, *}

@main def main =
  val lines =
    val lines = fromFile("../example/gugudan.jwak").getLines.toVector
    println(lines.mkString("\n"))
    if lines.head != "교주님" then throw Exception("어떻게 이게 리슝좍이냐!")
    lines.tail.map { 코드.parse(_).toOption.getOrElse(Expr.Noop) }

  println(
    lines.zipWithIndex.map((line, idx) => s"$idx: $line").mkString("\n") + "\n",
  )
  val interpreter =
    MutableInterpreter(lines = lines)(using log = jwak.logger.Logger)
  interpreter.eval()
