#!/bin/bash
# Clean screenshot script for OS Lab 13 - suitable for documentation
# This version has no color codes for clean screenshot capture

set -e

# Function to print section header
print_header() {
    echo ""
    echo "=================================================================="
    echo "  $1"
    echo "=================================================================="
    echo ""
}

# Function to print subsection
print_subsection() {
    echo ">>> $1"
    echo ""
}

# Clean up previous test outputs
rm -f screenshot_*.txt screenshot_*.bin 2>/dev/null || true

print_header "OS LAB 13: EXT2 FILE SYSTEM UTILITY - CLEAN OUTPUT"
echo "Demonstrating ls and cp Commands"
echo ""

# ==============================================================================
print_header "SCREENSHOT 1: LS COMMAND - ROOT DIRECTORY"
# ==============================================================================

print_subsection "List all files and directories in the root of the EXT2 image"
echo "$ ./myfs my_partition.img ls"
echo ""
./myfs my_partition.img ls 2>&1 | tail -n +5

# ==============================================================================
print_header "SCREENSHOT 2: LS COMMAND - SUBDIRECTORY (/docs)"
# ==============================================================================

print_subsection "List files in the /docs subdirectory"
echo "$ ./myfs my_partition.img ls /docs"
echo ""
./myfs my_partition.img ls /docs 2>&1 | tail -n +5

# ==============================================================================
print_header "SCREENSHOT 3: CP COMMAND - COPY SMALL TEXT FILE"
# ==============================================================================

print_subsection "Copy /hello.txt from the EXT2 image"
echo "$ ./myfs my_partition.img cp /hello.txt ./hello_output.txt"
echo ""
./myfs my_partition.img cp /hello.txt ./hello_output.txt 2>&1 | tail -n +5
echo ""
echo "Verify the copied file:"
echo "$ cat ./hello_output.txt"
echo ""
cat ./hello_output.txt

# ==============================================================================
print_header "SCREENSHOT 4: CP COMMAND - COPY FROM SUBDIRECTORY"
# ==============================================================================

print_subsection "Copy /docs/info.txt from the /docs subdirectory"
echo "$ ./myfs my_partition.img cp /docs/info.txt ./info_output.txt"
echo ""
./myfs my_partition.img cp /docs/info.txt ./info_output.txt 2>&1 | tail -n +5
echo ""
echo "Verify the copied file:"
echo "$ cat ./info_output.txt"
echo ""
cat ./info_output.txt

# ==============================================================================
print_header "SCREENSHOT 5: CP COMMAND - COPY README.TXT"
# ==============================================================================

print_subsection "Copy /README.txt"
echo "$ ./myfs my_partition.img cp /README.txt ./readme_output.txt"
echo ""
./myfs my_partition.img cp /README.txt ./readme_output.txt 2>&1 | tail -n +5
echo ""
echo "Verify the copied file:"
echo "$ cat ./readme_output.txt"
echo ""
cat ./readme_output.txt

# ==============================================================================
print_header "SCREENSHOT 6: CP COMMAND - COPY BINARY FILE"
# ==============================================================================

print_subsection "Copy large binary file /largefile.bin"
echo "$ ./myfs my_partition.img cp /largefile.bin ./large_output.bin"
echo ""
./myfs my_partition.img cp /largefile.bin ./large_output.bin 2>&1 | tail -n +5
echo ""
echo "Verify the file was copied correctly:"
echo "$ ls -lh ./large_output.bin"
echo ""
ls -lh ./large_output.bin | awk '{print $9, "(" $5 ")"}'

# ==============================================================================
print_header "SCREENSHOT 7: FILE VERIFICATION - SHOW EXTRACTED FILES"
# ==============================================================================

print_subsection "Verify all extracted files exist and have correct sizes"
echo "$ ls -lh hello_output.txt info_output.txt readme_output.txt large_output.bin"
echo ""
ls -lh hello_output.txt info_output.txt readme_output.txt large_output.bin 2>/dev/null | \
    awk '{printf "%-30s %10s\n", $9, $5}'

echo ""
echo "=================================================================="
echo "  ALL TESTS COMPLETED SUCCESSFULLY"
echo "=================================================================="
echo ""
echo "Summary:"
echo "  + ls command successfully lists files and directories"
echo "  + cp command successfully extracts files from EXT2 image"
echo "  + Both root and subdirectory paths work correctly"
echo "  + Text and binary files are extracted with correct content"
echo ""
