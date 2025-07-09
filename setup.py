import questionary
import shutil

# yes i was lazy i used ChatGPT for this

answer = questionary.text("What should be the name of the Nova module?").ask()

default_nickname = ''.join(word[0].upper() for word in answer.split() if word)

questionary.print("I recommend leaving this as recommended")
nickname = questionary.text("Nickname of the module (as standalone)", default=default_nickname).ask()

words = answer.split()
if words and words[0].startswith(('N', 'n')):
    output2 = ''.join(word[0].upper() for word in words[1:] if word)
else:
    output2 = default_nickname

nickname_Nova = questionary.text(
    "Nickname of the module in the Engine (Sometimes I put N at the start like in NGE but in engine I put Nova::GE)", 
    default=output2
).ask()

print("Answer:", answer)
print("Nickname:", nickname)
print("Nickname Nova:", nickname_Nova)
ok = questionary.confirm("Are your sure? All things will be now applied in code", auto_enter=False).ask()


module_info = """
#ifndef MODULE_INFO
#define MODULE_INFO

namespace {} {{
    constexpr const char* module_name = "{}";
}}

namespace Nova {{
    namespace {} = ::{}; 
}}

#endif
""".format(nickname, answer, nickname_Nova, nickname)


if ok:
    import os
    output_path = "src/core/include/module.h"
    os.makedirs(os.path.dirname(output_path), exist_ok=True)

    with open(output_path, "w", encoding="utf-8") as f:
        f.write(module_info)

    print(f"Written module info to {output_path}")
else:
    print("Operation cancelled by user.")



ok = questionary.confirm("Nickname the include folder ? (Using nickname not nickname Nova)", default=True).ask()

if ok:
    base_include_dir = "src/core/include"
    target_dir = os.path.join(base_include_dir, nickname)

    # Make sure target directory exists
    os.makedirs(target_dir, exist_ok=True)

    # Move all files and folders from base_include_dir except the <nickname> folder itself
    for item in os.listdir(base_include_dir):
        source_path = os.path.join(base_include_dir, item)
        dest_path = os.path.join(target_dir, item)

        # Skip if it's the target folder itself
        if item == nickname:
            continue

        # Move the file/folder
        shutil.move(source_path, dest_path)

    print(f"Moved all files/folders into {target_dir}")
else:
    print("Skipping include folder renaming.")