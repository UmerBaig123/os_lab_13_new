#!/bin/bash

# Simple test setup script (non-root version)
# This creates a test EXT2 disk image with sample files

set -e

echo "=== EXT2 Lab Test Setup ==="

# Create a 200MB raw disk image
echo "Creating 200MB EXT2 disk image..."
dd if=/dev/zero of=my_partition.img bs=1M count=200 2>/dev/null

# Format it with EXT2 file system
echo "Formatting as EXT2..."
mkfs.ext2 -F my_partition.img 2>/dev/null

echo "Disk image created: my_partition.img"
echo ""

# Mount the image to add test files
MOUNT_POINT="/tmp/ext2_mount_$$"
mkdir -p "$MOUNT_POINT"

echo "Mounting image to: $MOUNT_POINT"
if sudo mount -o loop my_partition.img "$MOUNT_POINT" 2>/dev/null; then
    
    # Create test files and directories
    echo "Creating test files and directories..."
    
    # Create some test directories
    sudo mkdir -p "$MOUNT_POINT/docs" 2>/dev/null
    sudo mkdir -p "$MOUNT_POINT/bin" 2>/dev/null
    sudo mkdir -p "$MOUNT_POINT/data" 2>/dev/null
    
    # Create test files with content
    echo "This is the README file." | sudo tee "$MOUNT_POINT/README.txt" >/dev/null 2>&1
    echo "Hello from EXT2 filesystem!" | sudo tee "$MOUNT_POINT/hello.txt" >/dev/null 2>&1
    echo "Test data file content" | sudo tee "$MOUNT_POINT/data/test.dat" >/dev/null 2>&1
    echo "Documentation example" | sudo tee "$MOUNT_POINT/docs/info.txt" >/dev/null 2>&1
    
    # Create a larger file for testing
    echo "Creating larger test file..."
    head -c 10000 /dev/urandom | base64 | sudo tee "$MOUNT_POINT/largefile.bin" >/dev/null 2>&1
    
    echo ""
    echo "Test files created:"
    sudo ls -la "$MOUNT_POINT" 2>/dev/null | tail -7
    
    # Unmount the image
    echo ""
    echo "Unmounting image..."
    sudo umount "$MOUNT_POINT" 2>/dev/null
else
    echo "Warning: Could not mount image. Continuing with empty EXT2 image."
fi

rm -rf "$MOUNT_POINT" 2>/dev/null || true

echo ""
echo "=== Setup Complete ==="
echo "Disk image is ready at: my_partition.img"
echo ""
echo "Test the program with:"
echo "  ./myfs my_partition.img ls"
echo "  ./myfs my_partition.img ls /docs"
echo "  ./myfs my_partition.img cp /hello.txt ./hello_extracted.txt"
