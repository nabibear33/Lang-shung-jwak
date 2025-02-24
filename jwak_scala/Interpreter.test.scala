package jwak.interpreter

import munit.FunSuite
import jwak.parser.Expr
import jwak.parser.*
import parsley.Success
import jwak.interpreter.{given_JwakIO, *}

class 연산_테스트 extends FunSuite:
  val interpreter = MutableInterpreter(
    scala.collection.mutable.Map(
      Expr.Var("슝") -> 0,
      Expr.Var("슈웅") -> 10,
      Expr.Var("슈우웅") -> 2,
    ),
  )
  def doEval(input: Expr) = interpreter
    .copy()
    .eval(input)

  test("덧셈"):
    assertEquals(표현식.parse("좍~좍").map(doEval), Success(2))
    assertEquals(표현식.parse("좍~~~~~~~~~~좍").map(doEval), Success(2))

  test("곱셈"):
    assertEquals(표현식.parse("좌악,좌아악").map(doEval), Success(6))

  test("뺄셈"):
    assertEquals(표현식.parse("슝;좌아악").map(doEval), Success(-3))

  test("사칙연산"):
    assertEquals(표현식.parse("슈웅;슈우웅@좍").map(doEval), Success(8))

// TODO: ImmutableInterpreter 개발후 적용
// class 변수_선언_테스트 extends FunSuite:
//   import Expr.*
//   val interpreter = MutableInterpreter()
//   val exprs = List(
//     "슝",
//     "슝좍",
//     "슝...?좍!!!",
//     "슈웅좌아악,좌악,좌악",
//     "슈우웅슈웅~슝",
//   ).flatMap(선언문.parse(_).toOption)

//   val results = exprs
//     .scanLeft(interpreter) { (interpreter, expr) =>
//       interpreter.eval(expr)
//       interpreter
//     }
//     .map(_.vars)

//   test("0 대입"):
//     assertEquals(results(1).get(Var("슝")), Some(0))

//   test("1 대입"):
//     assertEquals(results(2).get(Var("슝")), Some(1))
//     assertEquals(results(3).get(Var("슝")), Some(1))

//   test("표현식 대입"):
//     assertEquals(results(4).get(Var("슈웅")), Some(12))
//     assertEquals(results(5).get(Var("슈우웅")), Some(13))
