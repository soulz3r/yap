# Design decisions

YAP is still evolving, so these notes describe the principles behind the current structure and distinguish decisions from future plans.

## Keep the interface separate from the work

The CLI should collect user intent, validate command-line syntax, call core functionality, and present the result. It should not own reusable project-generation logic.

The planned TUI should use the same core functionality. This avoids implementing project creation twice just because the input method changes.

This is a design direction. The current command handlers are not yet connected to a complete project-generation workflow.

## One project workflow, multiple interfaces

The intended architecture has one project workflow. The CLI and future TUI are interfaces that construct the same request/configuration.

The core should not inspect argv, print terminal-specific formatting, or depend on a particular UI library. It should accept explicit data and return structured results.

## The template renderer is not the whole generator

A template renderer transforms text. A project generator decides what the project should contain, resolves user configuration, selects the appropriate template material, builds a plan, and coordinates filesystem operations.

YAP currently has a variable renderer and a plan executor as separate components. Neither one is a complete generator, and neither is currently connected to the `new` command.

Keeping this distinction clear prevents the project architecture from becoming a thin wrapper around string replacement.

## Plans describe operations before they execute

The project-plan API represents directory creation and file writing as ordered operations. This separates the description of work from the code that performs filesystem operations.

The current executor applies operations sequentially and can leave partial output on failure. It does not currently provide rollback or transactional filesystem semantics.

## Core APIs return results

Core functions should communicate success and failure through return values or result structures. The interface can then decide how to display the outcome.

This makes the same operation usable from a CLI, a future TUI, or a test without embedding presentation logic in the core.

## Logger is diagnostic-only

The logger records diagnostic information. It should not be used as the normal command-output system.

For example, a failed directory creation can be logged with internal context while the filesystem helper returns failure. The CLI can then present a useful explanation to the user.

## Caller-owned memory for current APIs

The project-plan and template-renderer APIs currently use caller-owned arrays, strings, and buffers. This keeps ownership explicit and avoids hidden allocation behavior.

Any future allocation-owning API should document who allocates, who frees, and how long returned data remains valid.

## C23 and explicit platform code

YAP targets C23 and uses Clang in its supplied build scripts. Platform-specific behavior is selected through compile-time definitions.

The presence of a platform macro is not proof of full platform support. Features need real builds and behavior checks on each supported target.

## Document current behavior, not intended behavior

The CLI dispatcher currently registers commands that are placeholders. Documentation must label those commands as unfinished rather than inventing flags or claiming they work.

As implementations change, update the relevant usage and architecture pages in the same change.
