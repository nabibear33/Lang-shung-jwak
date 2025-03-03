## 로컬 실행

1. 깃허브 파일을 다운받고 압축을 풀어주세요.
2. 터미널을 열고 루트 폴더에서 다음 명령어를 실행하세요.
```bash
python -m jwak_python .\example\hello_world.jwak
```

## 문장을 .jwak 코드 파일로 만들기
example 폴더 내 output.jwak 파일에 기록됩니다.
```bash
python -m jwak_python.jwak_maker "원하는 문장"
>> Code is saved at 'example\output.jwak'
```
코드 실행 결과
```bash
python -m jwak_python .\example\output.jwak
>> Hello, world!
```