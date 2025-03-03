use std::isize;
use std::{fmt, io, ops::Rem};
use std::error::Error as StdError;
use regex::Regex;
use lazy_static::lazy_static;


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

impl StdError for SyntaxError {}
impl StdError for ZeroDivisionError {}
impl StdError for RecursionError {}

#[derive(Debug)]
pub enum Error {
    SyntaxError,
    ZeroDivisionError,
    RecursionError(RecursionError)
}

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Error::SyntaxError => write!(f, "SyntaxError"),
            Error::ZeroDivisionError => write!(f, "ZeroDivisionError"),
            Error::RecursionError(e) => write!(f, "{}", e),
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
    input_queue: Vec<isize>
}

lazy_static! {
    static ref RE_SHUNG: Regex = Regex::new(r"(슝|슈우*웅)").unwrap();
    static ref RE_JWAK: Regex = Regex::new(r"(좍|좌아*악)").unwrap();
}

impl LangShungJwak {
    pub fn new() -> Self {
        LangShungJwak {
            var: vec![0; MAX_VARIABLE_SIZE],
            codeline: Vec::new(),
            input_queue: Vec::new()
        }
    }

    fn check_kcount(&self, code: &str) -> bool {
        lazy_static! {
            static ref K_RE: Regex = Regex::new(r"^ㅋ*$").unwrap();
        }

        if !K_RE.is_match(&code) {
            return false;
        }

        true
    }

    fn condition_print(&self, code: &str) -> bool {
        if !code.starts_with("비비") {
            return false;
        }

        lazy_static! {
            static ref RE_DDAIT: Regex = Regex::new(r"(따+잇)").unwrap();
        }
        
        let mut processed_code = code.replacen("비비", "", 1);

        let binding = RE_DDAIT.replacen(&processed_code, 1, "");
        processed_code = binding.as_ref().to_owned();

        if processed_code.matches("보호막").count() > 1 {
            return false;
        }

        processed_code = processed_code.replacen("보호막", "", 1);

        self.check_kcount(&processed_code)
    }

    fn condition_input(&self, code: &str) -> bool {
        if !code.starts_with("순수") {
            return false;
        }

        lazy_static! {
            static ref SUNSUDDAIT_RE: Regex = Regex::new(r"(순수|따+잇)").unwrap();
        }
        let code = SUNSUDDAIT_RE.replace_all(code, "");
    
        return self.check_kcount(&code);
    }

    fn condition_goto(&self, code: &str) -> bool {
        if !code.starts_with("에잇") {
            return false;
        }
        
        let modified_code = code.replacen("에잇", "", 2);

        lazy_static! {
            static ref KS_RE: Regex = Regex::new(r"^ㅋ+$").unwrap();
        }
        
        KS_RE.is_match(&modified_code)
    }

    fn type_command(&self, code: &str) -> Result<CommandType, Error> {
        if code.is_empty() {
            return Ok(CommandType::NONE)
        }

        if code.contains("하는재미") {
            return Ok(CommandType::IF)
        } else if RE_SHUNG.is_match(code) {
            return Ok(CommandType::ASSIGN)
        } else if self.condition_print(code) {
            return Ok(CommandType::PRINT)
        } else if self.condition_input(code) {
            return Ok(CommandType::INPUT)
        } else if self.condition_goto(code) {
            return Ok(CommandType::GOTO)
        }
        
        Err(Error::SyntaxError)
    }

    fn shung_to_idx(&self, code: &str) -> Result<usize, Error> {
        if let Some(captures) = RE_SHUNG.captures(code) {
            captures.get(0).map_or_else(
                || Err(Error::SyntaxError),
                |group| {
                    let matches_str = group.as_str();
                    if matches_str == "슝" {
                        Ok(0)
                    } else {
                        let count_w = matches_str.matches("우").count();
                        Ok(count_w + 1)
                    }
                }
            )
        } else {
            Err(Error::SyntaxError)
        }
    }

    fn tokenize_formula(&self, code: &str) -> Result<Vec<String>, Error> {
        lazy_static! {
            static ref RE_TOKENIZE: Regex = Regex::new(r"(좍|좌아*악|,+|~+|;+|@+|슝|슈우*웅)").unwrap();
        }

        let tokens = RE_TOKENIZE.find_iter(code)
            .map(|mat| mat.as_str().to_string())
            .collect();
        
        let code = RE_TOKENIZE.replace_all(code ,"");

        if !code.trim().is_empty() {
            Err(Error::SyntaxError)
        } else {
            Ok(tokens)
        }
    }

    fn calculate(&self, code: &str) -> Result<isize, Error> {
        if code.is_empty() {
            return Ok(0);
        }

        let token_list = self.tokenize_formula(code)?;

        if token_list.len().rem(2) != 1 {
            return Err(Error::SyntaxError)
        }

        for (index, token) in token_list.iter().enumerate() {
            if index % 2 == 0 {
                lazy_static! {
                    static ref A_RE:Regex = Regex::new(r"(좍|좌아*악|슝|슈우*웅)").unwrap();
                }

                if !A_RE.is_match(token) {
                    return Err(Error::SyntaxError)
                }
            } else {
                lazy_static! {
                    static ref A_RERE:Regex = Regex::new(r"(@+|,+|~+|;+)").unwrap();
                }

                if !A_RERE.is_match(token) {
                    return Err(Error::SyntaxError)
                }
            }
        }

        let mut result: isize = 0;

        if RE_JWAK.is_match(&token_list[0]) {
            result = self.jwak_to_int(&token_list[0]).unwrap() as isize;
        }
        if RE_SHUNG.is_match(&token_list[0]) {
            result = self.var[
                self.shung_to_idx(&token_list[0]).unwrap()
            ] as isize;
        }

        let mut current_index = 1;

        lazy_static! {
            static ref RE_ADD: Regex = Regex::new(r"~+").unwrap();
            static ref RE_MINUS: Regex = Regex::new(r";+").unwrap();
            static ref RE_MULTI: Regex = Regex::new(r",+").unwrap();
            static ref RE_DIVIS: Regex = Regex::new(r"@+").unwrap();
        }
        
        while current_index < token_list.len() {
            let operator = &token_list[current_index];
            let next_value_string = &token_list[current_index + 1];
            let mut next_value: isize = 0;

            if RE_JWAK.is_match(next_value_string) {
                next_value = self.jwak_to_int(next_value_string).unwrap() as isize;
            }
            if RE_SHUNG.is_match(next_value_string ) {
                next_value = self.var[self.shung_to_idx(next_value_string).unwrap()];
            }

            if RE_ADD.is_match(&operator) {
                result += next_value;
            } else if RE_MINUS.is_match(&operator) {
                result -= next_value;
            } else if RE_MULTI.is_match(&operator) {
                result *= next_value;
            } else if RE_DIVIS.is_match(&operator) {
                if next_value == 0 {
                    return Err(Error::ZeroDivisionError)
                }
                result /= next_value;
            }
            current_index += 2;
        }

        Ok(result)
    }

    fn jwak_to_int(&self, code: &str) -> Result<usize, Error> {
        if let Some(captures) = RE_JWAK.captures(code) {
            let matched_str = &captures[0];
            if matched_str == "좍" {
                Ok(1)
            } else {
                Ok(matched_str.strip_prefix("좌")
                        .map_or(0, |s| s.matches('아').count()) + 2)
            }
        } else {
            Err(Error::SyntaxError)
        }
    }

    fn compile_line(&mut self, code: &str) -> Result<isize, Error> {
        let code_type = self.type_command(code)?;
        match code_type {
            CommandType::ASSIGN => {
                let index = self.shung_to_idx(code)?;
                let code = RE_SHUNG.replacen(code, 1, "");
                let result = self.calculate(&code)?;
                self.var[index] = result;
            }
            CommandType::IF => {
                if let Some((command_string, condition_string)) = code.split_once("하는재미") {
                    let value = self.calculate(condition_string)?;
                    if value == 0 {
                        return Ok(self.compile_line(command_string)?);
                    }
                }
            },
            CommandType::PRINT => {
                let kcount = code.chars().filter(|&c| c == 'ㅋ').count();
                
                if kcount == 0 {
                    return Err(Error::SyntaxError);
                }
            
                let index = kcount - 1;

                if code.contains("보호막") {
                    print!("{}", self.var[index]);
                } else {
                    print!("{}", char::from(self.var[index] as u8));
                }
            },
            CommandType::INPUT => {
                let kcount = code.matches("ㅋ").count();
                if kcount == 0 {
                    return Err(Error::SyntaxError)
                } else {
                    let index = kcount - 1;
                    if self.input_queue.is_empty() {
                        let mut input_text = String::new();

                        io::stdin()
                            .read_line(&mut input_text)
                            .expect("failed to read from stdin");

                        let value = input_text.trim().split_whitespace();

                        for v in value {
                            if let Ok(num)= v.parse::<isize>() {
                                self.input_queue.push(num);
                            } else {
                                for c in v.chars() {
                                    self.input_queue.push(c as isize);
                                }
                            }
                        }
                    }
                    if let Some(value) = self.input_queue.pop() {
                        self.var[index] = value;
                    } else {
                        return Err(Error::SyntaxError);
                    }
                }
            },
            CommandType::GOTO => {
                let eitcount = code.matches("에잇").count();
                if eitcount == 1 {
                    return Ok((code.matches("ㅋ").count() as isize) * -1)
                } else if eitcount == 2 {
                    return Ok(code.matches("ㅋ").count() as isize)
                }
                
            },
            CommandType::NONE => {},
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
                println!("Line 1\t{}\t<-", first_line);
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
                Err(_) => {
                    println!("Line {}\t{}\t<-", index+1, self.codeline[index]);
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