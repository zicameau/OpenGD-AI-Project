# Distribution-Specific Notes

## Ubuntu/Debian
- Tested on Ubuntu 20.04 LTS and 22.04 LTS
- Compatible with Debian 11 (Bullseye)
- Uses APT package management

### Setup
```bash
# Install dependencies
sudo apt-get update
sudo apt-get install build-essential cmake
```

## Fedora
- Tested on Fedora 37 and 38
- Uses DNF package management

### Setup
```bash
# Install dependencies
sudo dnf update
sudo dnf install gcc-c++ cmake
```

## Arch Linux
- Rolling release compatibility
- Uses Pacman package management

### Setup
```bash
# Install dependencies
sudo pacman -Syu
sudo pacman -S base-devel cmake
```

## Known Issues

### Ubuntu
- PulseAudio latency on older versions
- NVIDIA driver compatibility on 22.04

### Fedora
- SELinux policy adjustments needed
- Wayland compatibility issues

### Arch Linux
- Package version conflicts
- AUR dependency management 