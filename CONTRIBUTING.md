# [Flow](README.md) / Contributing

[![Back to Parent Page](https://img.shields.io/badge/-Back_to_Parent_Page-blue?style=for-the-badge)](README.md)

The project becomes better for everyone when people like you help make it better!

Have any questions or concerns? Did I forget something? Feel free to submit an issue or pull request.


Thanks for taking the time to contribute :)

---

## Development Setup

### Build Prerequisites

- Modern C++26 compliant toolchain with C++26 Standard Library Module support (LLVM / Clang ≥ 19)
- [XMake](https://xmake.io/) (v2.8+)

Always configure the project using LLVM and shared C++ runtime:
```console
xmake f --toolchain=llvm --runtimes="c++_shared"
```

To configure with unit tests enabled:
```console
xmake f --toolchain=llvm --runtimes="c++_shared" --enable_tests=y
```

---

## Development Workflow

1. Create a new branch from `master`.
2. Format branch names in `kebab-case`. Prefixing with a type (such as `feat/`, `fix/`, `ci/`, etc. See [Commit Messages](#commit-messages--pull-request-titles) below) is recommended, but optional (e.g., `feat/pipeline-routing`, `fix/quarantine-handling`).
3. Follow the [Conventional Commits](https://www.conventionalcommits.org/) convention for all commits and Pull Request titles (e.g., `feat(pipeline): add routing tag`, `fix(staging): handle dirty reads`).
4. Push your branch and open a Pull Request targeting `master`.
5. Ensure all CI checks pass before merging.

### Commit Messages & Pull Request Titles

Both commit messages and Pull Request titles must follow the [Conventional Commits](https://www.conventionalcommits.org/) specification format: `<type>[optional scope]: <description>` (e.g., `feat(engine): support parallel ingestion`, `fix(ui): correct elapsed duration formatting`).

Pull Request titles are automatically parsed by the CI to assign repository labels and categorize entries in the release changelog.

Supported and common types:
- `feat`: A new feature.
- `fix`: A bug fix.
- `docs`: Documentation updates only (e.g., README, code comments).
- `refactor`: Code refactoring that neither fixes a bug nor adds a feature.
- `perf`: A code change that improves performance.
- `test`: Adding missing tests or correcting existing tests.
- `build`: Changes that affect the build system or dependencies (e.g., `xmake.lua`).
- `ci`: Changes to CI/CD configurations and scripts (e.g., GitHub Actions workflows).
- `style`: Formatting or style adjustments that do not affect code logic (e.g., `.clang-format`).
- `chore`: Miscellaneous maintenance tasks, tooling updates, or release preparation.
- `revert`: Reverting a previous commit.

For breaking changes, append a `!` after the type/scope (e.g., `feat!: change pipeline tag syntax` or `feat(model)!: update UserRequestLog schema`).

---

## Local Dependency Overrides (`local_CppUtils`)

Flow depends on the [`CppUtils`](https://github.com/MorganCaron/CppUtils) library. By default, XMake retrieves `CppUtils` remotely from [`xmake-repo`](https://github.com/MorganCaron/xmake-repo).

When co-developing or testing local changes across `CppUtils` and `Flow` simultaneously (without publishing a new release or git tag to `xmake-repo`), you can point Flow to your local sibling repository `../CppUtils`.

### 1. Enable Local CppUtils

To link against the default sibling workspace directory (`../CppUtils`):
```console
xmake f -c --toolchain=llvm --runtimes="c++_shared" --local_CppUtils=y -y
```

Or specify an arbitrary custom path to `CppUtils`:
```console
xmake f -c --toolchain=llvm --runtimes="c++_shared" --local_CppUtils=/path/to/CppUtils -y
```

You can now build and run Flow directly with your local modifications in `CppUtils`:
```console
xmake build Flow
xmake run Flow
```

To run unit tests against the local `CppUtils`:
```console
xmake f --toolchain=llvm --runtimes="c++_shared" --local_CppUtils=y --enable_tests=y -y
xmake run Flow-UnitTests
```

### 2. Disable Local CppUtils (Switch Back to Remote `xmake-repo`)

To revert back to the remote `xmake-repo` package:
```console
xmake f -c --toolchain=llvm --runtimes="c++_shared" --local_CppUtils=n -y
```
Or simply reconfigure with a clean cache without the flag:
```console
xmake f -c --toolchain=llvm --runtimes="c++_shared" -y
```

> [!NOTE]
> Passing `-c` (clean reconfigure) is recommended when toggling `--local_CppUtils` to ensure cached package build artifacts are properly refreshed.

### 3. Alternative: Global Package Override with `xmake require --debugdir`

XMake provides a native command-line option to install a package directly from local sources into the package cache:
```console
xmake require -f --debugdir=/path/to/CppUtils -y CppUtils
# Or via xrepo:
xrepo install -d /path/to/CppUtils CppUtils
```
- **Pros**: Works without any project configuration options in `xmake.lua`.
- **Cons**: Compiles and installs a static snapshot into `~/.xmake/packages/`. Edits in `CppUtils` are not automatically picked up by Flow until you re-run `xmake require -f --debugdir=...`, and it can overwrite the cached release version globally for other projects.
- **Recommendation**: Use `--local_CppUtils` for day-to-day paired development because it is scoped to the project configuration and seamlessly accepts custom paths.

---

## Testing & Validation

Before submitting a pull request, ensure that the project builds, tests pass, and sample executions succeed:

1. Configure the build with test suite enabled:
   ```console
   xmake f --toolchain=llvm --runtimes="c++_shared" --enable_tests=y -y
   ```
2. Run the unit test suite:
   ```console
   xmake run Flow-UnitTests
   ```
3. Verify batch execution against sample datasets:
   ```console
   bash scripts/copy_samples.sh
   xmake run Flow
   ```
4. (Optional) Run tests continuously in watch mode during development:
   ```console
   xmake watch -r Flow-UnitTests
   ```
