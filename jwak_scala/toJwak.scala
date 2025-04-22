package jwak.tojwak

import jwak.parser.{Expr, varAt}

private object JwakConverters:
  val template =
    """|교주님
       |슝 좍
       |슈웅 좌악, 좌아아아악
       |슈우웅 슈웅, 슈웅
       |슈우우웅 슈우웅, 슈웅
       |슈우우우웅 슈우우웅, 슈웅
       |""".stripMargin

  extension (inline n: Int)
    inline def e: String = n match
      case 1 => "좍"
      case _ => s"좌${"아" * (n - 2)}악"

  val at6 = varAt(6).n

  def getLines(num: Int, idx: Int) =
    s"$at6 ${(num + 2).e}; ${2.e}, ${varAt(idx + 1).n}~ $at6\n"

def toJwak(sentence: String): String =
  import JwakConverters.*

  template + sentence.map { char =>
    s"$at6\n" + char.toInt.toString
      .map(_.asDigit)
      .reverse
      .iterator
      .zipWithIndex
      .filter((num, _) => num != 0)
      .map(getLines)
      .mkString + "비비따잇 ㅋㅋㅋㅋㅋㅋ\n"
  }.mkString

@main def main(sentence: String) = println(toJwak(sentence))
