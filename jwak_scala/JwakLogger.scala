package jwak.logger

trait JwakLogger:
  def print(value: => String): Unit

object NoopLogger extends JwakLogger:
  def print(value: => String): Unit = ()

object Logger extends JwakLogger:
  def print(value: => String): Unit = println(value)
