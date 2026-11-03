function config = phase4_config()
% PHASE4_CONFIG holds the Phase 4 scenario settings for the planar hide-and-seek
% simulation. All values are design choices unless they come directly from an
% existing repo file such as auv_agent_params or radar_params.
%
% Design choices are stored here so the scenario logic remains explicit and
% reproducible without burying them in helper functions.

config = struct();

config.arenaSize = 40.0;                     % m, design choice
config.dt = 0.05;                           % s, design choice
config.timeout = 120.0;                     % s, design choice
config.goalRadius = 1.5;                    % m, design choice
config.captureDistance = 2.0;               % m, design choice
config.bWinsOnDetection = false;            % bool, design choice

config.goal = [35.0; 0.0];                              % m, design choice
config.agentAStart = [0.0; 0.0; 0.0; 0.0; 0.0; 0.0];     % x, y, psi, u, v, r (m, m, rad, m/s, m/s, rad/s), design choice
config.agentBStart = [8.0; 6.0; deg2rad(90); 0.0; 0.0; 0.0]; % x, y, psi, u, v, r (m, m, rad, m/s, m/s, rad/s), design choice

config.agentASpeed = 1.0;                   % m/s, design choice
config.agentBSpeed = 0.8;                   % m/s, design choice
config.agentAHeading = 0.0;                 % rad, design choice
config.agentBHeading = deg2rad(90);         % rad, design choice

config.radar = radar_params();              % uses the repo's radar sensor settings
config.scanTime = config.radar.scanPeriod; % s, from radar_params
config.searchSpeed = config.agentBSpeed;    % m/s, design choice
config.safeGapBias = 0.25;                  % rad, design choice

config.minSeparation = 0.0;                 % m, design choice
config.maxRuns = 50;                        % runs, design choice
config.initialNoiseSigma = 0.25;            % m, design choice
config.initialNoiseBound = 3.0 * config.initialNoiseSigma; % m, design choice
end
