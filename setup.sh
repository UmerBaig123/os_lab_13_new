#!/bin/bash

# Script to set up the lab environment and create a test EXT2 disk image

set -e

echo "=== EXT2 Lab Setup ==="

# Compile the program
echo "Building myfs..."
make clean
make

echo ""
echo "Creating 200MB EXT2 disk image..."

# Create a 200MB raw disk image
dd if=/dev/zero of=my_partition.img bs=1M count=200 2>/dev/null

# Format it with EXT2 file system
mkfs.ext2 -F my_partition.img 2>/dev/null

echo "Disk image created: my_partition.img"
echo ""

# Mount the image to add test files
MOUNT_POINT="/tmp/ext2_mount_$$"
mkdir -p "$MOUNT_POINT"

echo "Mounting image to: $MOUNT_POINT"
sudo mount -o loop my_partition.img "$MOUNT_POINT" || {
    echo "Warning: Could not mount image. You may need to run with sudo."
    rm -rf "$MOUNT_POINT"
    exit 1
}

# Create test files and directories
echo "Creating test files and directories..."

# Create some test directories
sudo mkdir -p "$MOUNT_POINT/docs"
sudo mkdir -p "$MOUNT_POINT/bin"
sudo mkdir -p "$MOUNT_POINT/data"

# Create test files
sudo bash -c "echo 'This is the README file.' > '$MOUNT_POINT/README.txt'"
sudo bash -c "echo 'Hello from EXT2 filesystem!' > '$MOUNT_POINT/hello.txt'"
sudo bash -c "echo 'Test data file content' > '$MOUNT_POINT/data/test.dat'"
sudo bash -c "echo 'Documentation example' > '$MOUNT_POINT/docs/info.txt'"

# Create a larger file for testing
sudo bash -c "head -c 10000 /dev/urandom | base64 > '$MOUNT_POINT/largefile.bin'"

echo "Test files created:"
sudo ls -la "$MOUNT_POINT"

# Unmount the image
echo ""
echo "Unmounting image..."
sudo umount "$MOUNT_POINT"
rm -rf "$MOUNT_POINT"

echo ""
echo "=== Setup Complete ==="
echo "Disk image is ready at: my_partition.img"
echo ""
echo "Test the program with:"
echo "  ./myfs my_partition.img ls"
echo "  ./myfs my_partition.img ls /docs"
echo "  ./myfs my_partition.img cp /hello.txt ./hello_extracted.txt"
