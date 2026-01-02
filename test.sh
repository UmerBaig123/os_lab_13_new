#!/bin/bash
# Test script for the EXT2 file system utility

set -e

echo "========================================="
echo "EXT2 File System Utility - Test Suite"
echo "========================================="
echo ""

# Clean up previous test outputs
rm -f test_*.txt test_*.bin 2>/dev/null || true

echo "Test 1: List Root Directory"
echo "Command: ./myfs my_partition.img ls"
./myfs my_partition.img ls 2>&1 | grep -E "(Name|Type|hello|README|largefile)"
echo ""

echo "Test 2: List /docs Subdirectory"
echo "Command: ./myfs my_partition.img ls /docs"
./myfs my_partition.img ls /docs 2>&1 | grep -E "(Name|Type|info\.txt)"
echo ""

echo "Test 3: Copy /hello.txt"
echo "Command: ./myfs my_partition.img cp /hello.txt ./test_hello.txt"
./myfs my_partition.img cp /hello.txt ./test_hello.txt 2>&1 | grep "File copied"
if [ -f test_hello.txt ]; then
    CONTENT=$(cat test_hello.txt)
    echo "✓ File created successfully"
    echo "  Content: '$CONTENT'"
else
    echo "✗ File not created"
    exit 1
fi
echo ""

echo "Test 4: Copy /docs/info.txt"
echo "Command: ./myfs my_partition.img cp /docs/info.txt ./test_info.txt"
./myfs my_partition.img cp /docs/info.txt ./test_info.txt 2>&1 | grep "File copied"
if [ -f test_info.txt ]; then
    CONTENT=$(cat test_info.txt)
    echo "✓ File created successfully"
    echo "  Content: '$CONTENT'"
else
    echo "✗ File not created"
    exit 1
fi
echo ""

echo "Test 5: Copy /README.txt"
echo "Command: ./myfs my_partition.img cp /README.txt ./test_readme.txt"
./myfs my_partition.img cp /README.txt ./test_readme.txt 2>&1 | grep "File copied"
if [ -f test_readme.txt ]; then
    SIZE=$(wc -c < test_readme.txt)
    echo "✓ File created successfully ($SIZE bytes)"
else
    echo "✗ File not created"
    exit 1
fi
echo ""

echo "Test 6: Copy /largefile.bin"
echo "Command: ./myfs my_partition.img cp /largefile.bin ./test_large.bin"
./myfs my_partition.img cp /largefile.bin ./test_large.bin 2>&1 | grep "File copied"
if [ -f test_large.bin ]; then
    SIZE=$(ls -lh test_large.bin | awk '{print $5}')
    echo "✓ File created successfully ($SIZE)"
else
    echo "✗ File not created"
    exit 1
fi
echo ""

echo "========================================="
echo "All tests passed!"
echo "========================================="
echo ""
echo "Test files created:"
ls -lh test_*.* 2>/dev/null | awk '{print "  " $9 " (" $5 ")"}'
