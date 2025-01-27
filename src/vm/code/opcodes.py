opcodes = []

with open("opcodes.txt", "r") as source:
    for line in source.readlines():
        if len(line) > 1:
            opcodes.append(line[:-1].upper())

with open("opcodes.h", "w") as target_h:
    target_h.write("#ifndef OPCODES_H\n")
    target_h.write("#define OPCODES_H\n")
    for i, opcode in enumerate(opcodes):
        if (len(opcode) == 0):
            target_h.write("\n")
            continue
        target_h.write(f"#define OP_{opcode}{" "*(30 - len(opcode))}{i}\n")
    target_h.write(f"#define N_OPCODES {len(opcodes)}\n")
    target_h.write("extern char RyOpcodeNames[N_OPCODES][100];\n")
    target_h.write("#endif // OPCODES_H")

with open("opcodes.c", "w") as target_c:
    target_c.write("#include \"opcodes.h\"\n")
    target_c.write("char RyOpcodeNames[N_OPCODES][100] = {\n")
    for opcode in opcodes:
        target_c.write(f"\t\"{opcode}\",\n")
    target_c.write("};")