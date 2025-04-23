# Scala-Jwak

- [랭슝좍](https://github.com/nabibear33/Lang-shung-jwak)의
  [scala 3](https://www.scala-lang.org/) 구현체입니다.
- 이 구현체는 파일 실행만을 지원합니다.
- 코드는 리눅스를 기준으로 작성되었습니다.

## 실행 방법

- scala-jwak을 실행하기 위해서는 `scala` 바이너리가 필요합니다.
- [scala 공식 홈페이지에서](https://www.scala-lang.org/download/) `scala`
  바이너리를 설치하실 수 있습니다.
- 다음 명령어로 `scala`가 올바르게 설치되었는지 확인합니다.

```sh
$ scala --version
Scala code runner version: 1.5.4
Scala version (default): 3.6.3
```

### `run`

상대 경로로 주어진 파일을 실행합니다.

```sh
# 현재 경로
$ scala . -- run "../example/hello_world.jwak"
Hello, world!

# 프로젝트 루트 경로
$ scala jwak_scala/ -- run "example/hello_world.jwak"
Hello, world!
```

### `jwak`

주어진 문장을 생성하는 jwak 코드를 생성합니다.

```sh
$ scala . -- jwak "리슝좍"
교주님
슝 좍
슈웅 좌악, 좌아아아악
...
```

### `eval`

jwak 코드를 인자로 받아 실행합니다.

```sh
$ scala . -- eval "$(scala . -- jwak 리슝좍)"
리슝좍
```

### `demo`

모든 예제 코드를 실행합니다.

```sh
$ scala . -- demo
```

## 기타

- 오류 메시지 및 오류 처리가 제대로 되어 있지 않습니다. 추후 개선 예정입니다.
- `#` 를 사용한 주석을 지원합니다.

## 패키징

[실행 파일을 패키징](https://scala-cli.virtuslab.org/docs/commands/package)하여 최대 [54배](#벤치마크) 더 빠르게 실행할 수 있습니다.

```sh
# JVM
scala --power package . -o jwak-scala.jar -f

# GraalVM
scala --power package . -o jwak-scala.graal --native-image -f

# Scala Native
scala --power package . -o jwak-scala.native --native -f
```

### 벤치마크

```sh
hyperfine --warmup 5 --export-markdown benchmark.md -N \
  './jwak-scala.native demo' \
  './jwak-scala.graal demo' \
  './jwak-scala.jar demo'
```

| Command                    |    Mean [ms] | Min [ms] | Max [ms] |     Relative |
| :------------------------- | -----------: | -------: | -------: | -----------: |
| `./jwak-scala.native demo` |    8.9 ± 0.4 |      8.2 |     10.9 |         1.00 |
| `./jwak-scala.graal demo`  |   13.5 ± 0.4 |     12.7 |     15.4 |  1.52 ± 0.08 |
| `./jwak-scala.jar demo`    | 488.5 ± 11.9 |    473.8 |    510.6 | 54.88 ± 2.56 |
