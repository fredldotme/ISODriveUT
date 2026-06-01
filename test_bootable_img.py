#!/usr/bin/env python3
"""
Test script for bootable image creation
"""

import os
import sys
import tempfile
from pathlib import Path

# Add the current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from create_bootable_img import create_bootable_image

def test_empty_bootable_image():
    """Test creating an empty bootable image"""
    with tempfile.TemporaryDirectory() as temp_dir:
        output_path = os.path.join(temp_dir, "test_boot.img")
        
        # Test with default parameters
        result = create_bootable_image(
            source_path=None,
            output_path=output_path,
            size_mb=100,  # Smaller size for testing
            filesystem="vfat"
        )
        
        if result["success"]:
            print("✅ Empty bootable image test passed")
            # Verify file exists and has correct size
            if os.path.exists(output_path):
                size = os.path.getsize(output_path)
                expected_size = 100 * 1024 * 1024  # 100 MB
                if size == expected_size:
                    print("✅ File size verification passed")
                    return True
                else:
                    print(f"❌ File size mismatch: expected {expected_size}, got {size}")
            else:
                print("❌ Output file not created")
        else:
            print(f"❌ Test failed: {result.get('error', 'Unknown error')}")
        
        return False

def test_with_custom_path():
    """Test with custom output path"""
    test_path = "/tmp/documents/flashdrive/test.img"
    os.makedirs(os.path.dirname(test_path), exist_ok=True)
    
    result = create_bootable_image(
        source_path=None,
        output_path=test_path,
        size_mb=50,
        filesystem="vfat"
    )
    
    if result["success"]:
        print("✅ Custom path test passed")
        if os.path.exists(test_path):
            os.remove(test_path)
        return True
    else:
        print(f"❌ Custom path test failed: {result.get('error', 'Unknown error')}")
        return False

if __name__ == "__main__":
    print("Running bootable image tests...")
    
    success_count = 0
    total_tests = 2
    
    if test_empty_bootable_image():
        success_count += 1
    
    if test_with_custom_path():
        success_count += 1
    
    print(f"\nTest Results: {success_count}/{total_tests} passed")
    
    if success_count == total_tests:
        print("🎉 All tests passed! Ready for PR submission.")
        sys.exit(0)
    else:
        print("⚠️ Some tests failed. Need to fix before PR.")
        sys.exit(1)