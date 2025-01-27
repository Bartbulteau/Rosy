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

with open("stdlib.h", "w") as f:
    f.write("#ifndef RY_STDLIB_H\n#define RY_STDLIB_H\n\n")
    f.write(f"static const char *stdlib_code = \"{content}\\n\\n\";\n\n")
    f.write("#endif // RY_STDLIB_H")
                
