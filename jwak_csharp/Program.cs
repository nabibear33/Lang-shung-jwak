using System.Diagnostics;
using System.Reflection;
using System.Reflection.Emit;
using System.Text.RegularExpressions;

namespace LangShungJwak
{
    internal class Program
    {
        static void Main(string[] args)
        {
            if (args.Length < 1)
            {
                Console.WriteLine("사용법: LangShungJwak.exe <FileName.jwak>");
                return;
            }
            string jwak = File.ReadAllText(args[0]);
            var runner = new Runner(jwak);
            var thread = new Thread(() => runner.Run()) { IsBackground = true };
            Stopwatch sw = Stopwatch.StartNew();
            thread.Start();
            while (thread.ThreadState == System.Threading.ThreadState.Background)
            {
                if (sw.Elapsed > TimeSpan.FromSeconds(3))
                {
                    Console.WriteLine($"Line {Runner.CurrentLine}에서 타임 패러독스!");
                    break;
                }
            }
        }
    }
    public partial class Runner
    {
        public static int CurrentLine;
        public readonly string Code;
        public bool IsRunning { get; private set; }

        private DynamicMethod dm;
        private Dictionary<int, LocalBuilder> vars;
        private Dictionary<int, Label> labels;
        private Action result;
        private int totalLines;

        public Runner(string code)
        {
            if (!code.StartsWith("교주님"))
                throw new InvalidOperationException("어떻게 이게 리슝좍이냐!");
            for (int i = 0; i < Skips.Length; i++)
                code = code.Replace(Skips[i].ToString(), "");
            Code = code;
            dm = new DynamicMethod(string.Empty, typeof(void), Type.EmptyTypes, true);
            vars = new Dictionary<int, LocalBuilder>();
            labels = new Dictionary<int, Label>();
            var il = dm.GetILGenerator();
            try
            {
                PrepareLabels(il);

                int lineNumber = 0;
                using var sr = new StringReader(code);
                while (sr.Peek() != -1)
                {
                    var line = sr.ReadLine()!;
                    if (labels.TryGetValue(lineNumber, out var label))
                        il.MarkLabel(label);
                    List<(OpCode, object?)> emits = new List<(OpCode, object?)>();
                    List<(OpCode, object?)> ifEmits = new List<(OpCode, object?)>(); // If문의 조건을 확인하는 IL 코드
                    (OpCode, object?)? op = null;
                    bool loadVar = false;
                    Label? ifLabel = null;
                    LocalBuilder? toSet = null;
                    foreach (Token tok in Tokenize(line))
                    {
                        switch (tok.type)
                        {
                            case 0:
                                emits.Add((OpCodes.Ldc_I4, tok.value));
                                if (op != null)
                                {
                                    emits.Add(op.Value);
                                    op = null;
                                }
                                break;
                            case 1:
                                (OpCode, object?) val = (tok.value switch
                                {
                                    0 => OpCodes.Add,
                                    1 => OpCodes.Sub,
                                    2 => OpCodes.Mul,
                                    3 => OpCodes.Div,
                                    _ => throw new InvalidOperationException("어떻게 이게 리슝좍이냐!")
                                }, null);
                                if (op != null) emits.Add(val);
                                else op = val;
                                break;
                            case 2:
                                if (!vars.TryGetValue((int)tok.value, out var lb))
                                    vars.Add((int)tok.value, lb = il.DeclareLocal(typeof(int)));
                                if (loadVar) emits.Add((OpCodes.Ldloc, lb));
                                else toSet = lb;
                                if (op != null)
                                {
                                    emits.Add(op.Value);
                                    op = null;
                                }
                                break;
                            case 3:
                                emits.Add((OpCodes.Ldloc, vars[(int)tok.value]));
                                emits.Add((OpCodes.Conv_U2, null));
                                emits.Add((OpCodes.Call, Write));
                                break;
                            case 4:
                                emits.Add((OpCodes.Ldloc, vars[(int)tok.value]));
                                emits.Add((OpCodes.Call, Write_Int));
                                break;
                            case 5:
                                if (!vars.TryGetValue((int)tok.value, out lb))
                                    vars.Add((int)tok.value, lb = il.DeclareLocal(typeof(int)));
                                emits.Add((OpCodes.Call, Read));
                                emits.Add((OpCodes.Call, Parse_Int));
                                emits.Add((OpCodes.Stloc, vars[(int)tok.value]));
                                break;
                            case 6:
                                ifLabel = il.DefineLabel();
                                ifEmits = emits.ToList();
                                emits = new List<(OpCode, object?)>();
                                break;
                            case 7:
                                int target = lineNumber + (int)tok.value;
                                if (target >= totalLines)
                                    target = int.MaxValue;
                                emits.Add((OpCodes.Br, labels[target]));
                                break;
                        }
                        loadVar = true;
                    }
                    if (toSet != null)
                    {
                        if (emits.Count == 0)
                            emits.Add((OpCodes.Ldc_I4_0, null));
                        emits.Add((OpCodes.Stloc, toSet));
                    }

                    Emit(il, OpCodes.Ldc_I4, lineNumber + 1);
                    Emit(il, OpCodes.Stsfld, Cur_Line);

                    if (ifLabel != null)
                    {
                        for (int i = 0; i < emits.Count; i++)
                        {
                            var (oc, value) = emits[i];
                            Emit(il, oc, value);
                        }
                        Emit(il, OpCodes.Brtrue, ifLabel.Value);
                    }

                    var toEmit = ifLabel != null ? ifEmits : emits;
                    for (int i = 0; i < toEmit.Count; i++)
                    {
                        var (oc, value) = toEmit[i];
                        Emit(il, oc, value);
                    }
                    
                    if (ifLabel != null)
                        il.MarkLabel(ifLabel.Value);
                    lineNumber++;
                }
                if (labels.TryGetValue(int.MaxValue, out var lastLabel))
                    il.MarkLabel(lastLabel);
                Emit(il, OpCodes.Ret, null);

                result = (Action)dm.CreateDelegate(typeof(Action));
            }
            catch { throw; }
        }

        public void Run() => result();
        private void PrepareLabels(ILGenerator il)
        {
            int lineNumber = 0;
            using var sr = new StringReader(Code);
            while (sr.Peek() != -1)
            {
                var line = sr.ReadLine()!;
                var eitCount = BranchMatcher().Count(line);
                if (eitCount > 0)
                {
                    int offset = line.Count(c => c == 'ㅋ');
                    int isIf = line.IndexOf("하는재미");
                    if (isIf != -1) offset = line.Substring(0, isIf).Count(c => c == 'ㅋ');
                    if (eitCount == 2)
                        labels.Add(lineNumber + offset, il.DefineLabel());
                    else labels.Add(lineNumber - offset, il.DefineLabel());
                }
                lineNumber++;
            }
            totalLines = lineNumber;
            foreach (var target in labels.Keys.ToList())
            {
                if (target < 0) throw new InvalidOperationException("어떻게 이게 리슝좍이냐!");
                if (target >= lineNumber)
                {
                    labels[int.MaxValue] = labels[target];
                    labels.Remove(target);
                }
            }
        }

        internal static IEnumerable<Token> Tokenize(string line)
        {
            for (int i = 0; i < line.Length; i++)
            {
                // 출력
                if (IsKwEquals(line, "비비", ref i))
                {
                    int varId = -1;
                    SkipCurrentChar(line, 'ㅋ', ref i, () => varId++);
                    bool raw = IsKwEquals(line, "보호막", ref i);
                    SkipCurrentChar(line, 'ㅋ', ref i, () => varId++);
                    SkipCurrentChar(line, '따', ref i);
                    if (line[i] != '잇') throw new InvalidOperationException("어떻게 이게 리슝좍이냐!");
                    SkipNextChar(line, 'ㅋ', ref i, () => varId++);
                    if (varId < 0) varId = 0;
                    yield return new Token(raw ? 4 : 3, varId);
                }

                // 입력
                if (IsKwEquals(line, "순수", ref i))
                {
                    int varId = -1;
                    SkipCurrentChar(line, 'ㅋ', ref i, () => varId++);
                    SkipCurrentChar(line, '따', ref i);
                    if (line[i] != '잇') throw new InvalidOperationException("어떻게 이게 리슝좍이냐!");
                    SkipNextChar(line, 'ㅋ', ref i, () => varId++);
                    yield return new Token(5, varId);
                }

                // 조건
                if (IsKwEquals(line, "하는재미", ref i))
                    yield return new Token(6, -1);

                // 연산
                if (IsCalculateOperation(line[i]))
                    yield return TokenizeCalculateOperation(line, ref i);

                // 숫자 & 변수
                if (IsNumberOrVariable(line[i]))
                    yield return TokenizeNumberOrVariable(line, ref i);

                // 이동
                if (IsKwEquals(line, "에잇", ref i))
                {
                    int target = line.Count(c => c == 'ㅋ');
                    int isIf = line.IndexOf("하는재미");
                    if (isIf != -1) target = line.Substring(0, isIf).Count(c => c == 'ㅋ');
                    bool down = IsKwEquals(line, "에잇", ref i);
                    yield return new Token(7, down ? target : -target);
                }
            }
        }

        private static bool IsNumberOrVariable(char start) => start == '좌' || start == '좍' || start == '슈' || start == '슝';
        private static Token TokenizeNumberOrVariable(string line, ref int i)
        {
            SkipCurrentChar(line, 'ㅋ', ref i);
            // 숫자
            if (line[i] == '좌')
            {
                int value = 2;
                SkipNextChar(line, '아', ref i, () => value++);
                if (line[i] != '악') throw new InvalidOperationException("어떻게 이게 리슝좍이냐!");
                return new Token(0, value);
            }
            else if (line[i] == '좍') return new Token(0, 1);
            // 변수
            else if (line[i] == '슈')
            {
                int value = 1;
                SkipNextChar(line, '우', ref i, () => value++);
                if (line[i] != '웅') throw new InvalidOperationException("어떻게 이게 리슝좍이냐!");
                return new Token(2, value);
            }
            else if (line[i] == '슝') return new Token(2, 0);
            throw new InvalidOperationException($"어떻게 이게 리슝좍이냐!");
        }
        private static bool IsCalculateOperation(char c) => Array.IndexOf(Operators, c) != -1;
        private static Token TokenizeCalculateOperation(string line, ref int i)
        {
            // 연산
            int opIndex = Array.IndexOf(Operators, line[i]);
            SkipNextChar(line, Operators[opIndex], ref i);
            if (Array.IndexOf(Operators, line[i]) != -1)
                throw new InvalidOperationException($"어떻게 이게 리슝좍이냐!");
            return new Token(1, opIndex);
        }

        private static bool IsKwEquals(string line, string kw, ref int index)
        {
            if (line.Length - index < kw.Length) return false;
            for (int i = 0; i < kw.Length; i++)
                if (line[index + i] != kw[i]) return false;
            index += kw.Length;
            return true;
        }
        private static void SkipCurrentChar(string line, char c, ref int index, Action? onSkip = null)
        {
            while (index < line.Length && line[index] == c)
            {
                index++;
                onSkip?.Invoke();
            }
        }
        private static void SkipNextChar(string line, char c, ref int index, Action? onSkip = null)
        {
            while (index < line.Length - 1 && line[++index] == c) onSkip?.Invoke();
        }
        private static void Emit(ILGenerator il, OpCode op, object? value)
        {
            if (value is int i) il.Emit(op, i);
            else if (value is LocalBuilder lb) il.Emit(op, lb);
            else if (value is Label l) il.Emit(op, l);
            else if (value is MethodInfo m) il.Emit(op, m);
            else if (value is FieldInfo f) il.Emit(op, f);
            else il.Emit(op);
        }

        private static readonly char[] Operators = { '~', ';', ',', '@' };
        private static readonly char[] Skips = { '?', '!', '.', ' ' };
        private static readonly MethodInfo Read = typeof(Console).GetMethod("ReadLine")!;
        private static readonly MethodInfo Write = typeof(Console).GetMethod("Write", new Type[] { typeof(char) })!;
        private static readonly MethodInfo Write_Int = typeof(Console).GetMethod("Write", new Type[] { typeof(int) })!;
        private static readonly MethodInfo Parse_Int = typeof(int).GetMethod("Parse", new Type[] { typeof(string) })!;
        private static readonly FieldInfo Cur_Line = typeof(Runner).GetField("CurrentLine")!;
        internal struct Token
        {
            /// <summary>
            /// 0: Number
            /// 1: Operator (0:Add/1:Sub/2:Mul/3:Div)
            /// 2: Variable
            /// 3: Print ASCII
            /// 4: Print Raw
            /// 5: Input
            /// 6: If
            /// 7: Goto
            /// </summary>
            public int type;
            public object value;
            public Token(int type, object value)
            {
                this.type = type;
                this.value = value;
            }
        }
        [GeneratedRegex("에잇")]
        private static partial Regex BranchMatcher();
    }
}
