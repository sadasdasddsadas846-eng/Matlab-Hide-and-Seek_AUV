function [t, state, controlHistory] = auv_planar_step_response(params, desiredSpeed, desiredHeading)
% AUV_PLANAR_STEP_RESPONSE simulates a simple 3-DOF planar AUV under a
% speed-and-heading step input.
%
% Inputs:
%   params         Parameter struct produced by auv_params.m
%   desiredSpeed   Target surge speed in m/s
%   desiredHeading Desired heading in radians
%
% Outputs:
%   t              Time vector (s)
%   state          Simulated state history [x, y, psi, u, v, r]
%   controlHistory Control history [uCmd, yawRateCmd]

if nargin < 3
    desiredSpeed = 1.0;
    desiredHeading = deg2rad(30);
end

state = params.sim.initialState(:);
dt = params.sim.dt;
tEnd = params.sim.tFinal;
N = ceil(tEnd / dt) + 1;

t = (0:(N - 1)).' * dt;
stateHistory = zeros(N, 6);
controlHistory = zeros(N, 2);

for k = 1:N
    if t(k) < 5.0
        speedRef = 0.0;
        headingRef = 0.0;
    else
        speedRef = desiredSpeed;
        headingRef = desiredHeading;
    end

    control = auv_simple_controller(state, speedRef, headingRef, params);
    stateDot = auv_planar_dynamics(state, control, params);
    state = state + dt * stateDot;

    stateHistory(k, :) = state(:).';
    controlHistory(k, :) = control(:).';
end

state = stateHistory;
end
