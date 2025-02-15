import sys
import re

MAX_LINE_COUNT = 1000000
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
            raise SyntaxError('어떻게 이게 리슝좍이냐!')
        if match.group() == "슝":
            return 0
        return match.group().count("우") + 1

    def jwak_to_int(self, code):
        match = re.fullmatch(r"(좍|좌아*악)", code)
        if not match:
            raise SyntaxError('어떻게 이게 리슝좍이냐!')
        if match.group() == "좍":
            return 1
        return match.group().count("아") + 2

    def calculate(self, code):
        code = re.sub(r"ㅋ+", "", code)
        
        if code == '':
            return 0

        token_list = self.tokenize_formula(code)
        if len(token_list) % 2 != 1:
            raise SyntaxError('어떻게 이게 리슝좍이냐!')
        for index, token in enumerate(token_list):
            if index % 2 == 0:
                if not re.fullmatch(r"(좍|좌아*악|슝|슈우*웅)", token):
                    raise SyntaxError('어떻게 이게 리슝좍이냐!')
            if index % 2 == 1:
                if not re.fullmatch(r"(@+|,+|~+|;+)", token):
                    raise SyntaxError('어떻게 이게 리슝좍이냐!')

        result = 0
        if re.fullmatch(r"(좍|좌아*악)", token_list[0]):
            result = self.jwak_to_int(token_list[0])
        elif re.fullmatch(r"(슝|슈우*웅)", token_list[0]):
            result = self.var[self.shung_to_idx(token_list[0])]

        current_index = 1
        while current_index < len(token_list):
            operator = token_list[current_index]
            next_value_string = token_list[current_index + 1]
            next_value = None

            if re.fullmatch(r"(좍|좌아*악)", next_value_string):
                next_value = self.jwak_to_int(next_value_string)
            elif re.fullmatch(r"(슝|슈우*웅)", next_value_string):
                next_value = self.var[self.shung_to_idx(next_value_string)]

            if re.fullmatch(r"~+", operator):
                result += next_value
            elif re.fullmatch(r";+", operator):
                result -= next_value
            elif re.fullmatch(r",+", operator):
                result *= next_value
            elif re.fullmatch(r"@+", operator):
                result //= next_value

            current_index += 2

        return result

    def condition_assign(self, code):
        match = re.match(r"(슝|슈우*웅)", code)
        if not match:
            return False
        
        return True

    def condition_print(self, code):
        if not code.startswith("비비"):
            return False

        code = re.sub(r"비비", "", code, count=1)
        code = re.sub(r"따+잇", "", code, count=1)

        if code.count("보호막") > 1:
            return False

        code = re.sub(r"보호막", "", code, count=1)

        if not re.fullmatch(r"ㅋ*", code):
            return False

        return True
    
    def condition_input(self, code):
        if not code.startswith("순수"):
            return False

        code = re.sub(r"순수", "", code, count=1)
        code = re.sub(r"따+잇", "", code, count=1)

        if not re.fullmatch(r"ㅋ*", code):
            return False

        return True
    
    def condition_if(self, code):
        if not "하는재미" in code:
            return False
        return True

    def condition_goto(self, code):
        if not code.startswith("에잇"):
            return False
        
        code = re.sub(r"에잇", "", code, count=2)
        if not re.fullmatch(r"ㅋ+", code):
            return False

        return True


    def type(self, code):
        if code == '':
            return None
        
        if self.condition_if(code):
            return 'IF'
        elif self.condition_assign(code):
            return 'ASSIGN'
        elif self.condition_print(code):
            return 'PRINT'
        elif self.condition_input(code):
            return 'INPUT'
        elif self.condition_goto(code):
            return 'GOTO'

        raise SyntaxError('어떻게 이게 리슝좍이냐!')

    def compileLine(self, code):
        TYPE = self.type(code)

        if TYPE == 'ASSIGN':
            index = self.shung_to_idx(code)
            code = re.sub(r"(슝|슈우*웅)", "", code, count=1)
            result = self.calculate(code)
            self.var[index] = result

        elif TYPE == 'PRINT':
            if code.count("ㅋ") == 0:
                raise SyntaxError('어떻게 이게 리슝좍이냐!')
            index = code.count("ㅋ") - 1
            if "보호막" in code:
                print(self.var[index], end="")
            else:
                print(chr(self.var[index]), end="")

        elif TYPE == 'INPUT':
            if code.count("ㅋ") == 0:
                raise SyntaxError('어떻게 이게 리슝좍이냐!')
            index = code.count("ㅋ") - 1
            value = int(input())
            self.var[index] = value

        elif TYPE == 'IF':
            command_string, condition_string = code.split("하는재미")
            value = self.calculate(condition_string)
            if value == 0:
                return self.compileLine(command_string)

        elif TYPE == 'GOTO':
            if code.count("에잇") == 1:
                return -code.count("ㅋ")
            elif code.count("에잇") == 2:
                return code.count("ㅋ")
        
        return

    def compile(self, code, check=True):
        code = code.replace(" ", "").replace("?", "").replace(".", "").replace("!", "")
        self.codeline = code.rstrip().split('\n')

        if not self.codeline[0] == '교주님':
            self.print_shungjwak()
            raise SyntaxError('어떻게 이게 리슝좍이냐!')
        
        index = 1
        line_count = 0
        while index < len(self.codeline):
            line_count += 1
            if line_count == MAX_LINE_COUNT:
                raise RecursionError('Line ' + str(index + 1) + '에서 타임 패러독스!')

            move_line = self.compileLine(self.codeline[index])

            if move_line is not None:
                index += move_line
            else:
                index += 1

    def compilePath(self, path):
        with open(path) as file:
            code = ''.join(file.readlines())
            self.compile(code)