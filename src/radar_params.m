function params = radar_params()
% RADAR_PARAMS returns the fixed-frequency radar settings for the seeker drone.
% The values below are intentionally simple and explicit, so the geometry can
% be checked in unit tests before we connect the sensor to a larger scenario.

params.scanPeriod = 10.0;                                   % s, from the project description: fixed scan period of 10 s
params.scanFrequency = 1 / params.scanPeriod;               % Hz, from the project description: fixed scanning frequency for one full sweep in one scan period
params.maxRange = 15.0;                                    % m, from the project description: suggested starting max range for the radar sensor
params.beamwidthDeg = 30.0;                                % deg, from the project description: suggested starting beamwidth for a narrow sector
params.beamwidth = deg2rad(params.beamwidthDeg);           % rad, from the project description: equivalent beamwidth in radians
params.arenaSize = 40.0;                                   % m, from the project description: planar arena is about 40 m x 40 m
params.theta0 = 0.0;                                       % rad, ASSUMPTION: starting beam angle at zero radians keeps the first detection tests deterministic and easy to explain
params.maxTargetSpeed = 1.0;                               % m/s, ASSUMPTION: a moderate vehicle speed target consistent with the Phase 1 controllers and the project brief
params.minTargetSpeed = 0.8;                               % m/s, ASSUMPTION: a realistic seeker speed matched to the same Phase 1 tuning scale, so the radar range is large enough to catch the target before it exits the arena

% Reasoning for the assumptions:
% - The 10 s scan period is reasonable because both agents move only about 1.0
%   m/s and 0.8 m/s, so a 10 s sweep gives enough time to scan the arena and
%   still react before a target exits a 40 m x 40 m area.
% - The 15 m max range is a practical starting value for a 40 m arena when the
%   agents are roughly 1 m/s and the radar should detect a target before they get
%   too far apart.
% - The 30 deg beamwidth is a reasonable first-pass sector width because it is
%   narrow enough to require deliberate scanning, but not so narrow that a 1 m/s
%   target disappears between sweeps at a 10 s period.
end
