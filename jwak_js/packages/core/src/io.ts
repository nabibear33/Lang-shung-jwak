export class IO {
  constructor(
    public input: () => number,
    public output: (c: string) => void,
  ) {}
}

export class PeekIO extends IO {
  private buffer: string[] = [];
  constructor(input: () => number) {
    const output = (c: string) => this.buffer.push(c);
    super(input, output);
  }

  peek() {
    return this.buffer.join("");
  }
}
