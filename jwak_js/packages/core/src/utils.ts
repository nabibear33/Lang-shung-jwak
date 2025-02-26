export function 따잇(cause: unknown): never {
  throw new Error("따잇", { cause });
}

export function throwSyntax(): never {
  throw new Error("SyntaxError: 어떻게 이게 리슝좍이냐!");
}

export function throwRecursion(pc: number): never {
  throw new Error(`RecursionError: Line ${pc}에서 타임 패러독스!`);
}
