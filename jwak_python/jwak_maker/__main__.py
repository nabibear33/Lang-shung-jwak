import sys
from .jwak_maker import str_to_jwak_file

def main():
    """터미널에서 -m 옵션을 사용하여 실행할 때 동작하는 함수"""
    if len(sys.argv) < 2:
        print("How to use: python -m jwak_python 'String you want to save'")
        sys.exit(1)

    input_text = " ".join(sys.argv[1:])
    saved_file = str_to_jwak_file(input_text)

if __name__ == "__main__":
    main()