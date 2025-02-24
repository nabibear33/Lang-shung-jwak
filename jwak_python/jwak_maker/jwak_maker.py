import os

def get_filepath(directory: str, filename: str) -> str:
    filepath = os.path.join(directory, filename)
    
    if not os.path.exists(filepath):
        return filepath

    base, ext = os.path.splitext(filename)
    counter = 2

    while os.path.exists(os.path.join(directory, f"{base}_{counter}{ext}")):
        counter += 1

    return os.path.join(directory, f"{base}_{counter}{ext}")

def str_to_jwak_file(sentence: str, directory: str = "example", filename: str = "output.jwak") -> None:
    result = """교주님
슝 좍
슈웅 좌악, 좌아아아악
슈우웅 슈웅, 슈웅
슈우우웅 슈우웅, 슈웅
슈우우우웅 슈우우웅, 슈웅
"""

    ascll_array = [i for i in map(ord, sentence)]
    for ascllNum in ascll_array:
        num_array = [int(n) for n in str(ascllNum)]
        result += "슈우우우우웅\n"
        for index, num in enumerate(num_array[::-1]):
            if num != 0:
                if index == 0:
                    result += "슈우우우우웅 좌" + "아" * num + "악; 좌악, 슝~ 슈우우우우웅\n"
                else:
                    result += "슈우우우우웅 좌" + "아" * num + "악; 좌악, 슈" + "우" * (index - 1) + "웅~ 슈우우우우웅\n"
        result += "비비따잇 ㅋㅋㅋㅋㅋㅋ\n"

    filepath = get_filepath(directory, filename)

    with open(filepath, "w", encoding="utf-8") as file:
        file.write(result)

    print(f"Code is saved at '{filepath}'")
