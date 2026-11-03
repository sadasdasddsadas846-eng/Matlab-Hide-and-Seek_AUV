# Underwater Drone Hide-and-Seek

This README is a repo-based summary only. The official README text was not provided in the prompt, so statements outside the repo evidence or the brief are marked as not verified below.

## Verified repo status

- The project contains a planar 3-DOF AUV model, a plain MATLAB two-agent reference, a radar geometry/detection implementation, and a plain MATLAB Phase 4 hide-and-seek loop.
- The MATLAB environment used for validation is R2025b.
- `ver` shows MATLAB, Simulink, and UAV Toolbox; Stateflow is not installed in this environment.
- The state-machine logic is implemented in plain MATLAB functions (`phase4_agent_a_policy.m`, `phase4_agent_b_policy.m`), not in a Stateflow chart.

## Verified run commands

From the project root:

```matlab
addpath('src');
addpath('tests');
runtests('tests');
```

Baseline Phase 1 check:

```matlab
addpath('src');
phase1_demo;
```

Phase 4 demo:

```matlab
addpath('src');
phase4_demo(1);
```

## Not verified / not supported by the repo

- The official GitHub README text from MathWorks is not provided here, so any claims about the challenge description, license, or demo requirements beyond the project brief are not verified.
- The full challenge README, MIT license, and public GitHub repo steps are not established by the local repo alone.
- Simulink or Stateflow Phase 4 implementation is not verified here because this work is restricted to the plain MATLAB implementation and Stateflow is not installed.
- Any claim about a demo video or a public release is not verified by the repo contents.

## Scope and limitations

- Simulation only.
- Planar 2D model.
- No hardware use.
- The implementation is purposely simple and transparent so each file can be explained and defended.
