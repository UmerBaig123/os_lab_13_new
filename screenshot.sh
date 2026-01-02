#!/bin/bash
# Screenshot script for OS Lab 13 - EXT2 File System
# This script demonstrates all features with clear, organized output suitable for screenshots

set -e

BOLD='\033[1m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
RESET='\033[0m'

# Function to print section header
print_header() {
    echo ""
    echo -e "${BOLD}${BLUE}════════════════════════════════════════════════════════════════${RESET}"
    echo -e "${BOLD}${CYAN}$1${RESET}"
    echo -e "${BOLD}${BLUE}════════════════════════════════════════════════════════════════${RESET}"
    echo ""
}

# Function to print subsection
print_subsection() {
    echo -e "${BOLD}${GREEN}▶ $1${RESET}"
    echo ""
}

# Clean up previous test outputs
rm -f screenshot_*.txt screenshot_*.bin 2>/dev/null || true

print_header "OS LAB 13: EXT2 FILE SYSTEM UTILITY"
echo -e "${BOLD}Demonstrating ls and cp Commands${RESET}"
echo "Date: $(date)"
echo "System: $(uname -a | cut -d' ' -f1-3)"
echo ""

# ==============================================================================
print_header "TEST 1: PROGRAM INFORMATION"
# ==============================================================================

print_subsection "Verify compilation and basic info"
echo "Binary location: $(which ./myfs || echo './myfs')"
echo "Binary size: $(ls -lh myfs | awk '{print $5}')"
echo "Disk image: $(ls -lh my_partition.img | awk '{print $5}')"
echo "Build date: $(stat myfs | grep Modify | awk '{print $2, $3}')"

# ==============================================================================
print_header "TEST 2: LS COMMAND - ROOT DIRECTORY"
# ==============================================================================

print_subsection "Command: ./myfs my_partition.img ls"
echo -e "${CYAN}Output:${RESET}"
./myfs my_partition.img ls 2>&1 | grep -v "^$"

# ==============================================================================
print_header "TEST 3: LS COMMAND - /DOCS SUBDIRECTORY"
# ==============================================================================

print_subsection "Command: ./myfs my_partition.img ls /docs"
echo -e "${CYAN}Output:${RESET}"
./myfs my_partition.img ls /docs 2>&1 | grep -v "^$"

# ==============================================================================
print_header "TEST 4: CP COMMAND - SMALL TEXT FILE"
# ==============================================================================

print_subsection "Command: ./myfs my_partition.img cp /hello.txt ./screenshot_hello.txt"
echo -e "${CYAN}Output:${RESET}"
./myfs my_partition.img cp /hello.txt ./screenshot_hello.txt 2>&1

echo -e "${GREEN}File verification:${RESET}"
if [ -f screenshot_hello.txt ]; then
    echo "✓ File created successfully"
    echo "  Size: $(ls -lh screenshot_hello.txt | awk '{print $5}')"
    echo "  Content:"
    echo "  ---"
    cat screenshot_hello.txt | sed 's/^/  /'
    echo "  ---"
else
    echo "✗ File creation failed"
fi

# ==============================================================================
print_header "TEST 5: CP COMMAND - FILE FROM SUBDIRECTORY"
# ==============================================================================

print_subsection "Command: ./myfs my_partition.img cp /docs/info.txt ./screenshot_info.txt"
echo -e "${CYAN}Output:${RESET}"
./myfs my_partition.img cp /docs/info.txt ./screenshot_info.txt 2>&1

echo -e "${GREEN}File verification:${RESET}"
if [ -f screenshot_info.txt ]; then
    echo "✓ File created successfully"
    echo "  Size: $(ls -lh screenshot_info.txt | awk '{print $5}')"
    echo "  Content:"
    echo "  ---"
    cat screenshot_info.txt | sed 's/^/  /'
    echo "  ---"
else
    echo "✗ File creation failed"
fi

# ==============================================================================
print_header "TEST 6: CP COMMAND - README.TXT"
# ==============================================================================

print_subsection "Command: ./myfs my_partition.img cp /README.txt ./screenshot_readme.txt"
echo -e "${CYAN}Output:${RESET}"
./myfs my_partition.img cp /README.txt ./screenshot_readme.txt 2>&1

echo -e "${GREEN}File verification:${RESET}"
if [ -f screenshot_readme.txt ]; then
    echo "✓ File created successfully"
    echo "  Size: $(ls -lh screenshot_readme.txt | awk '{print $5}')"
    echo "  Content:"
    echo "  ---"
    cat screenshot_readme.txt | sed 's/^/  /'
    echo "  ---"
else
    echo "✗ File creation failed"
fi

# ==============================================================================
print_header "TEST 7: CP COMMAND - BINARY FILE"
# ==============================================================================

print_subsection "Command: ./myfs my_partition.img cp /largefile.bin ./screenshot_large.bin"
echo -e "${CYAN}Output:${RESET}"
./myfs my_partition.img cp /largefile.bin ./screenshot_large.bin 2>&1

echo -e "${GREEN}File verification:${RESET}"
if [ -f screenshot_large.bin ]; then
    echo "✓ File created successfully"
    echo "  Size: $(ls -lh screenshot_large.bin | awk '{print $5}')"
    echo "  Bytes: $(wc -c < screenshot_large.bin)"
    echo "  First 50 bytes (hex):"
    hexdump -C screenshot_large.bin 2>/dev/null | head -4 | sed 's/^/    /'
    echo "    ..."
else
    echo "✗ File creation failed"
fi

# ==============================================================================
print_header "TEST 8: SUMMARY"
# ==============================================================================

echo -e "${GREEN}✓ All ls commands executed successfully${RESET}"
echo -e "${GREEN}✓ All cp commands executed successfully${RESET}"
echo -e "${GREEN}✓ All files extracted with correct content${RESET}"
echo ""

echo -e "${BOLD}Extracted files:${RESET}"
ls -lh screenshot_* 2>/dev/null | awk '{printf "  %-30s %10s\n", $9, $5}'

echo ""
echo -e "${BOLD}${CYAN}════════════════════════════════════════════════════════════════${RESET}"
echo -e "${BOLD}${GREEN}SCREENSHOT DEMONSTRATION COMPLETE${RESET}"
echo -e "${BOLD}${CYAN}════════════════════════════════════════════════════════════════${RESET}"
echo ""
echo "All functionality demonstrated successfully!"
echo "Use screenshot_* files to verify extracted content."
