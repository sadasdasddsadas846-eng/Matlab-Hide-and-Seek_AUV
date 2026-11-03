function result = phase4_run_trial(seed, config)
% PHASE4_RUN_TRIAL runs one hide-and-seek trial using the validated planar AUV
% model and the simple stealth/seeker policies.

if nargin < 2 || isempty(config)
    config = phase4_config();
end
if nargin < 1 || isempty(seed)
    seed = 1;
end

rng(seed);

paramsA = auv_agent_params('A');
paramsB = auv_agent_params('B');

paramsA.initialState = config.agentAStart(:);
paramsB.initialState = config.agentBStart(:);
paramsA.targetSpeed = config.agentASpeed;
paramsB.targetSpeed = config.agentBSpeed;

stateA = paramsA.initialState(:);
stateB = paramsB.initialState(:);

trajA = zeros(ceil(config.timeout / config.dt) + 1, 6);
trajB = zeros(size(trajA));
trajA(1, :) = stateA(:).';
trajB(1, :) = stateB(:).';

detectionCount = 0;
resultState = 'RUNNING';

for k = 1:(size(trajA, 1) - 1)
    t = k * config.dt;

    [cmdA, aMode, aDetect] = phase4_agent_a_policy(stateA, stateB(1:2), config, t);
    [bMode, cmdB, bDetect, lastSeen] = phase4_agent_b_policy(stateB, stateA(1:2), config, t);

    if bDetect
        detectionCount = detectionCount + 1;
    end

    if norm(stateA(1:2) - stateB(1:2)) <= config.captureDistance
        resultState = 'B_WINS';
        break;
    end

    if norm(stateA(1:2) - config.goal(:)) <= config.goalRadius
        resultState = 'A_WINS';
        break;
    end

    controlA = auv_simple_controller(stateA, cmdA(1), cmdA(2), paramsA);
    controlB = auv_simple_controller(stateB, cmdB(1), cmdB(2), paramsB);

    dStateA = auv_planar_dynamics(stateA, controlA, paramsA);
    dStateB = auv_planar_dynamics(stateB, controlB, paramsB);

    stateA = stateA + dStateA * config.dt;
    stateB = stateB + dStateB * config.dt;

    trajA(k + 1, :) = stateA(:).';
    trajB(k + 1, :) = stateB(:).';

    if t >= config.timeout
        resultState = 'TIMEOUT';
        break;
    end
end

trajA = trajA(1:k + 1, :);
trajB = trajB(1:k + 1, :);

result = struct();
result.outcome = resultState;
result.stateA = trajA;
result.stateB = trajB;
result.time = k * config.dt;
result.detectionCount = detectionCount;
result.lastSeen = lastSeen;
result.aMode = aMode;
result.bMode = bMode;
end
