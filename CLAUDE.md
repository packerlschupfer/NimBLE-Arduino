# Claude Code Instructions for NimBLE-Arduino Library

## Library Context
- Lightweight BLE library for Arduino based on Apache NimBLE stack
- Fork of ESP32 NimBLE with Arduino IDE compatibility
- Optimized for memory efficiency (~50% less flash, ~100KB less RAM than bluedroid)
- Recently enhanced with observer-only build support for passive scanning applications
- Supports ESP32 series and Nordic nRF5x (with n-able Arduino core)

## Recent Enhancements
- Observer-only build mode with ~46KB+ flash savings
- CONFIG_BT_NIMBLE_STORE_CONFIG_DISABLED for persistence removal
- Conditional compilation for L2CAP to support observer-only builds
- Encryption stub for builds without Security Manager
- Error code availability fixes for observer-only compilation

## Code Standards
- Follow existing NimBLE coding conventions
- Minimize dynamic memory allocation
- Use conditional compilation for optional features
- Maintain compatibility across all supported platforms
- Document configuration options clearly in nimconfig.h
- Use MYNEWT_VAL() macros for internal configuration

## Testing Requirements
- Test all build configurations (full, observer-only, etc.)
- Verify memory savings match documentation
- Test on ESP32 and document platform-specific issues
- Ensure examples compile and run correctly
- Validate long-duration scanning stability
- Check that disabled features fail gracefully

## Current Optimization Efforts
High Priority:
- [x] Store/persistence removal (CONFIG_BT_NIMBLE_STORE_CONFIG_DISABLED)
- [ ] Privacy/address resolution conditional compilation
- [ ] Whitelist feature conditional compilation
- [ ] Buffer pool optimization for observer-only

Medium Priority:
- [ ] Enhanced ATT (EATT) conditional compilation
- [ ] Timer/scheduling optimization
- [ ] Direct Test Mode (DTM) removal

Low Priority:
- [ ] UUID handling optimization
- [ ] iBeacon advertising removal
- [ ] Further subsystem modularization

## Git Commit Standards
- Use conventional commits (feat:, fix:, docs:, refactor:, perf:)
- Include clear descriptions of memory savings when applicable
- Reference issue numbers if applicable
- Keep commits atomic (one logical change per commit)
- Test compilation before committing

## Memory Optimization Guidelines
- Always measure flash/RAM impact of changes
- Document memory savings in commit messages
- Prefer compile-time configuration over runtime
- Use stub implementations for disabled features
- Minimize buffer allocations for unused features

## Configuration Philosophy
- Default configuration should work for most users
- Observer-only mode should be a simple one-line change
- Advanced users can fine-tune individual subsystems
- Configuration options should be well-documented
- Avoid breaking changes to existing configurations