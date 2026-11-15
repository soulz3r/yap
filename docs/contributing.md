# Contributing and maintenance

YAP is under active development. Changes should be focused, understandable, and consistent with the current architecture.

## Before changing code

1. Read [architecture.md](architecture.md) to identify the owning layer.
2. Read [design-decisions.md](design-decisions.md) before changing module boundaries.
3. Check [project-generation.md](project-generation.md) before modifying the project-plan or template APIs.
4. Check [logger.md](logger.md) before changing diagnostic behavior.
5. Check [usage.md](usage.md) before changing command names, arguments, or exit behavior.

## Keep changes focused

- Avoid unrelated formatting changes.
- Preserve existing naming and code conventions unless the change explicitly addresses them.
- Prefer small functions with clear contracts.
- Keep public declarations in the matching public header.
- Keep private implementation details out of public headers.
- Validate pointers, sizes, and enum values at API boundaries.
- Make ownership and lifetime requirements explicit.

## Layer responsibilities

### CLI

The CLI owns argument parsing, command dispatch, help text, and user-facing terminal output. It should translate requests into core API calls and translate core results into user-facing messages and exit statuses.

### Core

Core components should not depend on argv or terminal presentation. Prefer explicit inputs and structured return values.

### Logger

Use the logger for diagnostics. Do not use it as a substitute for normal output or an operation's return status.

### Filesystem

Keep filesystem operations explicit. The current helpers create one directory or operate on one file; they do not recursively create parent directories.

## Platform changes

The supplied build scripts target Linux and 64-bit Windows. Keep platform-specific APIs behind the platform definitions and update both builds when a shared source change affects both platforms.

A successful compile on one platform does not establish support on another. Test the affected feature on each platform before claiming parity.

## Public API changes

When changing a public API:

- Update the public header.
- Update implementation and all call sites.
- Document argument requirements and ownership.
- Document return statuses and failure behavior.
- Update examples in the relevant guide.
- Consider whether the change affects platform-specific behavior.

Avoid documenting an API as stable while its signature or contract is still changing.

## Documentation changes

Documentation is part of the feature. Update the relevant page when behavior changes.

- User-facing command behavior belongs in [usage.md](usage.md).
- Build requirements and flags belong in [building.md](building.md).
- Module boundaries belong in [architecture.md](architecture.md).
- Logger behavior belongs in [logger.md](logger.md).
- Project and template API contracts belong in [project-generation.md](project-generation.md).
- Design rationale belongs in [design-decisions.md](design-decisions.md).

Always distinguish implemented behavior from planned work.

## Validation

Before considering a change complete:

- [ ] Build with the relevant platform script.
- [ ] Confirm public declarations match implementations.
- [ ] Check error paths and return statuses.
- [ ] Check pointer, size, ownership, and lifetime assumptions.
- [ ] Confirm diagnostics and user-facing output remain separate.
- [ ] Update documentation.
- [ ] Run relevant tests if they exist.

There is not yet a repository-wide automated test suite documented in the current project. Do not claim tests passed unless they were actually run. For documentation-only changes, review links, code fences, examples, and statements against the current source.
