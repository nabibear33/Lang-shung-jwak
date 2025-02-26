import CodeMirror from "@uiw/react-codemirror";
import { githubDark } from "@uiw/codemirror-theme-github";
import { PeekIO, run } from "@jwak/core";
import { FC, useCallback, useState } from "react";
import { gugudan, fibbonacci, fizzbuzz, helloWorld, helloWorld2 } from "./code";
import { Controller, useForm } from "react-hook-form";
import * as styles from "./App.css";

interface Inputs {
  code: string;
  input: string;
}

const MAX_RUNTIME_CYCLE = 1_000_000;

export const App: FC = () => {
  const [output, setOutput] = useState("");
  const { control, handleSubmit, setValue } = useForm<Inputs>({
    defaultValues: { code: gugudan, input: "" },
  });

  const onSubmit = useCallback(({ code, input }: Inputs) => {
    let i = 0;

    const io = new PeekIO(() => input.split("\n").map((n) => +n)[i++]);
    try {
      run(code, io, MAX_RUNTIME_CYCLE);

      setOutput(io.peek());
    } catch (error) {
      if (error instanceof Error) {
        setOutput(io.peek() + "\nError:\n" + error.message);
      } else {
        setOutput(io.peek() + "\n" + "알 수 없는 에러");
      }
    }
  }, []);

  return (
    <form className={styles.container} onSubmit={handleSubmit(onSubmit)}>
      <h1 className={styles.title}>Lang Shung Jwak Playground</h1>
      <Controller
        name="code"
        control={control}
        render={({ field }) => (
          <CodeMirror
            maxHeight="400px"
            value={field.value}
            onChange={field.onChange}
            theme={githubDark}
            autoFocus
          />
        )}
      />

      <div>
        <div>샘플 코드</div>
        <div className={styles.buttonGroup}>
          <button
            type="button"
            className={styles.button}
            onClick={() => setValue("code", helloWorld)}
          >
            헬로월드
          </button>
          <button
            type="button"
            className={styles.button}
            onClick={() => setValue("code", helloWorld2)}
          >
            헬로월드 2
          </button>
          <button
            type="button"
            className={styles.button}
            onClick={() => setValue("code", fizzbuzz)}
          >
            피즈버즈
          </button>
          <button
            type="button"
            className={styles.button}
            onClick={() => {
              setValue("code", fibbonacci);
              setValue("input", "20");
            }}
          >
            피보나치
          </button>
          <button
            type="button"
            className={styles.button}
            onClick={() => setValue("code", gugudan)}
          >
            구구단
          </button>
        </div>
      </div>

      <div className={styles.footer}>
        <div className={styles.io}>
          <div className={styles.label}>in</div>
          <Controller
            name="input"
            control={control}
            render={({ field }) => (
              <CodeMirror
                placeholder="개행으로 구분된 숫자를 입력해주세요"
                value={field.value}
                onChange={field.onChange}
                theme={githubDark}
                autoFocus
              />
            )}
          />
        </div>

        <div className={styles.io}>
          <div className={styles.label}>out</div>
          <CodeMirror
            maxHeight="200px"
            value={output}
            theme={githubDark}
            editable={false}
            readOnly
            basicSetup={{ lineNumbers: false }}
          />
        </div>
      </div>
      <div>
        <button className={styles.button} type="submit">
          Run
        </button>
      </div>
    </form>
  );
};
