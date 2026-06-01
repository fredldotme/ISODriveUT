# ISODriveUT

> Create bootable USB/drive images from ISO files — built for Ubuntu Touch.

ISODriveUT is a Qt5/QML desktop application that lets you select an ISO file and write it to a bootable `.img` file on your device. Designed primarily for Ubuntu Touch devices, it also works on any Linux desktop with Qt5 installed.

## Features

- **ISO → IMG conversion** — Select any `.iso` file and produce a portable, bootable `.img` image
- **Dual directory modes** — Works with `/documents/iso` (read-only ISO mount) or `/documents/flashdrive` (read-write USB emulation) layouts
- **ConfigFS support** — Uses the Linux ConfigFS USB gadget framework for proper USB mass-storage emulation on OTG-capable devices
- **UTI manager** — Handles ISO and IMG UTI (Uniform Type Identifier) matching via Qt's UTI system
- **Multi-filesystem output** — Supports VFAT and ext4 output filesystems
- **Bootable image generation** — Creates images that are recognized as real USB drives by target devices
- **OPNsense / system image compatible** — Works with OPNsense, pfSense, and other `.img`-based system images
- **i18n ready** — Ships translations for: DE, ES, FR, IT, NL, PL, PT, RU
- **Click packaging** — Native [Click app](https://click-docs.readthedocs.io/) for Ubuntu Touch

## Architecture

```
┌─────────────────────────────────┐
│         QML UI (main.qml)       │
│   File picker · Progress · Log   │
└──────────────┬──────────────────┘
               │
┌──────────────▼──────────────────┐
│     C++ Core (src/)              │
│  FileManager · CommandRunner     │
│  GenericIsoManager               │
│  ConfigFsIsoManager              │
│  UtIsoManager                    │
└──────────────┬──────────────────┘
               │
┌──────────────▼──────────────────┐
│   Python CLI (create_bootable_   │
│   img.py) — does the actual       │
│   dd + mkfs work                  │
└─────────────────────────────────┘
```

### Key Source Files

| File | Role |
|------|------|
| `src/ISODrive.cpp` | Application entry point |
| `src/filemanager.{h,cpp}` | ISO/IMG file discovery and management |
| `src/commandrunner.{h,cpp}` | Async shell command execution (dd, mkfs, losetup, etc.) |
| `src/genericisomanager.{h,cpp}` | Base ISO manager with common logic |
| `src/configfsisomanager.{h,cpp}` | ConfigFS USB gadget integration |
| `src/utisomanager.{h,cpp}` | Qt UTI matching for ISO/IMG types |
| `create_bootable_img.py` | Standalone Python CLI for image creation |

## Requirements

### Build (Desktop)

- Qt 5.12+ with QML and Quick module
- Qt DBus module
- `python3` (for `create_bootable_img.py`)
- `dpkg` and `dpkg-architecture` (for Click arch detection)
- A Click-compatible toolchain (for Ubuntu Touch packaging)

### Runtime

- Linux (x86\_64 or ARM64)
- `python3`, `dd`, `mkfs.vfat` / `mkfs.ext4`, `losetup`
- For USB OTG passthrough: a kernel with ConfigFS USB gadget support

## Build & Run (Desktop)

```bash
# Install Qt5 dependencies (Debian/Ubuntu)
sudo apt install qt5-qmake qtbase5-dev qtdeclarative5-dev \
  qttools5-dev-tools python3

# Build
qmake ISODriveUT.pro
make -j$(nproc)

# Run
./ISODriveUT
```

## Build & Run (Ubuntu Touch)

```bash
# Install Click tooling
sudo apt install click-dev cmake

# Build the Click package
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make

# Install on device (via adb or ssh)
click install ./com.example.ISODriveUT.click
```

## Python CLI Tool

`create_bootable_img.py` can be used standalone, independent of the Qt app:

```bash
# Minimal usage — 100 MB VFAT image
python3 create_bootable_img.py --output ./my_image.img --size 100

# ext4 filesystem, custom output path
python3 create_bootable_img.py \
  --output /media/user/flashdrive/custom.img \
  --size 200 \
  --filesystem ext4

# ISO source to bootable image (via dd)
python3 create_bootable_img.py \
  --source /path/to/ubuntu.iso \
  --output /documents/flashdrive/boot.img
```

Options:

| Flag | Default | Description |
|------|---------|-------------|
| `--output` | required | Output path for `.img` file |
| `--size` | `100` | Image size in MB (50–4096) |
| `--filesystem` | `vfat` | Filesystem: `vfat` or `ext4` |
| `--source` | none | Source ISO file to copy into image |

## Testing

```bash
python3 test_bootable_img.py
```

The test suite exercises the Python CLI module with mocked file operations and validates filesystem creation logic.

## Supported Use Cases

- **System installer USB** — Flash Ubuntu, Debian, OPNsense ISOs to an IMG
- **Portable lab environment** — Carry bootable images on your phone's storage
- **ConfigFS USB gadget** — Present the IMG as a USB mass storage device to a PC via OTG

## Contributing

Contributions welcome. Please:

1. Fork and create a feature branch
2. Run `python3 test_bootable_img.py` before opening a PR
3. Keep translations up to date — add new `.ts` files in `translations/` and run `lrelease`

## License

MIT License — see [LICENSE](./LICENSE) for details.
