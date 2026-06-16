# Agents notes

MCBooster is a **header-only** C++11 library for fast generation of phase-space Monte Carlo events (particle decays, up to nine final-state particles). It is built on [Thrust](https://thrust.github.io/), so the same source compiles to CUDA (GPU), OpenMP, TBB, or single-threaded CPP backends. The algorithm follows ROOT's `TGenPhaseSpace` / CERNLIB GENBOD (Raubold–Lynch method).

This is the **GooFit fork** of the original MultithreadCorner/MCBooster. There are no tagged releases here; it tracks GooFit's needs.

Library lives in `mcbooster/` (all headers). `src/` holds example programs, not library code.

## Build & run

The library itself needs no build — just put `mcbooster/` on the include path. The CMake build only compiles the examples in `src/`.

```bash
mkdir build && cd build
cmake ../          # prints the install prefix; add -DCMAKE_INSTALL_PREFIX=<path> for non-root
make
```

Requirements for examples: Thrust ≥1.8, [ROOT](https://root.cern.ch/), [TCLAP](http://tclap.sourceforge.net/) (all `find_package`'d as REQUIRED). CUDA ≥6.5 is optional — if absent, only the `*_OpenMP_GCC_*` targets build.

## Tests

`tests/` holds a Catch2 unit-test suite (the example executables above are *not* tests). It is a self-contained CMake project that pulls in Thrust and Catch2 via FetchContent — no ROOT, TCLAP, or CUDA needed — pinned to the versions GooFit uses (Thrust 1.8.3, Catch2 v2.13.9). The backend defaults to the CPU `CPP` Thrust backend, so the suite builds and runs with only a C++11 compiler.

```bash
cmake -S tests -B build-tests -DMCBOOSTER_BACKEND=CPP   # or OMP
cmake --build build-tests -j
ctest --test-dir build-tests --output-on-failure
```

`.github/workflows/ci.yml` runs this on gcc and clang × `CPP`/`OMP`. Coverage: `test_vector4r` (Lorentz/3-vector math), `test_generate` (`PhaseSpace` mass/conservation/weights/unweighting), `test_evaluate` (`EvaluateArray` with a user `IFunctionArray`, mirroring GooFit's usage). Tests use only the public `mcbooster::` types so they are backend-agnostic.

Example executables follow `MCBooster_Example_<BACKEND_AND_COMPILER>_<NAME>`:
- `*_B2KPiJpsi` (`src/Generate.cu`/`.cpp`) — generates B0→J/ψ K π and evaluates kinematic variables in parallel.
- `*_GenerateSample` (`src/GenerateSample.cu`) — CLI-driven generator that writes a ROOT TTree.
- `*_PerformanceTest` (`src/PerformanceTest.cu`/`.cpp`) — timing vs. number of events/particles.
- `*_CompareWithRoot` (`src/CompareWithTGenPhaseSpace.cu`) — validates against ROOT.

The `.cu` and `.cpp` files for a given example are usually **the same code** — `.cu` is compiled by nvcc, `.cpp` by gcc for the OpenMP-only path. Keep them in sync when editing.

## Backend selection (key concept)

The backend is chosen at compile time via the `MCBOOSTER_BACKEND` macro (`CUDA`, `OMP`, `TBB`, or `CPP`), defaulting to `CUDA` (`mcbooster/Config.h`). Examples set it with `-DMCBOOSTER_BACKEND=OMP`. Because everything routes through Thrust execution policies, **writing code only with MCBooster's provided types lets you switch backends just by recompiling** (often literally `.cu` → `.cpp`).

## Architecture

Everything is in `namespace mcbooster`. Headers include each other through `mcbooster/Config.h`, which selects the backend and pulls in Thrust.

- **`GTypes.h`** — portable scalar typedefs (`GReal_t` is `double`, or `float` under `-DFP_SINGLE`; `GInt_t`, `GBool_t`, etc.). `kMAXP 9` caps the number of particles. Use these types everywhere instead of raw C++ types.
- **`GContainers.h` / `GContainersHost.h`** — the container layer. `mc_device_vector<T>` / `mc_host_vector<T>` wrap Thrust vectors so user code stays backend-agnostic. Domain typedefs: `RealVector_d`, `BoolVector_d`, `Particles_d` (a `Vector4R` device vector), and the set types `ParticlesSet_d` / `VariableSet_d` (STL vectors of pointers to those device vectors — one entry per particle/variable).
- **`Vector4R.h` / `Vector3R.h`** — `__host__ __device__` Lorentz/3-vector math used inside functors and user kinematics code.
- **`Generate.h`** — the core. `class PhaseSpace` is the generator: construct with mother mass, daughter masses, and event count; call `Generate(mother)`, then `Unweight()`, then `Export(Events*)` (or `ExportUnweighted`). `struct Events` is the host-side output container. Generation runs the `functors/` as Thrust transforms.
- **`Evaluate.h` / `EvaluateArray.h`** — free function templates that run a user functor over a `ParticlesSet_d` in parallel. `Evaluate` returns one value per event; `EvaluateArray` fills a `VariableSet_d` (several variables per event in one pass). Overloads target host output, device output, or in-place.
- **`GFunctional.h`** — the two abstract functor interfaces users subclass: `IFunction<RESULT>` (one value) and `IFunctionArray` (fills a `GReal_t*` array). Both declare `__host__ __device__ virtual operator()` taking `Vector4R** particles`.
- **`functors/`** — internal Thrust functors driving generation: `DecayMother`/`DecayMothers` (single fixed mother vs. per-event mothers, for sequential decays), `RandGen`, `FlagAcceptReject` + `IsAccepted` (the unweighting), `Calculate`.
- **`strided_iterator.h`** — iterator helper for interleaved data layouts.

`MCBooster.h` is generated by CMake from `MCBooster.h.in` (carries the version); the committed copy may be regenerated by a `cmake` run.

## Conventions

- `GReal_t` not `double`, `GInt_t` not `int`, etc. — this preserves the `FP_SINGLE` switch and portability.
- Functions touching kinematics are marked `__host__ __device__` so they work on both backends.
- New headers should include `mcbooster/Config.h` (directly or transitively) before using Thrust.
