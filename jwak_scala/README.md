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

- 상대 경로로 주어진 파일을 실행합니다.

```sh
# 현재 경로
$ scala . -- "../example/hello_world.jwak"
Hello, world!

# 프로젝트 루트 경로
$ scala jwak_scala/ -- "example/hello_world.jwak"
Hello, world!
```

## 기타

- 오류 메시지 및 오류 처리가 제대로 되어 있지 않습니다. 추후 개선 예정입니다.
- `#` 를 사용한 주석을 지원합니다.
