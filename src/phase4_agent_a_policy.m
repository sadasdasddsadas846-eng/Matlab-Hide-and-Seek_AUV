function [command, stateName, detectionState] = phase4_agent_a_policy(state, radarPos, config, t)
% PHASE4_AGENT_A_POLICY implements the stealth policy for Agent A.
% It uses a compact state machine with the following stages:
%   HIDE -> WAIT_FOR_GAP -> ADVANCE -> HOLD -> REACH_GOAL
%
% The logic is intentionally simple and transparent so it can be defended in
% a university project and is based on the same planar AUV model used earlier.

if nargin < 4 || isempty(t)
    t = 0.0;
end
if nargin < 3 || isempty(config)
    config = phase4_config();
end
if nargin < 2 || isempty(radarPos)
    radarPos = [8.0; 6.0];
end

state = state(:);
if numel(state) < 3
    error('phase4_agent_a_policy:InvalidState', 'State must contain x, y, psi.');
end

pos = state(1:2);
heading = state(3);

goal = config.goal(:);

distanceToGoal = norm(pos - goal);
if distanceToGoal <= config.goalRadius
    stateName = 'REACH_GOAL';
    command = [0.0; heading];
    detectionState = false;
    return;
end

radarPos = radarPos(:);
delta = pos - radarPos;
distanceToRadar = hypot(delta(1), delta(2));
if distanceToRadar <= 0
    bearingDiff = 0.0;
else
    targetBearing = atan2(delta(2), delta(1));
    beamDirection = radar_beam_angle(t, config.radar);
    bearingDiff = abs(wrap_to_pi_local(targetBearing - beamDirection));
end

beamHit = (distanceToRadar <= config.radar.maxRange) && ...
          (bearingDiff <= (config.radar.beamwidth / 2));

if beamHit
    stateName = 'WAIT_FOR_GAP';
    command = [0.0; heading + config.safeGapBias];
    detectionState = true;
    return;
end

stateName = 'ADVANCE';
command = [config.agentASpeed; atan2(goal(2) - pos(2), goal(1) - pos(1))];
detectionState = false;
end
