import os


def get_shaders(shaders_path: str) -> list[str]:
    is_shader = lambda file: any(file.endswith(ext) for ext in [".vert", ".frag", ".glsl"])
    return [file for file in os.listdir(shaders_path) if is_shader(file)]

def create_definitions(shaders_path: str, shader_filenames: list[str]) -> list[str]:
    res = []

    for file in shader_filenames:
        varname = file.replace(".", "_")
        varname = varname.upper()

        with open(shaders_path + "/" + file, "r") as fstream:
            text = fstream.read()
            res.append(f"constexpr const char *{varname} = R\"---({text})---\";\n")

    return res


shaders_path = "src/shaders"
filenames = get_shaders(shaders_path)
definitions = create_definitions(shaders_path, filenames)

with open(f"{shaders_path}/shaders.h", "w") as shaders_h:
    shaders_h.write("\n".join(definitions))
