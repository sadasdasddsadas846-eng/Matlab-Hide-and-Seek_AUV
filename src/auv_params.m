function params = auv_params()
% AUV_PARAMS returns the parameter set used for the simplified planar
% three-degree-of-freedom (3-DOF) underwater vehicle model.
%
% This project uses a custom, simulation-only model. It is intentionally
% independent of the MathWorks AUV example and any toolbox dependency.
%
% Every numerical value in this file is labeled as either "from my brief" or
% "ASSUMPTION" in the inline comments below.

params.vehicle = struct();
params.vehicle.dryMass = 22.5;       % kg, from my brief: dry mass 22.5 kg
params.vehicle.length = 0.54;        % m, from my brief: vehicle length 0.54 m
params.vehicle.width = 0.28;         % m, from my brief: vehicle width 0.28 m
params.vehicle.height = 0.24;        % m, from my brief: vehicle height 0.24 m

% Added mass and damping terms for a simplified planar model.
params.dynamics = struct();
params.dynamics.massSurge = 28.0;    % kg, from my brief: surge mass incl. added mass
params.dynamics.massSway = 35.2;     % kg, from my brief: sway mass incl. added mass
params.dynamics.yawInertia = ...
    0.5 * (params.vehicle.dryMass + params.dynamics.massSurge) * ...
    ((params.vehicle.length^2 + params.vehicle.width^2) / 12); % kg*m^2, ASSUMPTION: not given in the brief, so I estimated it from a slender-body mass-radius approximation to keep the turn dynamics physically plausible in simulation.

params.damping = struct();
params.damping.Xu = 4.03;            % N/(m/s), from my brief: surge linear damping Xu = 4.03
params.damping.Xuu = 18.18;          % N/(m/s)^2, from my brief: surge quadratic damping Xuu = 18.18
params.damping.Yv = 6.22;            % N/(m/s), from my brief: sway linear damping Yv = 6.22
params.damping.Yvv = 21.66;          % N/(m/s)^2, from my brief: sway quadratic damping Yvv = 21.66
params.damping.Zw = 5.18;            % N/(m/s), from my brief: heave damping Zw = 5.18
params.damping.Zww = 36.99;          % N/(m/s)^2, from my brief: heave quadratic damping Zww = 36.99
params.damping.YawLinear = 0.07;     % N*m/(rad/s), ASSUMPTION: not given in the brief, so I used a small rotational damping term to keep yaw-rate decay stable without requiring a measured inertia test.
params.damping.YawQuadratic = 1.55;  % N*m/(rad/s)^2, ASSUMPTION: not provided in the brief, so I chose a modest quadratic yaw damping term to prevent unrealistically sustained turn oscillations in the simplified planar model.

% Scaling used by the simple controller. These are simulation tuning values,
% not official aerodynamic or hydrodynamic coefficients.
params.control = struct();
params.control.maxSpeed = 1.5;       % m/s, ASSUMPTION: chosen as a conservative saturation limit above the 1.0 m/s command so the limit does not bind during the baseline test.
params.control.maxYawRate = 0.75;    % rad/s, ASSUMPTION: chosen as a realistic moderate turn-rate limit (about 43 deg/s) for stable, non-aggressive heading changes.
params.control.speedGain = 2.0;      % 1/s, ASSUMPTION: tuned to reduce steady-state speed error while keeping the simple surge loop stable.
params.control.kpYaw = 3.2;          % 1/s, ASSUMPTION: chosen to give a fast but damped heading response without large overshoot in the 30 deg step test.
params.control.kdYaw = 1.8;           % s, ASSUMPTION: chosen to add yaw-rate damping and reduce oscillation after the heading step.

params.sim = struct();
params.sim.dt = 0.05;                % s, from my brief: integration step used for the simulation-only demo
params.sim.tFinal = 30.0;            % s, from my brief: total simulation time for the baseline study
params.sim.initialState = [0; 0; 0; 0; 0; 0];
% state vector: [x (m), y (m), psi (rad), u (m/s), v (m/s), r (rad/s)]

end
