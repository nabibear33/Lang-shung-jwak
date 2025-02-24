package jwak

import java.io.File
import scala.io.Source.fromFile

import parsley.debug.combinator.*
import parsley.debug.PrintView

import jwak.parser.*
import jwak.io.*
import jwak.interpreter.{given, *}
import jwak.logger.JwakLogger
import scala.util.Try

extension [A](xs: Iterable[A])
  def prettyLine =
    xs.zipWithIndex
      .map((line, idx) => s"${idx.toString.padTo(2, ' ')}: $line")
      .mkString("\n") + "\n"

def run(fileName: String)(using io: JwakIO, log: JwakLogger) =
  val lines =
    val lines = fromFile(fileName).getLines.toVector
    log.print(lines.prettyLine)
    lines.map { 코드.parse(_).toOption.getOrElse(Expr.Noop) }

  log.print(lines.prettyLine)
  val interpreter =
    MutableInterpreter(lines = lines, maxCommandCount = 1000)(using
      io = io,
      log = log,
    )
  interpreter.eval()
  println("\n")

def main(args: Array[String]): Unit = args.toVector match
  case Vector(path) => run(path)
  case _ =>
    Try(run("../example/fizzbuzz.jwak")).failed.foreach(println)
    run("../example/hello_world.jwak")
    run("../example/gugudan.jwak")
    run("../example/fibonacci.jwak")(using io = ControlledIO(10))
    Try(run("../example/infinite_loop.jwak")).failed.foreach(println)
    Try(run("../example/syntax_error.jwak")).failed.foreach(println)
