# Python 3.10 is required
import sys
import argparse
import pyparsing as pp
import datetime

# https://github.com/pyparsing/pyparsing/blob/master/examples/lua_parser.py#L210
ppc = pp.pyparsing_common
pp.ParserElement.enablePackrat()

SHOW_DEBUG = False
EXIT_SUCCESS = 0
EXIT_FAILURE = 1
MEM_SIZE = 4096

class bcolors:
    HEADER    = '\033[95m'
    OKBLUE    = '\033[94m'
    OKCYAN    = '\033[96m'
    OKGREEN   = '\033[92m'
    WARNING   = '\033[93m'
    FAIL      = '\033[91m'
    ENDC      = '\033[0m'
    BOLD      = '\033[1m'
    UNDERLINE = '\033[4m'

def is_ascii(s):
    return all(ord(c) < 128 for c in s)

def error(info, str):
    print(bcolors.FAIL + bcolors.BOLD + f"{info} ERROR:\n" + bcolors.ENDC + bcolors.WARNING + f"{str}" + bcolors.ENDC) 
    sys.exit(1) 

# Parse input file
def parse(file):
    global SHOW_DEBUG
    nodes = None

    COMMA = pp.Literal(",").suppress()
    COLON = pp.Literal(":").suppress()
    SEMIC = pp.Literal(";").suppress()

    registers = {
        r.upper(): pp.Keyword(r)
        for r in """\
        ra rb rc rd
        """.split()
    }
    vars().update(registers)

    keywords = {
        k.upper(): pp.Keyword(k)
        for k in """\
        dio jmp add shl sub clr gsr pts pfs lda sta beq bnq slt dec smt inc set
        """.split()
    }
    vars().update(keywords)

    defines = {
        d.upper(): pp.Keyword(d)
        for d in """\
        offset data macro import
        """.split()
    }
    vars().update(defines)

    any_keyword   = pp.MatchFirst(keywords.values()).set_name("<keyword>")
    any_registers = pp.MatchFirst(registers.values()).set_name("<register>")
    any_defines   = pp.MatchFirst(defines.values()).set_name("<define>")
    any_word = any_keyword | any_registers | any_defines
    # word <- !keywords <id>
    word =  ~any_word + ppc.identifier

    statements = pp.Forward()

    # hex_value <-
    hex_value = pp.Literal("0x").suppress() + ppc.hex_integer
    number = (hex_value | ppc.number) # ("number")
    normal_text = pp.QuotedString("'", '\\', multiline=True)("single_quotes") # unquoteResults=False,
    revers_text = pp.QuotedString('"', "\\", multiline=True)("double_quotes") # unquoteResults=False,
    # normal_text.
    def convert_string(instring, loc, toks):
        print(f'loc: {loc}, toks: {toks}')
        if (toks[0][0] == '"'):
            return [ toks[0], True ]
        else:
            return  [ toks[0], False ]
    string = normal_text | revers_text
    # string.set_parse_action(convert_string)
    label = pp.Literal("=") + word

    instr_define = word + pp.Optional(
        COLON + pp.delimitedList(word)
    )
    block = pp.Group(pp.Literal("(").suppress() + instr_define + pp.Literal(")").suppress()) \
          + pp.Group(pp.Literal("{").suppress() + pp.Optional(statements) + pp.Literal("}").suppress())
    
    #   import, data, macro
    define_offset = pp.Keyword("offset") + COLON + number + SEMIC
    define_data   = pp.Keyword("data") + COLON + pp.delimitedList(string | number) + SEMIC
    define_macro  = pp.Keyword("macro") + COLON + block + pp.Optional(SEMIC)
    define_stat   = pp.Literal("@") + (define_data | define_macro | define_offset)

    # label_stat <- = word :
    label_stat = label + COLON

    # rtype_op <- add | sub | lda | sta | beq | bnq | slt | smt
    rtype_op =  pp.Keyword("add") | \
                pp.Keyword("sub") | \
                pp.Keyword("lda") | \
                pp.Keyword("sta") | \
                pp.Keyword("beq") | \
                pp.Keyword("bnq") | \
                pp.Keyword("slt") | \
                pp.Keyword("smt")
    
    # instr_jtype <- jmp : (label | number) ;
    instr_jtype = pp.Keyword("jmp") + COLON + (label | number) + SEMIC

    # instr_stype <- set : reg , <-+[0-9]+> ;
    instr_stype = pp.Keyword("set") + COLON + any_registers + COMMA + ((label) | number) + SEMIC
    
    # instr_rtype <- rtype_op : reg , reg ;
    instr_rtype = rtype_op + COLON + any_registers + COMMA + any_registers + SEMIC

    # instr_itype <- dio | clr | gsr | pts | pfs ;
    itype_op =  pp.Keyword("dio") | \
                pp.Keyword("clr") | \
                pp.Keyword("gsr") | \
                pp.Keyword("pts") | \
                pp.Keyword("pfs")
    
    # itype_op2 <- (inc | dec | shl) : reg ;
    itype_op2 = (pp.Keyword("inc") + COLON + any_registers) | \
                (pp.Keyword("dec") + COLON + any_registers) | \
                (pp.Keyword("shl") + COLON + any_registers)
    
    instr_itype = (itype_op | itype_op2) + SEMIC

    # instr_ctype <- word : ((reg | number) , )+ ;
    # instr_ctype = word + pp.Optional(COLON + pp.delimitedList(any_registers | string | number)) + SEMIC

    # instr_stat <- instr_dio
    instr_stat = instr_itype | instr_rtype | instr_stype | instr_jtype#  | instr_ctype

    # comment <- # ... \n
    comment = pp.Literal("#") + pp.restOfLine

    for var_name in """
            define_stat
            label_stat
            instr_stat
            """.split():
        vars()[var_name].setName(var_name)

    # stats <- (define | label | instr)
    statements <<= pp.Group(
        # comment |
        define_stat |
        label_stat  |
        instr_stat
    )
    # entry <- stats*
    entry = pp.ZeroOrMore(statements)#
    entry.ignore(comment)

    # for key, val in entry.items():
    #     entry[key] = val.setResultsName(key)

    try:
        # print(f"SOURCE: \n{src}\n")
        nodes = entry.parseFile(file, parseAll=True)
        if SHOW_DEBUG:
            # print(nodes.dump())
            # nnodes = nodes.asDict()
            print(repr(nodes))
            # print(nodes.getName("single_quote"))
            print("NODES DUMP:")
            nodes.pprint()
            # print(nodes.dump())

        # print(nodes)
    except pp.ParseException as pe:
        error("PARSING", pe.explain())

    return nodes

def get_number(num, bitmask):
    return num & bitmask

def get_char(ch):
    if is_ascii(ch) != True:
        return 63 # '?' symbol
    return ord(ch)

def get_opcode(op):
    match op:
        case 'jmp' | 'dio': return 0x00
        case 'add': return 0b0100
        case 'shl': return 0b0100 # and rr rr
        case 'sub': return 0b0101
        case 'clr': return 0b01010000 # TODO: доделать
        case 'gsr': return 0b01010101
        case 'pts': return 0b01011010
        case 'pfs': return 0b01011111
        case 'lda': return 0b0110
        case 'sta': return 0b0111
        case 'beq': return 0b1000
        case 'bnq': return 0b1001
        case 'slt': return 0b1010
        case 'dec': return 0b1010 # and rr rr
        case 'smt': return 0b1011
        case 'inc': return 0b1011 # and rr rr
        case 'set': return 0b11
    error("TRANSLATE::OPCODE", f"Unknown opcode: {op}!")

def get_register(reg):
    match reg:
        case 'ra': return 0b00
        case 'rb': return 0b01
        case 'rc': return 0b10
        case 'rd': return 0b11
        case _:
            error("TRANSLATE::REG", f"Unknown register: {reg}!")
    

def translate(nodes):
    labels = [] 
    data = [0] * MEM_SIZE
    # print(nodes)

    # add labels first
    pc = 0
    for n in nodes:
        match n[0]:
            case '@':
                if n[1] == "offset":
                    pc = get_number(n[2], 0b1111_1111_1111)
                if n[1] == "data":
                    for nn in n[2:]:
                        if isinstance(nn, int): # put 8 bit numbers
                            # data[pc] = get_number(nn, 0b1111_1111)
                            pc += 1
                        elif isinstance(nn, str): # put every char
                            for s in nn:
                                # data[pc] = get_char(s)
                                pc += 1
                        else:
                            error("TRANSLATE::DEFINE::DATA", f"Unknown data: {nn}!")
            case '=':
                if len(labels) == 0:
                    labels.append({
                        'name': n[1],
                        'position': pc
                    })
                else:
                    r = next((item for item in labels if item['name'] == n[1]), None)
                    # print(f"label: {r}")
                    if r == None:
                        labels.append({
                            'name': n[1],
                            'position': pc
                        })
                    else:
                        error("TRANSLATE::LABEL", f"Dublicate '{n[1]}' label found!")
            case _:
                # every instruction is fixed width so
                pc += 1

    # proceed the rest of tokens
    pc = 0
    for n in nodes:
        # print(n)
        match n[0]:
            case '@':
                if n[1] == "offset":
                    pc = get_number(n[2], 0b1111_1111_1111)
                elif n[1] == "data":
                    for nn in n[2:]:
                        if isinstance(nn, int): # put 8 bit numbers
                            data[pc] = get_number(nn, 0b1111_1111)
                            pc += 1
                        elif isinstance(nn, str): # put every char
                            # print(nn.)
                            # print(n)
                            for s in nn:
                                data[pc] = get_char(s)
                                pc += 1
                else:
                    error("TRANSLATE::DEFINE", f"Unknown define: {n[1]}!")
            case '=':
                pass # we passed it before so skip it
            case 'dio' | 'clr' | 'gsr' | 'pts' | 'pfs':
                data[pc] = get_opcode(n[0])
                pc += 1
            case 'jmp':
                a = pc # Current address
                if n[1] == "=": # TODO: add - and + labels
                    r = next((item for item in labels if item['name'] == n[2]), None)
                    if r == None:
                        error("TRANSLATE::LABEL", f"Label '{n[2]}' not found!")
                    else:
                        a = (r['position'] - a)
                        # if (a > 0 or a < 0): a -= 1
                        # print(f"label a: {a}")
                elif isinstance(n[1], int):
                    a = n[1]
                    # print(f"value {a}")
                else:
                    error("TRANSLATE::OPCODE::JMP", f"Unknown data: f{n[1]}!")
                
                if (a < -32 or a > 31):
                    error("TRANSLATE::OPCODE::JMP", f"Offset '{a}' in label '{n[3]}' is too far! (jmp can be in -32 .. 31 range)")
                if (a == 0):
                    error("TRANSLATE::OPCODE::JMP", f"Invalid JMP Instruction! Can't jump to itself. (Label: {n[2]} is zero)")
                
                # print(f"offset: {a} ({ord(struct.pack('b', a & 0b00111111))})")
                opcode = get_opcode(n[0])
                data[pc] = opcode << 6 | get_number(a, 0b00111111)
                pc += 1
            case 'set':
                opcode = get_opcode(n[0])
                reg = get_register(n[1])
                val = pc
                if n[2] == "=":
                    r = next((item for item in labels if item['name'] == n[3]), None)
                    if r == None:
                        error("TRANSLATE::OPCODE::SET", f"Label '{n[3]}' not found!")
                    else:
                        val = (r['position'] - val)
                        # if (val > 0): val -= 1
                        # print(f"sosi {n[1]} {n[3]} {val}")
                elif isinstance(n[2], int):
                    val = n[2]
                else:
                    error("TRANSLATE::OPCODE::SET", f"Unknown data: f{n[2]}!")
                
                if (val < -8 or val > 7):
                    error("TRANSLATE::OPCODE::SET", f"Value '{val}' is out of range! (set can be in -8 .. 7)")

                val = get_number(val, 0b1111) 
                data[pc] = opcode << 6 | reg << 4 | val
                pc += 1
            case 'add' | 'lda' | 'sta' | 'beq' | 'bnq':
                opcode = get_opcode(n[0])
                r1 = get_register(n[1])
                r2 = get_register(n[2])
                data[pc] = opcode << 4 | r1 << 2 | r2
                pc += 1
            case 'slt' | 'smt':
                opcode = get_opcode(n[0])
                r1 = get_register(n[1])
                r2 = get_register(n[2])
                if (r1 == r2):
                    error(f"TRANSLATE::OPCODE::{str(n[0]).upper()}", f"Invalid opcode! You can't use same registers ({n[1]} with {n[2]})")
                data[pc] = opcode << 4 | r1 << 2 | r2
                pc += 1
            case 'inc' | 'dec' | 'shl':
                opcode = get_opcode(n[0])
                r1 = get_register(n[1])
                data[pc] = opcode << 4 | r1 << 2 | r1
                pc += 1
            case 'sub':
                opcode = get_opcode(n[0])
                r1 = get_register(n[1])
                r2 = get_register(n[2])
                if (r1 == r2):
                    error("TRANSLATE::OPCODE::SUB", f"Invalid opcode! You can't use same registers ({n[1]} with {n[2]})")
                    # data[pc] = opcode
                else:
                    data[pc] = opcode << 4 | r1 << 2 | r2
                pc += 1
            case _:
                error("TRANSLATE", f"Unknown token '{n[0]}'!")
                pass

    return data

# Main function (Entry Point)
def main():    
    global SHOW_DEBUG

    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--input' , type=argparse.FileType('r'), help='input as .asm file')
    parser.add_argument('-o', '--output', type=argparse.FileType('w'), help='output as .rom file')
    parser.add_argument('-d', '--debug', help='show debug info', action='store_true')
    args = parser.parse_args()
    
    if args.debug:
        SHOW_DEBUG = True

    # Print usage if no args provided
    if not args.input:
        parser.print_usage()
        return EXIT_FAILURE
    
    # It's parsing time
    start = datetime.datetime.now()
    nodes = None
    with args.input as file:
        nodes = parse(file)
    
    data = translate(nodes)

    # Write the result
    output = './out.rom'
    if args.output:
        output = args.output.name
    with open(output, "wb") as file:
        # print(data)
        file.write(bytes(data))
    # 
    end = datetime.datetime.now()
    elapsed = end - start
    print(f"Done! Output: {output}")
    print(f"Process time: {(elapsed.total_seconds() * 1000):.2f} ms.")
    return EXIT_SUCCESS
if __name__ == '__main__':
    sys.exit(main())