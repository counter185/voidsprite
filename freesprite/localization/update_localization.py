import re
import sys
from typing import List, Tuple

base_localization_file = 'localization_english.txt'

target_localization_file = sys.argv[1] if len(sys.argv) > 1 else input("Localization file name: >")

keyregex = re.compile(r'\{"([^"]+)",\s*(?:"[^"]+"\s*)+\}')

def find_keys_in_file(filename):
    ret = {}
    with open(filename, 'r', encoding='utf-8') as file:
        content = file.read()
        for match in keyregex.finditer(content):
            matched_key = match.group(1)
            full_match = match.group(0)
            ret[matched_key] = full_match
    return ret

base_keys = find_keys_in_file(base_localization_file)
target_keys = find_keys_in_file(target_localization_file)

missing_keys = {key: base_keys[key] for key in base_keys if key not in target_keys}
dropped_keys = {key: target_keys[key] for key in target_keys if key not in base_keys}

for x in missing_keys:
    print(f"New key: {missing_keys[x]}")

for x in dropped_keys:
    print(f"Dropped key: {dropped_keys[x]}")

targetcontent = ''
with open(target_localization_file, 'r', encoding='utf-8') as file:
    targetcontent = file.read()

targetcontent.rstrip()
insertIndex = targetcontent.rfind('},\n') + 2

targetcontent = targetcontent[:insertIndex] + "\n\n" + targetcontent[insertIndex:]
insertIndex += 2

section = ""
section_re = re.compile(r'[^\.]+\.[^\.]+')
for key in missing_keys:
    insertString = f'    {missing_keys[key]},\n'
    section_now = section_re.match(key).group(0)
    if section_now != section:
        insertString = f'\n{insertString}'
        section = section_now
    targetcontent = targetcontent[:insertIndex] + insertString + targetcontent[insertIndex:]
    insertIndex += len(insertString)

with open(target_localization_file, 'w', encoding='utf-8') as file:
    file.write(targetcontent)

print(f"Updated {target_localization_file} with {len(missing_keys)} new keys.")