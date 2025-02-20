# LANG-SHUNG-JWAK-C

[랭슝좍](https://github.com/nabibear33/Lang-shung-jwak)의 c언어 구현체입니다. 언어에 대한 설명은 원본 레포의 README.md를 참고해 주세요.

속도 자체는 파이썬 구현체보다 빠르나 문자열 출력 속도는 파이썬 구현체보다 느립니다.

코드는 윈도우를 기준으로 작성되었습니다.

## 빌드

빌드를 하기 위해선 make가 설치되어 있어야 합니다.

### Jwak 빌드

```
make main
```

### test 빌드

```
make test
```

## 사용

이 구현체는 REPL과 파일 실행을 지원합니다.

### REPL

```
jwak
```

### 파일

```
jwak 파일경로
```

## 주의

 - ASTNode는 내부적으로 동적 할당을 합니다. ASTSeq와 ASTNode는 소멸자가 없으니 사용 후 반드시 `freeAST`나 `freeASTNode`를 해서 메모리 해제를 하세요.
 - 파싱이나 바이트 코드로 컴파일을 하던 중 오류가 날 시 자동으로 `freeAST`를 호출해 주니 오류가 났을 땐 호출을 안 해줘도 괜찮습니다.

## 참고 사항

 - 파일 인코딩은 ANSI가 아닌 UTF-8로 되어 있어야 합니다.
 - 문자 출력 전 `setlocale(LC_ALL, "");`을 실행해야 한글이 출력되며 main 함수에서 한 번만 호출하면 됩니다.
 - `std::wcout.imbue(std::locale(""));` 사용 시 런타임 오류가 나는 문제가 있습니다. 한글 출력 시 `wcout` 대신 `wprintf`를 사용해 주시기 바랍니다.
