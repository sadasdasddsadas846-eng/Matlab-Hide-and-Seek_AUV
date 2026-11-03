function [mode, command, detection, lastSeen] = phase4_agent_b_policy(state, targetPos, config, t)
% PHASE4_AGENT_B_POLICY implements the seeker policy for Agent B.
% It searches for a target, switches to pursuit after detection, and then
% captures if the target is close enough.

if nargin < 4 || isempty(t)
    t = 0.0;
end
if nargin < 3 || isempty(config)
    config = phase4_config();
end
if nargin < 2 || isempty(targetPos)
    targetPos = [0.0; 0.0];
end

state = state(:);
targetPos = targetPos(:);

radarPos = state(1:2);
[detection, ~, ~, ~, ~] = radar_detects(config.radar, radarPos, targetPos, t);
lastSeen = targetPos;

if norm(targetPos - radarPos) <= config.captureDistance
    mode = 'CAPTURE';
    command = [0.0; state(3)];
    return;
end

if detection
    mode = 'PURSUE';
    targetHeading = atan2(targetPos(2) - radarPos(2), targetPos(1) - radarPos(1));
    command = [config.agentBSpeed; targetHeading];
    lastSeen = targetPos;
    return;
end

mode = 'SEARCH';
searchHeading = mod(t / config.radar.scanPeriod, 2 * pi);
command = [config.searchSpeed; searchHeading];
end
