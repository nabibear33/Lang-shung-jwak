import sys
from .runtime import Lang_shung_jwak

def main():
    if len(sys.argv) != 2:
        print("How to use: python -m lang_shung_jwak <filename.jwak>")
        sys.exit(1)

    filename = sys.argv[1]
    try:
        with open(filename, "r", encoding="UTF-8") as file:
            code = file.read()
    except:
        print('FileNotFoundError: 어떻게 이게 리슝좍이냐!')
        return

    interpreter = Lang_shung_jwak()
    interpreter.compile(code)

if __name__ == "__main__":
    main()
