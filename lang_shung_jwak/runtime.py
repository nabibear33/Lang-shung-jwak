import sys
import re

MAX_LOOP_COUNT = 10000000
MAX_VARIABLE_SIZE = 256

class Lang_shung_jwak:
    def __init__(self):
        self.var = [0] * MAX_VARIABLE_SIZE  # list of variable
        self.codeline = []  # list of each codeline

    @staticmethod
    def print_shungjwak():
        print('\n')
        return

    def tokenize_formula(self, code):
        pattern = r"(좍|좌아*악|,+|~+|;+|@+|슝|슈우*웅)"
        tokens = re.findall(pattern, code)
        return tokens

    def shung_to_idx(self, code):
        match = re.match(r"(슝|슈우*웅)", code)
        if not match:
            return None
        if match.group() == "슝":
            return 0
        return code.count("우") + 1

    def jwak_to_int(self, code):
        match = re.fullmatch(r"(좍|좌아*악)", code)
        if not match:
            return None
        if match.group() == "좍":
            return 1
        return code.count("아") + 2

    def calculate(self, code):
        token_list = self.tokenize_formula(code)

        result = 0
        if re.fullmatch(r"(좍|좌아*악)", token_list[1]):
            result = self.jwak_to_int(token_list[1])
        elif re.fullmatch(r"(슝|슈우*웅)", token_list[1]):
            result = self.var[self.shung_to_idx(token_list[1])]

        current_index = 2
        while current_index < len(token_list) - 1:
            operator = token_list[current_index]
            next_value_string = token_list[current_index + 1]
            next_value = None

            if re.fullmatch(r"(좍|좌아*악)", next_value_string):
                next_value = self.jwak_to_int(next_value_string)
            elif re.fullmatch(r"(슝|슈우*웅)", next_value_string):
                next_value = self.var[self.shung_to_idx(next_value_string)]

            if next_value is None:
                return None

            if operator[0] == "~":
                result += next_value
            elif operator[0] == ";":
                result -= next_value
            elif operator[0] == ",":
                result *= next_value
            elif operator[0] == "@":
                result /= next_value
            else:
                return None

            current_index += 2

        return result

    def condition_assign(self, code):
        match1 = re.match(r"(슝|슈우*웅)", code)
        if not match1:
            return False
        
        return True

    def condition_print(self, code):
        if not code.startswith("비비") and not code.startswith("순수"):
            return False

        if not re.search(r"따+잇", code):
            return False
        modified_s = re.sub(r"따+잇", "", code)
        if "따" in modified_s:
            return False

        allowed_chars = {"따", "잇", "ㅋ"}
        if not all(char in allowed_chars for char in code[2:]):    
            return False

        return True

    def type(self, code):
        if code == '':
            return None
        
        if self.condition_assign(code):
            return 'ASSIGN'
        elif self.condition_print(code):
            return 'PRINT'
        
        raise SyntaxError('어떻게 이게 리슝좍이냐!')

    def compileLine(self, code):
        TYPE = self.type(code)

        if TYPE == 'ASSIGN':
            index = self.shung_to_idx(code)
            result = self.calculate(code)
            self.var[index] = result

        elif TYPE == 'PRINT':
            index = code.count("ㅋ")

            if code.startswith("비비"):
                print(chr(self.var[index]), end="")
            elif code.startswith("순수"):
                print(self.var[index], end="")

        return

    def compile(self, code, check=True):
        code = code.replace(" ", "").replace("?", "").replace(".", "").replace("!", "")
        self.codeline = code.rstrip().split('\n')

        if not self.codeline[0] == '교주님':
            self.print_shungjwak()
            raise SyntaxError('어떻게 이게 리슝좍이냐!')
        
        index = 1
        loop_count = 0
        while index < len(self.codeline):
            self.compileLine(self.codeline[index])

            loop_count += 1
            if loop_count == MAX_LOOP_COUNT:
                raise RecursionError('Line ' + str(index + 1) + '에서 타임 패러독스!')
            index += 1

    def compilePath(self, path):
        with open(path) as file:
            code = ''.join(file.readlines())
            self.compile(code)