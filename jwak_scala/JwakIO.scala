package jwak.io

import jwak.parser.Expr

trait JwakIO:
  def printAscii(v: Int): Unit
  def printValue(v: Int): Unit
  def readValue(v: Expr.Var): Int

object StdOutIO extends JwakIO:
  def printAscii(v: Int): Unit = print(v.toChar)
  def printValue(v: Int): Unit = print(v)
  def readValue(v: Expr.Var): Int = scala.io.StdIn.readInt()

case class ControlledIO(input: Int) extends JwakIO:
  def printAscii(v: Int): Unit = print(v.toChar)
  def printValue(v: Int): Unit = print(v)
  def readValue(v: Expr.Var): Int = input
