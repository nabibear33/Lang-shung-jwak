package jwak.interpreter

import jwak.parser.Expr

trait JwakIO:
  def printAscii(v: Int): Unit
  def printValue(v: Int): Unit
  def readValue(v: Expr.Var): Int

object StdOutIO extends JwakIO:
  def printAscii(v: Int): Unit = print(v.toChar)
  def printValue(v: Int): Unit = print(v)
  def readValue(v: Expr.Var): Int = scala.io.StdIn.readInt()

case class Interpreter(
    vars: Map[Expr.Var, Int] = Map.empty,
)(using io: JwakIO = StdOutIO):
  import Expr.*

  /** `p`ure */
  inline private def p[A](v: A) = (this, v)

  inline private def binOp(l: Expr, r: Expr)(
      inline f: (Int, Int) => Int,
  ): Either[String, Int] = for
    l <- eval(l)._2
    r <- eval(r)._2
  yield f(l, r)

  def eval(e: Expr): (Interpreter, Either[String, Int]) = e match
    case v: Var => p(vars.get(v).toRight(s"존재하지 않는 변수 ${v}"))
    case Num(v) => p(Right(v))

    case Add(l, r) => p(binOp(l, r)(_ + _))
    case Sub(l, r) => p(binOp(l, r)(_ - _))
    case Mul(l, r) => p(binOp(l, r)(_ * _))
    case Div(l, r) => p(binOp(l, r)(_ / _))

    case Assign(v, e) =>
      eval(e) match
        case (i, Left(err)) => (i, Left(err))
        case (i, Right(res)) =>
          (i.copy(vars = i.vars.updated(v, res)), Right(res))

    case PrintAscii(v) =>
      eval(e) match
        case (i, Left(err)) => (i, Left(err))
        case (i, Right(res)) =>
          val ascii = res.toChar
          io.printAscii(ascii)
          (i, Right(ascii))

    case PrintValue(_) =>
      eval(e) match
        case (i, Left(err)) => (i, Left(err))
        case (i, Right(res)) =>
          io.printValue(res)
          (i, Right(res))

    case ReadValue(_) => ???
    case If(_, _)     => ???
    case Goto(_)      => ???
