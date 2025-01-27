with open("stdlib.h", "w") as target_h:
    target_h.write("#ifndef RY_STDLIB_H\n")
    target_h.write("#define RY_STDLIB_H\n")
    target_h.write("extern char *stdlib_code;\n")
    target_h.write("#endif // RY_STDLIB_H")

stdlib_files = ["io", "assert", "math", "random"]
content = ""

for filename in stdlib_files:
    with open(filename+".ry", "r") as f:
        for line in f.readlines():
            for c in line:
                if c == '\\':
                    content += "\\\\"
                elif c == '\n':
                    content += "\\n"
                elif c == '\t':
                    content += "\\t"
                elif c == '"':
                    content += "\\\""
                elif c == '\0':
                    continue
                else:
                    content += c
        content += '\\n\\n'

with open("stdlib.c", "w") as f:
    f.write("#include \"stdlib.h\"\n")
    f.write(f"char *stdlib_code = \"{content}\\n\\n\";")
                
