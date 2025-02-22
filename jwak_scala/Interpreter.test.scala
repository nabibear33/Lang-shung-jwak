package jwak.interpreter

import munit.FunSuite
import jwak.parser.Expr
import jwak.parser.*
import parsley.Success

class OpsTest extends FunSuite:
  val interpreter = Interpreter(
    Map(
      Expr.Var("슝") -> 0,
      Expr.Var("슈웅") -> 10,
      Expr.Var("슈우웅") -> 2,
    ),
  )
  def doEval(input: Expr) = interpreter.eval(input)._2

  test("덧셈"):
    assertEquals(표현식.parse("좍~좍").map(doEval), Success(Right(2)))
    assertEquals(표현식.parse("좍~~~~~~~~~~좍").map(doEval), Success(Right(2)))

  test("곱셈"):
    assertEquals(표현식.parse("좌악,좌아악").map(doEval), Success(Right(6)))

  test("뺄셈"):
    assertEquals(표현식.parse("슝;좌아악").map(doEval), Success(Right(-3)))

  test("사칙연산"):
    assertEquals(표현식.parse("슈웅;슈우웅@좍").map(doEval), Success(Right(8)))

class AssignTest extends FunSuite:
  import Expr.*
  val interpreter = Interpreter()
  val exprs = List(
    "슝",
    "슝좍",
    "슝...?좍!!!",
    "슈웅좌아악,좌악,좌악",
    "슈우웅슈웅~슝",
  ).flatMap(선언문.parse(_).toOption)

  val results = exprs.scanLeft[(Interpreter, Either[String, Int])](
    (interpreter, Right(0)),
  ) { case ((interpreter, vars), expr) => interpreter.eval(expr) }

  def doVars(input: Expr) = interpreter.eval(input)._1.vars

  test("0 대입"):
    assertEquals(results(1)._1.vars.get(Var("슝")), Some(0))

  test("1 대입"):
    assertEquals(results(2)._1.vars.get(Var("슝")), Some(1))
    assertEquals(results(3)._1.vars.get(Var("슝")), Some(1))

  test("표현식 대입"):
    assertEquals(results(4)._1.vars.get(Var("슈웅")), Some(12))
    assertEquals(results(5)._1.vars.get(Var("슈우웅")), Some(13))
