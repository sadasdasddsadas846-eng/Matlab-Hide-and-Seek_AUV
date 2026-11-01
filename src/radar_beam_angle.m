function theta = radar_beam_angle(t, params)
% RADAR_BEAM_ANGLE returns the radar beam direction in radians.
% The beam rotates at a fixed angular rate, so the angle is:
%   theta(t) = theta0 + omega*t
% where omega = 2*pi*f and f = 1 / scanPeriod.
%
% Inputs:
%   t       - time in seconds
%   params  - radar parameter struct from radar_params.m
%
% Output:
%   theta  - beam direction in radians, measured from the world +x axis

if nargin < 2 || isempty(params)
    params = radar_params();
end

omega = 2 * pi * params.scanFrequency;
theta = params.theta0 + omega * t;
end
