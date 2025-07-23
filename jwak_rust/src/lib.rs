use std::isize;
use std::{fmt};
use std::error::Error as StdError;
use std::collections::VecDeque;


const MAX_LINE_COUNT: usize = 1000000;
const MAX_VARIABLE_SIZE: usize = 256;

#[derive(Debug)]
pub enum CommandType {
    IF,
    ASSIGN,
    PRINT,
    INPUT,
    GOTO,
    NONE
}

enum Op {
    Add,
    Sub,
    Mul,
    Div,
}

enum Value {
    Imm(isize),
    Var(usize),
}

#[derive(Debug)]
pub struct IOError;

#[derive(Debug)]
pub struct ZeroDivisionError;

#[derive(Debug)]
pub struct SyntaxError;

#[derive(Debug)]
pub struct RecursionError {
    index: usize
}

impl fmt::Display for SyntaxError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "SyntaxError")
    }
}

impl fmt::Display for ZeroDivisionError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "ZeroDivisionError")
    }
}

impl fmt::Display for RecursionError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "RecursionError")
    }
}

impl fmt::Display for IOError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "RecursionError")
    }
}

#[derive(Debug)]
pub enum Error {
    SyntaxError,
    ZeroDivisionError,
    RecursionError(RecursionError),
    IOError
}

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Error::SyntaxError => write!(f, "SyntaxError"),
            Error::ZeroDivisionError => write!(f, "ZeroDivisionError"),
            Error::RecursionError(e) => write!(f, "{}", e),
            Error::IOError => write!(f, "IOError")
        }
    }
}

impl StdError for Error {}

impl Drop for Error {
    fn drop(&mut self) {
        let error = match self {
            Error::RecursionError(e) => format!("{}: Line {} 에서 타임 패러독스!", e, e.index+1),
            _ => format!("{}: 어떻게 이게 리슝좍이냐!", self)
        };
        panic!("{}", error);
    }
}

pub struct LangShungJwak {
    var: Vec<isize>,
    codeline: Vec<String>,
    input_queue: VecDeque<isize>
}
impl LangShungJwak {
    pub fn new() -> Self {
        LangShungJwak {
            var: vec![0; MAX_VARIABLE_SIZE],
            codeline: Vec::new(),
            input_queue: VecDeque::new()
        }
    }

    fn check_kcount(&self, code: &str) -> bool {
        code.chars().all(|c| c == 'ㅋ')
    }

    fn condition_print(&self, code: &str) -> bool {
        let Some(processed_code) = code.strip_prefix("비비") else {
            return false;
        };

        let mut result = String::with_capacity(processed_code.len());
        let mut chars = processed_code.chars().peekable();

        let mut pattern_removed = false;

        while let Some(c) = chars.next() {
            if !pattern_removed && c == '따' {
                let mut dda_count = 1;
                while let Some(&'따') = chars.peek() {
                    chars.next();
                    dda_count += 1;
                }

                if let Some(&'잇') = chars.peek() {
                    chars.next(); 
                    pattern_removed = true;
                    continue;
                } else {
                    for _ in 0..dda_count {
                        result.push('따');
                    }
                }
            } else {
                result.push(c);
            }
        }

        if result.matches("보호막").count() > 1 {
            return false;
        }

        result = result.replacen("보호막", "", 1);

        self.check_kcount(&result)
    }

    fn condition_input(&self, code: &str) -> bool {
        let Some(processed) = code.strip_prefix("순수") else {
            return false;
        };

        let mut cleaned = processed.to_string();
        let mut search_from = 0;

        
        while let Some(relative_start) = cleaned[search_from..].find("따") {
            let start = search_from + relative_start;
        
            let mut end_of_ddas = start + "따".len();
            while cleaned.get(end_of_ddas..).map_or(false, |s| s.starts_with("따")) {
                end_of_ddas += "따".len();
            }

            if cleaned.get(end_of_ddas..).map_or(false, |s| s.starts_with("잇")) {
                let end_of_pattern = end_of_ddas + "잇".len();
                cleaned.replace_range(start..end_of_pattern, "");   
                search_from = start;
            } else {
                search_from = start + "따".len();
            }
        }

        self.check_kcount(&cleaned)
    }

    fn condition_goto(&self, code: &str) -> bool {
        if !code.starts_with("에잇") {
            return false;
        }

        let modified_code = code.replacen("에잇", "", 2);
        !modified_code.is_empty() && self.check_kcount(&modified_code)
    }

    fn is_shung(&self, code: &str) -> bool {
        if code.starts_with("슝") {
            return true;
        }
        if code.starts_with("슈") {
            if let Some(wung_pos) = code.find("웅") {
                let middle = &code["슈".len()..wung_pos];
                return middle.chars().all(|c| c == '우');
            }
        }
        false
    }

    fn type_command(&self, code: &str) -> Result<CommandType, Error> {
        if code.is_empty() {
            return Ok(CommandType::NONE);
        }

        if code.contains("하는재미") {
            return Ok(CommandType::IF);
        }

        if self.is_shung(code) {
            return Ok(CommandType::ASSIGN);
        }

        if self.condition_print(code) {
            return Ok(CommandType::PRINT);
        }

        if self.condition_input(code) {
            return Ok(CommandType::INPUT);
        }

        if self.condition_goto(code) {
            return Ok(CommandType::GOTO);
        }
        
        Err(Error::SyntaxError)
    }

    fn parse_shuwung_at(&self, slice: &str) -> Option<(usize, usize)> {
        if !slice.starts_with("슈") {
            return None;
        }
    
        if let Some(wung_pos_in_suffix) = slice["슈".len()..].find("웅") {
            let wung_pos_in_slice = "슈".len() + wung_pos_in_suffix;
            let middle = &slice["슈".len()..wung_pos_in_slice];

            if middle.chars().all(|c| c == '우') {
                let w_count = middle.chars().count();
                let value = w_count + 1;
                let token_len = wung_pos_in_slice + "웅".len();
                Some((token_len, value))
            } else {
                None
            }
        } else {
            None
        }
    }

    fn shung_to_idx(&self, code: &str) -> Result<usize, Error> {
        let mut i = 0;
        while i < code.len() {
            let remaining = &code[i..];
        
            if remaining.starts_with("슝") {
                return Ok(0);
            }
        
            if let Some((_token_len, value)) = self.parse_shuwung_at(remaining) {
                return Ok(value);
            }
        
            if let Some(c) = remaining.chars().next() {
                i += c.len_utf8();
            } else {
                break;
            }
        }

        Err(Error::SyntaxError)
    }

    fn tokenize_formula(&self, code: &str) -> Result<Vec<String>, Error> {
        let mut tokens = Vec::new();
        let mut rest = String::new();

        let chars: Vec<char> = code.chars().collect();
        let mut i = 0;

        while i < chars.len() {
            let current_char = chars[i];
            let remaining_slice: String = chars[i..].iter().collect();
    
            if remaining_slice.starts_with('좌') {
                let mut j = i + 1;
                while j < chars.len() && chars[j] == '아' {
                    j += 1;
                }
                if j < chars.len() && chars[j] == '악' {
                    tokens.push(chars[i..=j].iter().collect());
                    i = j + 1;
                    continue;
                }
            }

            if remaining_slice.starts_with('슈') {
                let mut j = i + 1;
                while j < chars.len() && chars[j] == '우' {
                    j += 1;
                }
                if j < chars.len() && chars[j] == '웅' {
                    tokens.push(chars[i..=j].iter().collect());
                    i = j + 1;
                    continue;
                }
            }
            
            if matches!(current_char, ',' | '~' | ';' | '@') {
                let mut j = i + 1;
                while j < chars.len() && chars[j] == current_char {
                    j += 1;
                }
                tokens.push(chars[i..j].iter().collect());
                i = j;
                continue;
            }
 
            if matches!(current_char, '좍' | '슝') {
                tokens.push(current_char.to_string());
                i += 1;
                continue;
            }
        
            rest.push(current_char);
            i += 1;
        }

        if !rest.trim().is_empty() {
            Err(Error::SyntaxError)
        } else {
            Ok(tokens)
        }
    }

    fn jwak_to_int(&self, code: &str) -> Result<usize, Error> {
        if let Some(_index) = code.find("좍") {
            return Ok(1);
        }

        if let Some(index) = code.find("좌") {
            let mut chars = code[index..].chars();
            chars.next();
            
            let mut a_count = 0;
            let mut remaining_slice = chars.as_str();
            
            while remaining_slice.starts_with('아') {
                a_count += 1;
                remaining_slice = &remaining_slice['아'.len_utf8()..];
            }
            
            if remaining_slice.starts_with('악') {
                return Ok(a_count + 2);
            }
        }
        Err(Error::SyntaxError)
    }
    
    
    fn parse_value(&self, token: &str) -> Result<Value, Error> {
        if token == "좍" || token.starts_with("좌") {
            let imm = self.jwak_to_int(token)? as isize;
            Ok(Value::Imm(imm))
        } else if token == "슝" || token.starts_with("슈") {
            let idx = self.shung_to_idx(token)?;
            Ok(Value::Var(idx))
        } else {
            Err(Error::SyntaxError)
        }
    }
    
    fn parse_op(&self, token: &str) -> Result<Op, Error> {
        match token.chars().next() {
            Some('~') => Ok(Op::Add),
            Some(';') => Ok(Op::Sub),
            Some(',') => Ok(Op::Mul),
            Some('@') => Ok(Op::Div),
            _ => Err(Error::SyntaxError),
        }
    }

    fn calculate(&self, code: &str) -> Result<isize, Error> {
        if code.is_empty() {
            return Ok(0);
        }

        let tokens = self.tokenize_formula(code)?;
        if tokens.len() % 2 != 1 {
            return Err(Error::SyntaxError);
        }

        let mut result: isize = match self.parse_value(&tokens[0])? {
            Value::Imm(n) => n,
            Value::Var(i) => self.var[i],
        };
        
        let mut i = 1;
        while i < tokens.len() {
            let op = self.parse_op(&tokens[i])?;
            let val = match self.parse_value(&tokens[i + 1])? {
                Value::Imm(n) => n,
                Value::Var(idx) => self.var[idx],
            };

            result = match op {
                Op::Add => result + val,
                Op::Sub => result - val,
                Op::Mul => result * val,
                Op::Div => {
                    if val == 0 {
                        return Err(Error::ZeroDivisionError);
                    }
                    result / val
                }
            };

            i += 2;
        }
        
        Ok(result)
    }


    fn compile_line(&mut self, code: &str) -> Result<isize, Error> {
        let code_type = self.type_command(code)?;
        match code_type {
            CommandType::ASSIGN => {
                let index = self.shung_to_idx(code)?;
                let first_token_len = if code.starts_with("슝") {
                    "슝".len()
                } else {
                    code.find("웅").map_or(0, |pos| pos + "웅".len())
                };

                if first_token_len == 0 { return Err(Error::SyntaxError); }
    
                let formula_part = &code[first_token_len..];
                let result = self.calculate(formula_part)?;
                self.var[index] = result;
            }
            CommandType::IF => {
                if let Some((command_string, condition_string)) = code.split_once("하는재미") {
                    let value = self.calculate(condition_string)?;
                    if value == 0 {
                        return Ok(self.compile_line(command_string)?);
                    }
                }
            }
            CommandType::PRINT => {
                let kcount = code.chars().filter(|&c| c == 'ㅋ').count();
                if kcount == 0 {
                    return Err(Error::SyntaxError);
                }
                
                let index = kcount - 1;
                let value = self.var[index];
                if code.contains("보호막") {
                    print!("{}", &value.to_string());
                } else {

                    match char::from_u32(value as u32) {
                        Some(ch) => print!("{}", ch),
                        None => return Err(Error::SyntaxError), 
                    }
                }
            }

            CommandType::INPUT => {
                let kcount = code.chars().filter(|&c| c == 'ㅋ').count();
                if kcount == 0 {
                    return Err(Error::SyntaxError);
                }

                let index = kcount - 1;
                
                if self.input_queue.is_empty() {
                    let mut input_text = String::new();
                    std::io::stdin().read_line(&mut input_text)
                        .map_err(|_| Error::IOError)?;

                    for v in input_text.trim_end().split_ascii_whitespace() {
                        if let Ok(num) = v.parse::<isize>() {
                            self.input_queue.push_back(num); // push_back 사용
                        } else {
                            v.chars().for_each(|c| self.input_queue.push_back(c as isize));
                        }
                    }
                }

                if let Some(value) = self.input_queue.pop_front() {
                    self.var[index] = value;
                } else {
                    return Err(Error::SyntaxError);
                }
            }

            CommandType::GOTO => {
                let eitcount = code.matches("에잇").count();
                let kcount = code.matches("ㅋ").count() as isize;
                
                return match eitcount {
                    1 => Ok(-kcount),
                    2 => Ok(kcount),
                    _ => Err(Error::SyntaxError),
                };
            }

            CommandType::NONE => {}
        }

        Ok(0)
    }
    
    pub fn compile(&mut self, code: &str) -> Result<(), Error> {
        let processed_code = code.replace(" ", "")
                                        .replace("?", "")
                                        .replace(".", "")
                                        .replace("!", "");
        self.codeline = processed_code.lines()
                            .map(|s| s.trim().to_string()).collect();
        
        if let Some(first_line) = self.codeline.get(0) {
            if first_line != "교주님" {
                eprintln!("Line 1\t{}\t<-", first_line);
                return Err(Error::SyntaxError)
            }
        }

        let mut index = 1;
        let mut line_count = 0;
        while index < self.codeline.len() {
            line_count += 1;
            if line_count == MAX_LINE_COUNT{
                return Err(Error::RecursionError(RecursionError{ index }))
            }
            match self.compile_line(&self.codeline[index].clone()) {
                Ok(move_line) => {
                    index = (index as isize + if move_line == 0 { 1 } else { move_line }) as usize;
                }
                Err(_e) => {
                    eprintln!("Line {}\t{}\t<-", index+1, self.codeline[index]);
                    return Err(Error::SyntaxError)
                }
            }
        }
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use crate::LangShungJwak;

    #[test]
    fn test_print_jwak() {
        let code = "교주님
슝 좍
슈웅 좌악, 좌아아아악
슈우웅 슈웅, 슈웅
슈우우웅 슈우웅, 슈웅
슈우우우웅 슈우우웅, 슈웅
슈우우우우웅
슈우우우우웅 좌아아아악; 좌악, 슝~ 슈우우우우웅
슈우우우우웅 좌아아아아아아아악; 좌악, 슈웅~ 슈우우우우웅
비비따잇 ㅋㅋㅋㅋㅋㅋ
슈우우우우웅
슈우우우우웅 좌아아악; 좌악, 슝~ 슈우우우우웅
슈우우우우웅 좌아아아악; 좌악, 슈웅~ 슈우우우우웅
비비따잇 ㅋㅋㅋㅋㅋㅋ
슈우우우우웅
슈우우우우웅 좌아아아아아아아아악; 좌악, 슝~ 슈우우우우웅
슈우우우우웅 좌아악; 좌악, 슈우웅~ 슈우우우우웅
비비따잇 ㅋㅋㅋㅋㅋㅋ
슈우우우우웅
슈우우우우웅 좌아악; 좌악, 슝~ 슈우우우우웅
슈우우우우웅 좌아악; 좌악, 슈웅~ 슈우우우우웅
슈우우우우웅 좌아악; 좌악, 슈우웅~ 슈우우우우웅
비비따잇 ㅋㅋㅋㅋㅋㅋ
슈우우우우웅
슈우우우우웅 좌아아아아아아아아악; 좌악, 슝~ 슈우우우우웅
슈우우우우웅 좌아악; 좌악, 슈웅~ 슈우우우우웅
슈우우우우웅 좌아악; 좌악, 슈우웅~ 슈우우우우웅
비비따잇 ㅋㅋㅋㅋㅋㅋ
슈우우우우웅
슈우우우우웅 좌아악; 좌악, 슝~ 슈우우우우웅
슈우우우우웅 좌아악; 좌악, 슈우웅~ 슈우우우우웅
비비따잇 ㅋㅋㅋㅋㅋㅋ
슈우우우우웅
슈우우우우웅 좌아아악; 좌악, 슝~ 슈우우우우웅
슈우우우우웅 좌아아아악; 좌악, 슈웅~ 슈우우우우웅
비비따잇 ㅋㅋㅋㅋㅋㅋ
슈우우우우웅
슈우우우우웅 좌아아아아악; 좌악, 슝~ 슈우우우우웅
슈우우우우웅 좌아악; 좌악, 슈웅~ 슈우우우우웅
슈우우우우웅 좌아악; 좌악, 슈우웅~ 슈우우우우웅
비비따잇 ㅋㅋㅋㅋㅋㅋ
슈우우우우웅
슈우우우우웅 좌아아아아아아아악; 좌악, 슝~ 슈우우우우웅
슈우우우우웅 좌아아아아아아아아아악; 좌악, 슈웅~ 슈우우우우웅
비비따잇 ㅋㅋㅋㅋㅋㅋ
슈우우우우웅
슈우우우우웅 좌아아아아아아아아악; 좌악, 슝~ 슈우우우우웅
슈우우우우웅 좌아아아아아아아아아악; 좌악, 슈웅~ 슈우우우우웅
비비따잇 ㅋㅋㅋㅋㅋㅋ
슈우우우우웅
슈우우우우웅 좌아아아아아아아아악; 좌악, 슝~ 슈우우우우웅
슈우우우우웅 좌아아아아아아아아아악; 좌악, 슈웅~ 슈우우우우웅
비비따잇 ㅋㅋㅋㅋㅋㅋ
슈우우우우웅
슈우우우우웅 좌아아아아아악; 좌악, 슝~ 슈우우우우웅
슈우우우우웅 좌아악; 좌악, 슈우웅~ 슈우우우우웅
비비따잇 ㅋㅋㅋㅋㅋㅋ
슈우우우우웅
슈우우우우웅 좌아아아아아아악; 좌악, 슝~ 슈우우우우웅
슈우우우우웅 좌아악; 좌악, 슈웅~ 슈우우우우웅
슈우우우우웅 좌아악; 좌악, 슈우웅~ 슈우우우우웅
비비따잇 ㅋㅋㅋㅋㅋㅋ
슈우우우우웅
슈우우우우웅 좌아아아아아악; 좌악, 슝~ 슈우우우우웅
슈우우우우웅 좌아악; 좌악, 슈웅~ 슈우우우우웅
슈우우우우웅 좌아악; 좌악, 슈우웅~ 슈우우우우웅
비비따잇 ㅋㅋㅋㅋㅋㅋ
슈우우우우웅
슈우우우우웅 좌아아아아아아악; 좌악, 슝~ 슈우우우우웅
슈우우우우웅 좌아아아아악; 좌악, 슈웅~ 슈우우우우웅
비비따잇 ㅋㅋㅋㅋㅋㅋ
슈우우우우웅
슈우우우우웅 좌아아악; 좌악, 슝~ 슈우우우우웅
슈우우우우웅 좌아아아악; 좌악, 슈웅~ 슈우우우우웅
비비따잇 ㅋㅋㅋㅋㅋㅋ
슈우우우우웅
슈우우우우웅 좌아아악; 좌악, 슝~ 슈우우우우웅
슈우우우우웅 좌아아아아아아아악; 좌악, 슈웅~ 슈우우우우웅
비비따잇 ㅋㅋㅋㅋㅋㅋ
슈우우우우웅
슈우우우우웅 좌아악; 좌악, 슝~ 슈우우우우웅
슈우우우우웅 좌아악; 좌악, 슈우웅~ 슈우우우우웅
비비따잇 ㅋㅋㅋㅋㅋㅋ
슈우우우우웅
슈우우우우웅 좌아아아아악; 좌악, 슝~ 슈우우우우웅
슈우우우우웅 좌아악; 좌악, 슈우웅~ 슈우우우우웅
비비따잇 ㅋㅋㅋㅋㅋㅋ
슈우우우우웅
슈우우우우웅 좌아악; 좌악, 슝~ 슈우우우우웅
슈우우우우웅 좌아악; 좌악, 슈우웅~ 슈우우우우웅
비비따잇 ㅋㅋㅋㅋㅋㅋ
슈우우우우웅";
        let mut interpreter = LangShungJwak::new();
        let _ = interpreter.compile(code);
    }
}