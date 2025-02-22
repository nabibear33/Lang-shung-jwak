package jwak

import jwak.parser.*
import jwak.interpreter.*

@main def main =
  val interpreter = Interpreter()
  println(선언문.parse("슝").map(interpreter.eval))

  val exprs = List(
    "슝",
    "슝좍",
    "슝...?좍!!!",
    "슈웅좌아악,좌악,좌악",
    "슈우웅슈웅~슝",
  ).flatMap(선언문.parse(_).toOption)

  val results = exprs.scanLeft[(Interpreter, Either[String, Int])](
    (interpreter, Right(0)),
  ) { case ((interpreter, vars), expr) =>
    val (newInterpreter, newVars) = interpreter.eval(expr)
    (newInterpreter, newVars)
  }
  println(results)
