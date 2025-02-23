package jwak

import jwak.parser.*
import jwak.interpreter.*
import java.io.File
import jwak.interpreter.given_JwakIO

@main def main =
  val lines =
    코드.parseFile(File("../example/hello_world.jwak")).get.get.toVector

  // println(lines.mkString("\n"))
  val interpreter = MutableInterpreter(lines = lines)
  lines.foreach(interpreter.eval)
