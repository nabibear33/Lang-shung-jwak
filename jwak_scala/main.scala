package jwak

import scala.util.Try

import mainargs.{main, arg, ParserForMethods, Flag}

import jwak.runner.*
import jwak.interpreter.{given}
import jwak.tojwak.toJwak
import jwak.io.*

object Main:
  def main(args: Array[String]): Unit =
    val parser = ParserForMethods(this)
    parser.runEither(args.toSeq, allowPositional = true) match
      case Left(error @ s"Need to specify a sub command:$_") =>
        println(s"$error\n\n${parser.helpText()}")
      case Left(error) => println(error)
      case Right(_)    =>

  @main(doc = "주어진 .jwak 파일을 실행합니다.")
  def run(@arg(doc = "파일 경로") path: String) = runner.run(path)

  @main(doc = "표준 입력으로 주어진 jwak 코드를 실행합니다")
  def eval(@arg(doc = "실행할 코드") code: String) = runner.run(codeToExprs(code))

  @main(doc = "주어진 문장을 jwak 코드로 변환합니다.")
  def jwak(@arg(doc = "변환할 문장") sentence: String) = println(toJwak(sentence))

  @main(doc = "예제 코드를 실행합니다.") def demo() =
    Try(runner.run("../example/fizzbuzz.jwak")).failed.foreach(println)
    runner.run("../example/hello_world.jwak")
    runner.run("../example/gugudan.jwak")
    runner.run("../example/fibonacci.jwak")(using io = ControlledIO(10))
    Try(runner.run("../example/infinite_loop.jwak")).failed.foreach(println)
    Try(runner.run("../example/syntax_error.jwak")).failed.foreach(println)
