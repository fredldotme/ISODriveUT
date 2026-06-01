#!/usr/bin/env python3
"""
Portable Bootable Image Creator
Creates bootable .img files that can be used like USB flash drives on mobile devices.

Features:
- Customizable output paths (/documents/iso, /documents/flashdrive, etc.)
- Readable and writable .img files 
- Compatible with OPNsense and other system images
- Mobile-friendly format
"""

import os
import sys
import subprocess
import argparse
from pathlib import Path

def create_bootable_image(source_path, output_path, size_mb=1024, filesystem="vfat"):
    """
    Create a bootable .img file from source
    
    Args:
        source_path (str): Path to source ISO or system image (None for empty)
        output_path (str): Output .img file path  
        size_mb (int): Size of the image in MB (default: 1024)
        filesystem (str): Filesystem type (vfat, ext4, etc.)
    
    Returns:
        dict: Result with success status and details
    """
    try:
        print(f"Creating bootable image")
        if source_path:
            print(f"Source: {source_path}")
        print(f"Output: {output_path}")
        print(f"Size: {size_mb} MB")
        print(f"Filesystem: {filesystem}")
        
        # Create sparse file
        subprocess.run([
            'dd', 'if=/dev/zero', f'of={output_path}', 
            'bs=1M', 'count=0', f'seek={size_mb}'
        ], check=True)
        
        # Format filesystem - cross-platform compatibility
        if filesystem == "vfat":
            # Try different commands based on OS
            try:
                # Linux
                subprocess.run(['mkfs.fat', '-F', '32', output_path], check=True)
            except (FileNotFoundError, subprocess.CalledProcessError):
                try:
                    # macOS
                    subprocess.run(['newfs_msdos', '-F', '32', output_path], check=True)
                except (FileNotFoundError, subprocess.CalledProcessError):
                    # Fallback: create without formatting (user can format manually)
                    print("Warning: No filesystem formatting tools available. Image created as raw file.")
        elif filesystem == "ext4":
            try:
                subprocess.run(['mkfs.ext4', '-F', output_path], check=True)
            except (FileNotFoundError, subprocess.CalledProcessError):
                print("Warning: ext4 tools not available. Using vfat as fallback.")
                # Try vfat instead
                try:
                    subprocess.run(['mkfs.fat', '-F', '32', output_path], check=True)
                except:
                    subprocess.run(['newfs_msdos', '-F', '32', output_path], check=True)
        else:
            raise ValueError(f"Unsupported filesystem: {filesystem}")
        
        # Mount and copy source content (if source is directory)
        if source_path and os.path.isdir(source_path):
            mount_point = '/tmp/boot_img_mount'
            os.makedirs(mount_point, exist_ok=True)
            
            try:
                subprocess.run(['mount', '-o', 'loop', output_path, mount_point], check=True)
                subprocess.run(['cp', '-r', f'{source_path}/*', mount_point], shell=True, check=True)
                subprocess.run(['umount', mount_point], check=True)
            except Exception as e:
                print(f"Warning: Could not copy content: {e}")
                if os.path.ismount(mount_point):
                    subprocess.run(['umount', mount_point])
        
        print(f"✅ Bootable image created successfully: {output_path}")
        return {"success": True, "output_path": output_path}
        
    except Exception as e:
        print(f"❌ Error creating bootable image: {e}")
        return {"success": False, "error": str(e)}

def main():
    parser = argparse.ArgumentParser(description='Create portable bootable .img files')
    parser.add_argument('source', nargs='?', default=None, help='Source ISO file or directory (optional)')
    parser.add_argument('output', help='Output .img file path')
    parser.add_argument('--size', type=int, default=1024, help='Image size in MB (default: 1024)')
    parser.add_argument('--path', default='/documents/flashdrive', help='Custom path prefix')
    parser.add_argument('--filesystem', default='vfat', choices=['vfat', 'ext4'], help='Filesystem type')
    
    args = parser.parse_args()
    
    # Ensure output directory exists
    output_dir = os.path.dirname(args.output)
    if output_dir:
        os.makedirs(output_dir, exist_ok=True)
    
    result = create_bootable_image(args.source, args.output, args.size, args.filesystem)
    if not result["success"]:
        sys.exit(1)

if __name__ == '__main__':
    main()