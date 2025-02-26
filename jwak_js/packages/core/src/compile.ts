import { throwSyntax } from "./utils";

interface Token {
  type: string;
  value: string;
}

interface Rule {
  matcher: RegExp;
  builder: (code: string) => Token;
}

const createRule = (type: string, matcher: RegExp): Rule => ({
  matcher,
  builder: (value: string): Token => ({ type, value }),
});

const rules = [
  createRule("init", /^(교주님)/),
  createRule("numeric", /^(좍|좌아*악)/),
  createRule("arithmetic", /^(~+|;+|,+|@+)/),
  createRule("var", /^(슝|슈우*웅)/),
  createRule("writeAscii", /^(비비ㅋ+따+잇ㅋ*|비비ㅋ*따+잇ㅋ+)/),
  createRule(
    "writeValue",
    /^(비비ㅋ+보호막ㅋ*따+잇ㅋ*|비비ㅋ*보호막ㅋ+따+잇ㅋ*|비비ㅋ*보호막ㅋ*따+잇ㅋ+)/,
  ),
  createRule("read", /^(순수ㅋ+따+잇ㅋ*|순수ㅋ*따+잇ㅋ+)/),
  createRule("branch", /^(하는재미)/),
  createRule("goto", /^(에잇ㅋ+|에잇에잇ㅋ+)/),
];

function tokenize(code: string, tokens: Token[] = []) {
  if (!code) return tokens;

  let token: Token | null = null;

  for (const { matcher, builder } of rules) {
    const result = code.match(matcher)?.at(0);
    if (!result) continue;

    token = builder(result);
    break;
  }

  if (!token) throwSyntax();

  return tokenize(code.slice(token.value.length), [...tokens, token]);
}

const toAddress = (str: string) => [...str].filter((c) => c === "ㅋ").length;

type NumericExpr = { type: "numeric"; value: number };
type ArithmeticExpr = {
  type: "arithmetic";
  op: "+" | "-" | "*" | "/";
  lhs: Expr;
  rhs: Expr;
};
type DerefExpr = { type: "deref"; address: number };

export type Expr = NumericExpr | ArithmeticExpr | DerefExpr;

type InitStmt = { type: "init" };
type NoopStmt = { type: "noop" };
type DeclareStmt = { type: "declare"; value: Expr; address: number };
type WriteStmt = { type: "write"; format: "ascii" | "int"; address: number };
type ReadStmt = { type: "read"; address: number };
type BranchStmt = { type: "branch"; condition: Expr; body: Stmt };
type GotoStmt = { type: "goto"; address: number };

export type Stmt =
  | InitStmt
  | DeclareStmt
  | WriteStmt
  | ReadStmt
  | BranchStmt
  | GotoStmt
  | NoopStmt;

function parseExpr(tokens: Token[], stack: Expr[] = []): [Expr, Token[]] {
  if (!tokens.length) {
    if (stack.length !== 1) throwSyntax();
    return [stack[0], tokens];
  }

  const [token, ...rtoken] = tokens;

  if (token.type === "arithmetic") {
    if (rtoken.length < 1) throwSyntax();

    const op = ({ "~": "+", ";": "-", ",": "*", "@": "/" } as const)[
      token.value[0]
    ];
    if (!op) throwSyntax();

    const [lookahead, ...rrtoken] = rtoken;

    const [lhs, ...rstack] = stack;
    const [rhs] = parseExpr([lookahead], []);

    const expr: ArithmeticExpr = { type: "arithmetic", op, lhs, rhs };
    return parseExpr(rrtoken, [expr, ...rstack]);
  }

  if (token.type === "numeric") {
    const expr: NumericExpr = { type: "numeric", value: token.value.length };
    return parseExpr(rtoken, [expr, ...stack]);
  }

  if (token.type === "var") {
    const address = token.value.length;
    const expr: DerefExpr = { type: "deref", address };
    return parseExpr(rtoken, [expr, ...stack]);
  }

  if (stack.length !== 1) throwSyntax();
  return [stack[0], tokens];
}

function parseStmt(tokens: Token[], stack: Stmt[] = []): Stmt {
  if (!tokens.length) {
    if (stack.length !== 1) return { type: "noop" };
    return stack[0];
  }

  const [token, ...rtoken] = tokens;

  if (token.type === "init") {
    if (rtoken.length || stack.length) throwSyntax();
    const stmt: InitStmt = { type: "init" };
    return parseStmt(rtoken, [stmt, ...stack]);
  }

  if (token.type === "var") {
    if (!rtoken.length) {
      const stmt: DeclareStmt = {
        type: "declare",
        value: { type: "numeric", value: 0 },
        address: token.value.length,
      };
      return parseStmt([], [stmt, ...stack]);
    }

    const [value, rrtoken] = parseExpr(rtoken);
    const stmt: DeclareStmt = {
      type: "declare",
      value,
      address: token.value.length,
    };
    return parseStmt(rrtoken, [stmt, ...stack]);
  }

  if (token.type === "writeAscii" || token.type === "writeValue") {
    const format = token.type === "writeAscii" ? "ascii" : "int";
    const address = toAddress(token.value);
    const stmt: WriteStmt = { type: "write", format, address };
    return parseStmt(rtoken, [stmt, ...stack]);
  }

  if (token.type === "read") {
    const address = toAddress(token.value);
    const stmt: ReadStmt = { type: "read", address };
    return parseStmt(rtoken, [stmt, ...stack]);
  }

  if (token.type === "branch") {
    const [prevStmt, ...rstack] = stack;
    const [condition, rrtoken] = parseExpr(rtoken);

    const stmt: BranchStmt = { type: "branch", body: prevStmt, condition };
    return parseStmt(rrtoken, [stmt, ...rstack]);
  }

  if (token.type === "goto") {
    const dir = token.value.startsWith("에잇에잇") ? 1 : -1;
    const rel = toAddress(token.value);
    const address = dir * rel;

    const stmt: GotoStmt = { type: "goto", address };
    return parseStmt(rtoken, [stmt, ...stack]);
  }

  throwSyntax();
}

export function compile(code: string) {
  return [...code]
    .filter((c) => !"?!. ".includes(c))
    .join("")
    .split("\n")
    .map((line) => tokenize(line))
    .map((tokens) => parseStmt(tokens));
}
