package jwak.parser

import parsley.{Success, Failure}
import munit.FunSuite

import Expr.*
import scala.util.Try

class 표현식_테스트 extends FunSuite:

  test("자료형"):
    assertEquals(자료형.parse("좍"), Success(Num(1)))
    assertEquals(자료형.parse("좌악"), Success(Num(2)))
    assertEquals(자료형.parse("좌아악"), Success(Num(3)))
    assertEquals(자료형.parse("좌아아악"), Success(Num(4)))

  test("연산"):
    assertEquals(연산.parse("좍~좍"), Success(Add(Num(1), Num(1))))
    assertEquals(연산.parse("좍~~~~~~~~~~좍"), Success(Add(Num(1), Num(1))))
    assertEquals(연산.parse("좌악,좌아악"), Success(Mul(Num(2), Num(3))))
    assertEquals(연산.parse("슝;좌아악"), Success(Sub(Var("슝"), Num(3))))
    assertEquals(
      연산.parse("슈웅;슈우웅@좍"),
      Success(Div(Sub(Var("슈웅"), Var("슈우웅")), Num(1))),
    )

class 선언문_테스트 extends FunSuite:
  test("변수 선언"):
    assertEquals(`변수 선언`.parse("슝"), Success(Assign(Var("슝"), Num(0))))
    assertEquals(`변수 선언`.parse("슝좍"), Success(Assign(Var("슝"), Num(1))))
    assertEquals(`변수 선언`.parse("슝...?좍!!!"), Success(Assign(Var("슝"), Num(1))))
    assertEquals(
      `변수 선언`.parse("슈웅좌아악,좌악,좌악"),
      Success(Assign(Var("슈웅"), Mul(Mul(Num(3), Num(2)), Num(2)))),
    )
    assertEquals(
      `변수 선언`.parse("슈우웅슈웅~슝"),
      Success(Assign(Var("슈우웅"), Add(Var("슈웅"), Var("슝")))),
    )
    val expr = Add(Mul(Mul(Num(1), Num(4)), Var("슈웅")), Num(4))
    assertEquals(
      표현식.parse("좍???, 좌아아악,....!! 슈웅~ 좌아아악!"),
      Success(expr),
    )
    assertEquals(
      `변수 선언`.parse("슈우웅좍???, 좌아아악,....!! 슈웅~ 좌아아악!"),
      Success(Assign(Var("슈우웅"), expr)),
    )

  test("교주님"):
    assertEquals(교주님.parse("교주님"), Success(()))

  test("ascii 출력"):
    assert(Try(출력.parse("비비따잇")).isFailure)
    assertEquals(출력.parse("비비따잇ㅋㅋ"), Success(PrintAscii(Var("슈웅"))))
    assertEquals(출력.parse("비비ㅋ따따잇ㅋ"), Success(PrintAscii(Var("슈웅"))))
    assertEquals(출력.parse("비비ㅋㅋㅋ따잇"), Success(PrintAscii(Var("슈우웅"))))
    assert(출력.parse("비ㅋㅋ비따잇").isFailure)
    assert(출력.parse("비비따ㅋㅋㅋ잇").isFailure)

    assertEquals(출력.parse("비비 따잇!?ㅋ"), Success(PrintAscii(Var("슝"))))
    assertEquals(출력.parse("비비....ㅋㅋㅋ 따잇!!ㅋ"), Success(PrintAscii(Var("슈우우웅"))))

  test("값 출력"):
    assert(Try(출력.parse("비비보호막따잇")).isFailure)
    assertEquals(출력.parse("비비보호막따잇ㅋㅋ"), Success(PrintValue(Var("슈웅"))))
    assertEquals(출력.parse("비비ㅋ보호막따잇ㅋ"), Success(PrintValue(Var("슈웅"))))
    assertEquals(출력.parse("비비보호막ㅋㅋㅋ따잇"), Success(PrintValue(Var("슈우웅"))))

  test("입력"):
    assert(Try(입력.parse("순수따잇")).isFailure)
    assertEquals(입력.parse("순수따따잇ㅋㅋ"), Success(ReadValue(Var("슈웅"))))
    assert(입력.parse("순ㅋㅋ수따잇").isFailure)
    assert(입력.parse("순수따ㅋㅋㅋ잇").isFailure)

  test("if문"):
    assertEquals(
      if문.parse("비비따잇ㅋ하는재미슝"),
      Success(If(PrintAscii(Var("슝")), Var("슝"))),
    )

  test("goto문"):
    assertEquals(goto문.parse("에잇ㅋ"), Success(Goto(-1)))
    assertEquals(goto문.parse("에잇에잇ㅋㅋㅋㅋ"), Success(Goto(4)))
