package jwak.runner

import java.io.File
import scala.io.Source.fromFile

import parsley.debug.combinator.*
import parsley.debug.PrintView

import jwak.parser.*
import jwak.io.*
import jwak.tojwak.*
import jwak.interpreter.{given_JwakIO, *}
import jwak.logger.JwakLogger

extension [A](xs: Iterable[A])
  def prettyLine =
    xs.zipWithIndex
      .map((line, idx) => s"${idx.toString.padTo(2, ' ')}: $line")
      .mkString("\n") + "\n"

def codeToExprs(code: String)(using log: JwakLogger): Vector[Expr] =
  val lines = code.split("\n").toVector
  log.print(lines.prettyLine)
  lines.map { 코드.parse(_).toOption.getOrElse(Expr.Noop) }

def run(lines: Vector[Expr])(using io: JwakIO, log: JwakLogger): Unit =
  log.print(lines.prettyLine)
  val interpreter =
    MutableInterpreter(lines = lines, maxCommandCount = 1000)(using
      io = io,
      log = log,
    )
  interpreter.eval()
  println("\n")

def run(fileName: String)(using io: JwakIO, log: JwakLogger): Unit =
  val lines = codeToExprs(fromFile(fileName).mkString)
  run(lines)(using io = io, log = log)
