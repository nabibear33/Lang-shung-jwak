import { compile, Expr, Stmt } from "./compile";
import { IO } from "./io";
import { throwRecursion, 따잇 } from "./utils";

export class Env {
  public memory: number[] = [];
  public pc: number = 0;
  constructor(public io: IO) {}
}

function evaluate(expr: Expr, env: Env): number {
  if (expr.type === "numeric") return expr.value;
  if (expr.type === "deref") return env.memory[expr.address];

  if (expr.type === "arithmetic") {
    if (expr.op === "+")
      return evaluate(expr.lhs, env) + evaluate(expr.rhs, env);
    if (expr.op === "-")
      return evaluate(expr.lhs, env) - evaluate(expr.rhs, env);
    if (expr.op === "*")
      return evaluate(expr.lhs, env) * evaluate(expr.rhs, env);
    if (expr.op === "/")
      return evaluate(expr.lhs, env) / evaluate(expr.rhs, env);
  }

  따잇({ expr, env });
}

function executeOne(stmt: Stmt, env: Env): number {
  switch (stmt.type) {
    case "init":
    case "noop":
      return 1;
    case "declare":
      env.memory[stmt.address] = evaluate(stmt.value, env);
      return 1;
    case "write":
      env.io.output(
        stmt.format === "ascii"
          ? String.fromCharCode(env.memory[stmt.address])
          : env.memory[stmt.address].toString(),
      );
      return 1;
    case "read":
      env.memory[stmt.address] = env.io.input();
      return 1;
    case "branch":
      return evaluate(stmt.condition, env) ? 1 : executeOne(stmt.body, env);
    case "goto":
      return stmt.address;
    default:
      따잇({ stmt, env });
  }
}

function* execute(stmts: Stmt[], env: Env) {
  while (true) {
    const stmt = stmts[env.pc];
    if (!stmt) break;
    env.pc += executeOne(stmt, env);
    yield;
  }
}

export function run(code: string, io: IO, limit: number) {
  const stmts = compile(code);
  const env = new Env(io);
  const program = execute(stmts, env);

  let i = 0;

  for (const _ of program) {
    i++;

    if (i === limit) throwRecursion(env.pc);
  }
}
