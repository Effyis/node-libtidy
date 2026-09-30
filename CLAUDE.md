# node-libtidy

## Purpose

A fork of [gagern/node-libtidy](https://github.com/gagern/node-libtidy), republished as `@effyis/libtidy`, providing native Node bindings to [libtidy](http://www.html-tidy.org/developer/) for parsing and cleaning up malformed HTML. It is a native addon: the HTML Tidy C sources are vendored as a git submodule and compiled at install time, with prebuilt binaries available for common platforms so consumers usually do not build anything. The fork exists because upstream has been dormant since 2017 and did not run on modern Node; it is used by [pauk](../pauk) to tidy fetched pages before parsing, so when it fails to build the symptom is an install failure rather than a runtime one.

## Where it sits in Socialgist

Acquisition/crawl — a shared library, not a service. It has no upstream or downstream Socialgist systems. Its consumer is [pauk](../pauk), which declares `@effyis/libtidy` and uses it in `components/utils/html.js` to normalize HTML before the document layer parses it. Published to the internal Verdaccio registry, with prebuilt binaries served from this repo's own GitHub releases rather than upstream's.

## Key concepts and domain vocabulary

- **Fork delta** — four Effyis commits on top of upstream: Node 20 support, the package rename and scoping, an `npm audit fix`, and the 0.5.1 version bump. Upstream's last release was 0.3.8.
- **`binary.remote_path`** — the `package.json` field pointing node-pre-gyp at `Effyis/node-libtidy` GitHub releases. This is the load-bearing part of the rename: prebuilt binaries must exist there under a matching `v{version}` tag or every install falls back to compiling from source.
- **`tidy-html5`** — a git *submodule*, not vendored files. `binding.gyp` compiles its `src/*.c` directly into the addon, so the submodule must be initialized before any source build can work.
- **node-pre-gyp** — the install-time mechanism: `npx node-pre-gyp install --fallback-to-build` tries the prebuilt binary for your `{node_abi}-{platform}-{arch}` and compiles only if none matches. The fork moved from `node-pre-gyp` to the maintained `@mapbox/node-pre-gyp`.
- **`tidyBuffer(input, [opts], [cb])`** — the suggested entry point per `API.md`, asynchronous, with the callback receiving `{ output, errlog }`. The `TidyDoc` class is the lower-level, fine-grained interface.
- **Generated declarations** — `src/index.d.ts` is produced by `util/gen-typescript-decl.js`, not written by hand.

## Architecture

- `src/node-libtidy.cc`, `doc.cc`, `opt.cc`, `memory.cc`, `worker.cc` (with their `.hh` headers) — the C++ addon: document handling, option handling, a custom allocator, and the async worker that keeps tidying off the event loop.
- `src/index.js` and `src/TidyDoc.js` — the JavaScript surface: the convenience functions and the `TidyDoc` wrapper around the native document.
- `src/htmltidy.js` — a drop-in compatibility shim for the `htmltidy` / `htmltidy2` packages, so those can be swapped out without changing calling code.
- `binding.gyp` — the build definition, compiling the addon sources together with `tidy-html5/src/*.c`.
- `util/gen-typescript-decl.js` — regenerates `src/index.d.ts`; wired into both `prepublish` and `pretest`, so the declarations are rebuilt before every test run.

## Running it locally

- **Initialize the submodule first**: `git submodule update --init`. It is currently uninitialized in this checkout, and without it a source build fails on missing `tidy-html5/src/*.c`.
- `npm install` runs `npx node-pre-gyp install --fallback-to-build`, which needs network access to GitHub for the prebuilt lookup and a native toolchain (python3, make, a C/C++ compiler) if it falls back to compiling.
- `npm test` runs `pretest` (regenerating the TypeScript declarations) then `mocha --require ts-node/register` over `test/`. `npm run test-trace` is the same with `--trace-deprecation`.
- No environment variables are read. `.npmrc` scopes `@effyis` to `verdaccio.sgdctroy.net` and sets `preid=rc`.
- `engines` is unset, but the fork's whole point is Node 20; `@types/node` is pinned to 20.

## Conventions and constraints

- **Keep the fork delta small.** Upstream is dormant, so there is no merge pressure, but the value here is a thin modernization layer rather than a divergent codebase.
- **Releases must publish binaries.** Bumping the version without cutting a matching `v{version}` GitHub release on `Effyis/node-libtidy` leaves `binary.remote_path` pointing at nothing, and every consumer silently switches to compiling from source. `release.md` documents the release procedure.
- **`.travis.yml` and `appveyor.yml` are dead upstream leftovers**, configured for Node 4–9 and x86, from a time when they built the prebuilt binaries. Neither service runs; they are not the current release mechanism and should not be trusted as documentation of it.
- **`appveyor.yml` contains an encrypted upstream GitHub release token.** It belongs to upstream's account, is AppVeyor-encrypted, and is inert here.
- `README.md` and `API.md` are upstream's and document the package as `libtidy` — `require("libtidy")`, not the scoped name. Consumers use `@effyis/libtidy`.
- `src/index.d.ts` is generated. Edit `util/gen-typescript-decl.js` instead, and note `pretest` regenerates it, so a hand-edit vanishes on the next test run.
- The upstream authorship and MIT licence are retained and correct; this is a fork, not a rewrite.
- **`libtidy-updated` in pauk's `package.json` is a different, unrelated npm package** and is now dead there — `components/utils/html.js` requires this one.

## Data handling

No customer data, PII, or licensed source content is stored by this library. It is a pure transformation: HTML goes in as a buffer, tidied HTML and an error log come out, with nothing persisted, cached or logged. The HTML passing through it in pauk's use is crawled third-party page content, so licensed source material transits it in memory, but the library itself neither retains nor transmits anything. Being a native addon, it does execute compiled C code over untrusted input — malformed HTML from arbitrary sites — which is worth knowing when deciding how current to keep the vendored `tidy-html5`. No retention or compliance policy is documented in this repo.

## Testing and CI

No working CI exists for this repo — `.travis.yml` and `appveyor.yml` are dead upstream configuration, and there is no `.drone.yml`, `.github/workflows/` or `Jenkinsfile`. Nothing runs on a PR, so `npm test` locally is the only gate. That suite is mocha over `test/`, with `pretest` regenerating the TypeScript declarations first: `doc-test.js` and `opt-test.js` cover the `TidyDoc` and option surfaces, `htmltidy-test.js` the compatibility shim against the real `htmltidy2` package, and `ts-decl-test.ts` type-checks the generated declarations. Manual verification after any change touching the native sources or the submodule: delete `lib/`, run a full `--build-from-source` install and then `npm test`, since a prebuilt binary will otherwise mask a broken build entirely. Before releasing, confirm the GitHub release with the matching `v{version}` tag carries binaries for the platforms consumers actually install on.

## Ownership

Primary maintainer and contact: Aleksandar Mitic (amitic@socialgist.com).

Owner: Aleksandar Mitic, CLAUDE.md last updated: 2026-09-29
