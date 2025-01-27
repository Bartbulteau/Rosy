with open("opcodes.h", "w") as target_file:

    target_file.write("#ifndef OPCODES_H\n")
    target_file.write("#define OPCODES_H\n\n")

    op_counter = 0
    op_names = []

    with open("opcodes.txt", "r") as op_file:
        for line in op_file.readlines():
            line = line[:-1].upper()
            if line != "":
                target_file.write(f"#define OP_{line}{" "*(30-len(line))}{op_counter}\n")
                op_names.append(line)
                op_counter += 1
            else:
                target_file.write("\n")
    
    target_file.write(f"\n\n#define N_OPCODES {op_counter}\n")

    target_file.write("\n\nstatic char RyOpcodeNames[N_OPCODES][100] = {\n")
    for op_name in op_names:
        target_file.write(f"\t\"{op_name}\",\n")
    target_file.write("};\n\n")

    target_file.write("\n#endif // OPCODES_H\n")