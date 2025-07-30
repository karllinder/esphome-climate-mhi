# Contributing to ESPHome Climate MHI

Thank you for your interest in contributing to this project! This document provides guidelines for contributions.

## How to Contribute

### Reporting Issues
- Check if the issue already exists before creating a new one
- Include your ESPHome configuration (remove sensitive data)
- Provide ESPHome logs if relevant
- Specify your MHI AC model and remote control type

### Submitting Changes

1. **Fork the repository**
2. **Create a feature branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```
3. **Make your changes**
   - Follow the existing code style
   - Test your changes with a real device if possible
   - Update documentation if needed

4. **Commit your changes**
   ```bash
   git commit -m "Add feature: description of your changes"
   ```

5. **Push to your fork**
   ```bash
   git push origin feature/your-feature-name
   ```

6. **Create a Pull Request**
   - Provide a clear description of the changes
   - Reference any related issues
   - Include test results if applicable

## Code Guidelines

- Follow ESPHome's coding standards
- Comment complex logic
- Ensure backward compatibility
- Test with ESPHome's latest stable version

## Testing

Before submitting:
1. Clean build: `esphome clean your-device.yaml`
2. Compile: `esphome compile your-device.yaml`
3. Test all affected functions on actual hardware
4. Verify Home Assistant integration works correctly

## Questions?

Open an issue for discussion before making major changes.