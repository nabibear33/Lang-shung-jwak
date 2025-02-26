import { fibbonacci, fizzbuzz, gugudan, helloWorld } from "./code";
import { PeekIO } from "./io";
import { run } from "./runtime";

const TICK_LIMIT = 1_000_000;

function demo(code: string, inputs: number[]) {
  let i = 0;
  const io = new PeekIO(() => {
    if (inputs.length < i) throw new RangeError();
    return inputs[i++];
  });

  try {
    run(code, io, TICK_LIMIT);

    console.log(io.peek());
  } catch (error) {
    console.error(error);
  }
}

demo(fibbonacci, [10]);
