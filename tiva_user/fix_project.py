import os

file_path = r'c:\Users\feras\OneDrive\Desktop\University\2025-2026 Junior\Fall 2025\CSE322 Introduction to Embedded Systems\Project\Emb-proj\embProj.ewp'
lib_path = r'C:\ti\TivaWare_C_Series-2.2.0.295\driverlib\ewarm\Exe\driverlib.lib'

with open(file_path, 'r', encoding='utf-8') as f:
    lines = f.readlines()

new_lines = []
in_debug = False
in_ilink = False
found_lib_option = False

for i, line in enumerate(lines):
    if '<name>Debug</name>' in line:
        in_debug = True
    if '<name>Release</name>' in line:
        in_debug = False
    
    if in_debug and '<name>ILINK</name>' in line:
        in_ilink = True
    
    if in_debug and in_ilink and '<name>IlinkAdditionalLibs</name>' in line:
        found_lib_option = True
        new_lines.append(line)
        continue
        
    if found_lib_option:
        if '<state></state>' in line:
            new_lines.append(f'                    <state>{lib_path}</state>\n')
            found_lib_option = False # Reset
        else:
            # If it's not empty, maybe it already has something, keep it or append?
            # For now, assuming it's empty as per read_file
            new_lines.append(line)
            found_lib_option = False
    else:
        new_lines.append(line)

with open(file_path, 'w', encoding='utf-8') as f:
    f.writelines(new_lines)

print("Successfully updated embProj.ewp")
