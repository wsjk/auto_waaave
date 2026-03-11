#!/usr/bin/env python3
"""
Script to add midiLatchingEnabled check to all remaining latching code in ofApp.cpp
This script wraps all the abs(...)<CONTROL_THRESHOLD checks with midiLatchingEnabled conditionals.
"""

import re
import sys

def wrap_latching_code(content):
    """
    Finds and wraps latching code patterns with midiLatchingEnabled checks.

    Pattern to match:
    if(abs(...)<CONTROL_THRESHOLD){
        arrayVar[index]=1;
    }
    if(arrayVar[index]==1){
        assignment statements
    }
    """

    # This regex matches the latching pattern
    # Group 1: indentation
    # Group 2: the abs condition
    # Group 3: array variable name (midiLowActiveFloat, midiMidActiveFloat, midiHighActiveFloat, midiActiveFloat, vmidiActiveFloat)
    # Group 4: array index
    # Group 5: the assignment block

    pattern = r'( +)if\((abs\([^<]+)<CONTROL_THRESHOLD)\)\{\s+([a-zA-Z_]+)\[(\d+)\]=1;\s+\}\s+if\(\3\[\4\]==1\)\{([^}]+(?:\{[^}]*\}[^}]*)*)\}'

    def replace_func(match):
        indent = match.group(1)
        condition = match.group(2)
        array_var = match.group(3)
        index = match.group(4)
        assignments = match.group(5).strip()

        # Check if this section already has midiLatchingEnabled (to avoid double-wrapping)
        if 'midiLatchingEnabled' in match.group(0):
            return match.group(0)

        # Build the replacement with latching check
        replacement = f'''{indent}if(midiLatchingEnabled){{
{indent}    if({condition}){{
{indent}        {array_var}[{index}]=1;
{indent}    }}
{indent}    if({array_var}[{index}]==1){{
{indent}        {assignments}
{indent}    }}
{indent}}} else {{
{indent}    // Direct control without latching
{indent}    {array_var}[{index}]=1;
{indent}    {assignments}
{indent}}}'''

        return replacement

    # Apply the regex replacement
    new_content = re.sub(pattern, replace_func, content, flags=re.MULTILINE | re.DOTALL)

    return new_content

def main():
    filepath = 'AUTO_WAAAVE_4_5/src/ofApp.cpp'

    # Read the file
    try:
        with open(filepath, 'r') as f:
            content = f.read()
    except FileNotFoundError:
        print(f"Error: Could not find {filepath}")
        print("Make sure you're running this script from the auto_waaave directory")
        return 1

    print("Processing ofApp.cpp...")
    print(f"Original file size: {len(content)} characters")

    # Apply the transformations
    new_content = wrap_latching_code(content)

    print(f"New file size: {len(new_content)} characters")

    if new_content == content:
        print("No changes were made. All latching code may already be wrapped.")
    else:
        # Backup original file
        backup_path = filepath + '.backup'
        with open(backup_path, 'w') as f:
            f.write(content)
        print(f"Created backup: {backup_path}")

        # Write the modified content
        with open(filepath, 'w') as f:
            f.write(new_content)
        print(f"Updated {filepath}")
        print("MIDI latching checks have been added to all remaining controls.")

    return 0

if __name__ == '__main__':
    sys.exit(main())


