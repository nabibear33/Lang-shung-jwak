package jwak.tojwak

import jwak.interpreter.{MutableInterpreter, given_JwakLogger}

import munit.FunSuite
import jwak.parser.Expr
import jwak.io.JwakIO
import jwak.runner.codeToExprs

def evalToJwak(sentence: String) =
  object StringBuilderIO extends JwakIO:
    val sb = StringBuilder()
    def printAscii(v: Int): Unit = sb.append(v.toChar)
    def printValue(v: Int): Unit = sb.append(v)
    def readValue(v: Expr.Var): Int = 0

  val code = toJwak(sentence)
  val exprs = codeToExprs(code)

  val interpreter =
    MutableInterpreter(lines = exprs)(using io = StringBuilderIO)
  interpreter.eval()
  StringBuilderIO.sb.toString

class ToJwakSnapshotTest extends FunSuite:
  def assertToJwak(input: String) =
    assertEquals(evalToJwak(input), input)

  test("toJwak produces expected code"):
    assertToJwak("Hello, World!")

  test("toJwak works with unicode"):
    assertToJwak("안녕하세요")

  test("toJwak works with special characters"):
    assertToJwak("!@#$%^&*()_+")
    assertToJwak("~`")
    assertToJwak("1234567890")

  test("toJwak works with long message"):
    val templerun =
      """|안녕■연아너를처음본순간부터좋아
         |했어방학전에고백하고싶었는데바보
         |같이그땐용기가없더라지금은이수많
         |은사람들앞에서오로지너만사랑한다
         |고말하고싶어서큰마음먹고용기내어
         |봐매일매일버스에서너볼때마다두근
         |댔고동아리랑과활동에서도너만보이
         |고너생각만나고지난3월부터계속그
         |랬어니가남자친구랑헤어지고니맘이
         |아파울때내마음도너무아팠지만내심
         |좋은맘두있었어이런내맘을어떻게말
         |할지고민하다가정말인생에서제일크
         |게용기내어세상에서제일멋지게많은
         |사람들앞에서너한테고백해주고싶었
         |어사랑하는■연님내여자가되줄래?
         |아니나만의태양이되어줄래?난너의
         |달님이될게내일3시반에너수업마
         |치고학관앞에서기다리고있을게
         |너를사랑하는■윤이가""".stripMargin
    assertToJwak(templerun)
    assertToJwak("이제 누가 공지해주냐")
