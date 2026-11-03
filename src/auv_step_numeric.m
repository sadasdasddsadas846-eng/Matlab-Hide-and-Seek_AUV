function xdot = auv_step_numeric(x, u)
% AUV_STEP_NUMERIC computes the next state derivative for the Phase 1 planar AUV.
% x is a 6x1 state vector [x, y, psi, u, v, r]'.
% u is a numeric column vector with:
%   [targetSpeed; targetHeading; p(1:14)]
% where p is the same parameter vector used by auv_simple_controller_numeric and
% auv_planar_dynamics_numeric.

if nargin ~= 2
    error('auv_step_numeric:BadInput', 'auv_step_numeric requires exactly two inputs: x and u.');
end

if ~isnumeric(x) || ~isvector(x) || numel(x) ~= 6
    error('auv_step_numeric:BadState', 'x must be a 6x1 numeric state vector.');
end

if ~isnumeric(u) || ~isvector(u) || numel(u) ~= 16
    error('auv_step_numeric:BadInput', 'u must be a numeric vector with length 16: [targetSpeed; targetHeading; p(1:14)].');
end

x = x(:);
u = u(:);

targetSpeed = u(1);
targetHeading = u(2);
p = u(3:end);

control = auv_simple_controller_numeric(x, targetSpeed, targetHeading, p);
xdot = auv_planar_dynamics_numeric(x, control, p);
end
