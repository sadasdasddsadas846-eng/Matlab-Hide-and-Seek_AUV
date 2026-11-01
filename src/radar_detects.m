function [detected, distance, bearingErrorDeg, beamDirectionDeg, targetBearingDeg] = radar_detects(params, radarPos, targetPos, t)
% RADAR_DETECTS checks whether a target lies inside the rotating radar sector.
%
% The logic is deterministic for the MVP:
%   1) target must be within max range
%   2) target bearing relative to the beam must be inside half the beamwidth
%   3) target bearing is wrapped to [-180, 180] degrees before comparison
%
% Returns:
%   detected         - true if the target is in the radar sector
%   distance         - Euclidean distance from the radar center to the target
%   bearingErrorDeg  - absolute angular error between target bearing and beam
%   beamDirectionDeg - beam direction in degrees
%   targetBearingDeg - target bearing in degrees

if nargin < 4 || isempty(t)
    t = 0.0;
end
if nargin < 1 || isempty(params)
    params = radar_params();
end

radarPos = radarPos(:);
targetPos = targetPos(:);

delta = targetPos - radarPos;
distance = hypot(delta(1), delta(2));

beamDirectionRad = radar_beam_angle(t, params);
beamDirectionDeg = rad2deg(beamDirectionRad);

targetBearingRad = atan2(delta(2), delta(1));
targetBearingDeg = rad2deg(targetBearingRad);

% Wrap the difference to [-180, 180] degrees. This keeps the radar logic
% stable when the target is near the +/-180 degree boundary.
angleDiffDeg = mod((targetBearingDeg - beamDirectionDeg) + 180, 360) - 180;
if abs(angleDiffDeg + 180) < 1e-12
    angleDiffDeg = 180;
end
bearingErrorDeg = abs(angleDiffDeg);

if distance <= 0
    targetBearingDeg = beamDirectionDeg;
    bearingErrorDeg = 0.0;
end

detected = (distance <= params.maxRange) && ...
           (bearingErrorDeg <= (params.beamwidthDeg / 2));
end
