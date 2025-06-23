#!/usr/bin/env python3
"""
Add observer-only guards to NimBLE C++ files that aren't needed for scanning
"""

import os
import re

# Files that should be wrapped with observer-only guards
FILES_TO_WRAP = [
    # Client/Server files  
    "NimBLERemoteCharacteristic.cpp",
    "NimBLERemoteDescriptor.cpp", 
    "NimBLERemoteService.cpp",
    "NimBLERemoteValueAttribute.cpp",
    
    # HID support
    "NimBLEHIDDevice.cpp",
    
    # L2CAP (already has guards but we can add observer check)
    "NimBLEL2CAPChannel.cpp",
    "NimBLEL2CAPServer.cpp",
    
    # Beacons and special advertising
    "NimBLEBeacon.cpp",
    "NimBLEEddystoneTLM.cpp",
    
    # GATT descriptors
    "NimBLE2904.cpp",
    
    # Attributes (used by server/client)
    "NimBLEAttValue.cpp",
]

def add_observer_guard(filepath):
    """Add observer-only guard to a file if it doesn't already have one"""
    
    with open(filepath, 'r') as f:
        content = f.read()
    
    # Check if it already has our guard
    if "CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY" in content:
        print(f"Skipping {filepath} - already has observer guard")
        return False
        
    # Find where to insert the guard (after includes, before first real code)
    lines = content.split('\n')
    
    # Find the last include or the end of the license header
    insert_line = 0
    found_includes = False
    in_comment = False
    
    for i, line in enumerate(lines):
        if "/*" in line:
            in_comment = True
        if "*/" in line:
            in_comment = False
            insert_line = i + 1
        if not in_comment and line.strip().startswith('#include'):
            found_includes = True
            insert_line = i + 1
        elif found_includes and line.strip() and not line.strip().startswith('#'):
            # Found first non-include, non-empty line
            break
    
    # Look for existing #if defined(CONFIG_BT_ENABLED) style guards
    existing_guard_line = -1
    for i, line in enumerate(lines):
        if re.match(r'#if.*CONFIG_BT_ENABLED.*CONFIG_BT_NIMBLE', line):
            existing_guard_line = i
            break
    
    if existing_guard_line >= 0:
        # Add our guard to the existing condition
        lines[existing_guard_line] = lines[existing_guard_line].rstrip() + " && \\\n    !defined(CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY)"
        print(f"Modified existing guard in {filepath}")
    else:
        # Add new guard after includes
        guard = "\n#ifndef CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY\n"
        lines.insert(insert_line, guard)
        
        # Add closing endif
        lines.append("\n#endif // CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY\n")
        print(f"Added new guard to {filepath}")
    
    # Write back
    with open(filepath, 'w') as f:
        f.write('\n'.join(lines))
    
    return True

def main():
    src_dir = os.path.join(os.path.dirname(__file__), '..', 'src')
    
    modified_count = 0
    for filename in FILES_TO_WRAP:
        filepath = os.path.join(src_dir, filename)
        if os.path.exists(filepath):
            if add_observer_guard(filepath):
                modified_count += 1
        else:
            print(f"Warning: {filepath} not found")
    
    print(f"\nModified {modified_count} files")
    
if __name__ == "__main__":
    main()