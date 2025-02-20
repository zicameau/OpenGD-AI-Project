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

# Windows 10: 
# 1. Download and install VcXsrv from:
# https://sourceforge.net/projects/vcxsrv/
# 2. Configure VcXsrv:
#    - Display number: 0
#    - Start no client
#    - Extra settings: Disable access control
```

### Build and Test
1. Open Ubuntu terminal and install dependencies:
```bash
# Update package lists
sudo apt-get update

# Install required development packages
sudo apt-get install -y \
    build-essential \
    cmake \
    libx11-dev \
    libgl1-mesa-dev \
    libpulse-dev \
    libasound2-dev \
    libevdev-dev \
    libgtest-dev \
    libxrandr-dev \    # Added for display management
    libxinerama-dev \  # Added for multi-monitor support
    libxcursor-dev     # Added for cursor handling
```

2. Clone and build:
```bash
# Clone repository
git clone https://github.com/OpenGD/OpenGD.git
cd OpenGD

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build
make -j$(nproc)

# Run tests
./OpenGDTests
```

## Option 2: Virtual Machine

### Using VirtualBox
1. Download and install VirtualBox:
   - https://www.virtualbox.org/wiki/Downloads

2. Download Ubuntu ISO:
   - https://ubuntu.com/download/desktop

3. Create VM:
   - RAM: 8GB recommended (4GB minimum)
   - CPU: 4 cores recommended (2 minimum)
   - Video Memory: 128MB minimum
   - Enable 3D Acceleration
   - Storage: 40GB recommended (20GB minimum)
   - Enable EFI (required for some features)

4. Install Guest Additions:
```bash
sudo apt-get update
sudo apt-get install -y virtualbox-guest-x11
sudo reboot  # Reboot to apply changes
```

### Using VMware
1. Download VMware Workstation Player:
   - https://www.vmware.com/products/workstation-player.html

2. Create Ubuntu VM:
   - Enable 3D Acceleration
   - Install VMware Tools
   - Allocate sufficient resources (similar to VirtualBox recommendations)

## Option 3: Docker Desktop

### Setup
1. Install Docker Desktop:
   - https://www.docker.com/products/docker-desktop/

2. Enable WSL2 backend in Docker settings

3. Configure resource limits:
   - Memory: 8GB recommended
   - CPU: 4 cores recommended
   - Swap: 2GB minimum

### Run Tests
```powershell
# Clone repository
git clone https://github.com/OpenGD/OpenGD.git
cd OpenGD

# Build and run tests in container
docker build -t opengd-tests .
docker run --rm opengd-tests ./scripts/run_tests.sh

# Run distribution tests
./scripts/test_all_distributions.sh
```

## Comparison of Methods

### WSL2
Pros:
- Native Linux performance
- Easy setup on Windows 11
- Direct file system access
- Good graphics support (WSLg)
- Low resource overhead

Cons:
- Some hardware limitations
- Audio configuration can be complex
- Windows 11 recommended for best experience
- Limited GPU support on Windows 10

### Virtual Machine
Pros:
- Full Linux environment
- Complete hardware isolation
- Better for debugging
- Works on all Windows versions
- Full GPU passthrough support

Cons:
- Higher resource usage
- Slower performance
- More complex setup
- Requires more disk space

### Docker
Pros:
- Lightweight
- Good for CI/CD testing
- Consistent environment
- Easy to automate
- Version control friendly

Cons:
- Limited graphics support
- No direct hardware access
- Not ideal for development
- Complex networking setup

## Recommended Workflow

1. **Development (WSL2)**
```bash
# In WSL2 Ubuntu
cd OpenGD
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)

# Run tests
ctest --output-on-failure
./OpenGDTests

# Run with debugger
gdb ./OpenGD
```

2. **Distribution Testing (Docker)**
```bash
# In PowerShell
cd OpenGD

# Run tests across multiple distributions
./scripts/test_all_distributions.sh

# Check test results
cat test_results/*.xml
```

3. **Full System Testing (VM)**
- Use VirtualBox/VMware for complete system testing
- Test different distributions
- Verify hardware compatibility
- Profile performance
- Test multi-monitor setups

4. **Performance Monitoring**
```bash
# Install monitoring tools
sudo apt-get install -y htop iotop perf

# Monitor system resources
htop
iotop
perf record ./OpenGD
perf report
``` 