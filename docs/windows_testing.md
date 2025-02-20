# Testing OpenGD Linux Port on Windows

## Option 1: Windows Subsystem for Linux (WSL2) - Recommended

### Setup WSL2
1. Open PowerShell as Administrator and run:
```powershell
wsl --install
```

2. Restart your computer
3. Install Ubuntu from Microsoft Store (recommended) or run:
```powershell
wsl --install -d Ubuntu
```

### Setup Graphics Support
1. Install WSLg (Windows 11) or VcXsrv (Windows 10):
```powershell
# Windows 11: WSLg comes pre-installed
# Windows 10: Download and install VcXsrv from:
# https://sourceforge.net/projects/vcxsrv/
```

### Build and Test
1. Open Ubuntu terminal and install dependencies:
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libx11-dev \
    libgl1-mesa-dev \
    libpulse-dev \
    libasound2-dev \
    libevdev-dev \
    libgtest-dev
```

2. Clone and build:
```bash
git clone https://github.com/OpenGD/OpenGD.git
cd OpenGD
./scripts/run_tests.sh
```

## Option 2: Virtual Machine

### Using VirtualBox
1. Download and install VirtualBox:
   - https://www.virtualbox.org/wiki/Downloads

2. Download Ubuntu ISO:
   - https://ubuntu.com/download/desktop

3. Create VM:
   - RAM: 4GB minimum
   - CPU: 2 cores minimum
   - Video Memory: 128MB minimum
   - Enable 3D Acceleration
   - Storage: 20GB minimum

4. Install Guest Additions:
```bash
sudo apt-get update
sudo apt-get install -y virtualbox-guest-x11
```

### Using VMware
1. Download VMware Workstation Player:
   - https://www.vmware.com/products/workstation-player.html

2. Create Ubuntu VM:
   - Enable 3D Acceleration
   - Install VMware Tools

## Option 3: Docker Desktop

### Setup
1. Install Docker Desktop:
   - https://www.docker.com/products/docker-desktop/

2. Enable WSL2 backend in Docker settings

### Run Tests
```powershell
# Clone repository
git clone https://github.com/OpenGD/OpenGD.git
cd OpenGD

# Run distribution tests
.\scripts\test_all_distributions.sh
```

## Comparison of Methods

### WSL2
Pros:
- Native Linux performance
- Easy setup
- Direct file system access
- Good graphics support (WSLg)

Cons:
- Some hardware limitations
- Audio can be tricky
- Windows 11 recommended

### Virtual Machine
Pros:
- Full Linux environment
- Complete hardware isolation
- Better for debugging
- Works on all Windows versions

Cons:
- Higher resource usage
- Slower performance
- More complex setup

### Docker
Pros:
- Lightweight
- Good for CI/CD testing
- Consistent environment

Cons:
- Limited graphics support
- No direct hardware access
- Not ideal for development

## Recommended Workflow

1. **Development (WSL2)**
```bash
# In WSL2 Ubuntu
cd OpenGD
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
./OpenGDTests
```

2. **Distribution Testing (Docker)**
```bash
# In PowerShell
cd OpenGD
.\scripts\test_all_distributions.sh
```

3. **Full System Testing (VM)**
- Use VirtualBox/VMware for complete system testing
- Test different distributions
- Verify hardware compatibility 