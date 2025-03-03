# jwak_rust

- [랭슝좍](https://github.com/nabibear33/Lang-shung-jwak)의 Rust 구현체입니다.
- 이 구현체는 파일 실행과 라이브러리 사용을 지원합니다.
- 코드는 윈도우를 기준으로 작성되었습니다.

## 실행 방법.
- jwak_rust를 실행하기 위해서는 rust와 cargo가 필요합니다.

- [rust](https://www.rust-lang.org/learn/get-started) 공식 홈페이지로 가서 설치하실수 있습니다.

- 다음 명령어로 rust와 cargo가 올바르게 설치되었는지 확인합니다.
```ps
PS Lang-shung-jwak\jwak_rust> rustc --version
rustc 1.86.0-nightly (9cd60bd2c 2025-02-15)

PS Lang-shung-jwak\jwak_rust> cargo --version
cargo 1.86.0-nightly (ce948f461 2025-02-14)
```

- 상대 경로로 주어진 파일을 실행합니다.
```ps
# 현재 경로.
PS Lang-shung-jwak\jwak_rust> cargo run "../example/hello_world.jwak"
Hello, world!
```
- Cargo.toml에 라이브러리로 사용할수 있습니다.

```toml
[dependencies]
jwak_rust = {git = "https://github.com/nabibear33/Lang-shung-jwak"}
```
코드로써 사용
```rs
fn main() {
    /// ... code를 불러오는 코드
    let mut interpreter = LangShungJwak::new();
    let _ = interpreter.compile(code);
}
```

## 기타
- 파이썬 구현체를 기반으로 오류 처리, 구현 하였기에 두 언어간의 차이나 버그가 있을수 있습니다!