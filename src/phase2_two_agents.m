function results = phase2_two_agents()
% PHASE2_TWO_AGENTS runs a simple side-by-side two-agent plain MATLAB demo.
% Agent A is the stealth drone and Agent B is the seeker drone.
% Both use the same heading+speed autopilot from Phase 1, but with different
% initial states and target commands.

paramsA = auv_agent_params('A');
paramsB = auv_agent_params('B');

% Simple test conditions: A heads east, B heads north.
paramsA.initialState = [0; 0; 0; 0; 0; 0];
paramsA.targetSpeed = 1.0;                  % m/s
paramsA.targetHeading = deg2rad(0);         % rad

paramsB.initialState = [8; 6; deg2rad(90); 0; 0; 0];
paramsB.targetSpeed = 0.8;                  % m/s
paramsB.targetHeading = deg2rad(90);        % rad

paramsA.sim = struct('dt', 0.05, 'tFinal', 20);
paramsB.sim = struct('dt', 0.05, 'tFinal', 20);

dt = paramsA.sim.dt;
tFinal = paramsA.sim.tFinal;
steps = ceil(tFinal / dt) + 1;

t = zeros(steps, 1);
stateA = zeros(steps, 6);
stateB = zeros(steps, 6);

stateA(1, :) = paramsA.initialState.';
stateB(1, :) = paramsB.initialState.';

t(1) = 0.0;
for k = 1:(steps - 1)
    sA = stateA(k, :).';
    sB = stateB(k, :).';

    controlA = auv_simple_controller(sA, paramsA.targetSpeed, paramsA.targetHeading, paramsA);
    controlB = auv_simple_controller(sB, paramsB.targetSpeed, paramsB.targetHeading, paramsB);

    dStateA = auv_planar_dynamics(sA, controlA, paramsA);
    dStateB = auv_planar_dynamics(sB, controlB, paramsB);

    stateA(k + 1, :) = (sA + dStateA * dt).';
    stateB(k + 1, :) = (sB + dStateB * dt).';
    t(k + 1) = t(k) + dt;
end

figure('Color', 'w', 'Position', [100 100 900 600]);
plot(stateA(:, 1), stateA(:, 2), 'b', 'LineWidth', 2); hold on;
plot(stateB(:, 1), stateB(:, 2), 'r', 'LineWidth', 2);
plot(stateA(1, 1), stateA(1, 2), 'bo', 'MarkerSize', 8, 'LineWidth', 2);
plot(stateB(1, 1), stateB(1, 2), 'ro', 'MarkerSize', 8, 'LineWidth', 2);
legend('Agent A (stealth)', 'Agent B (seeker)', 'Location', 'best');
grid on;
axis equal;
xlabel('x (m)');
ylabel('y (m)');
title('Phase 2 plain MATLAB two-agent test');

saveas(gcf, 'phase2_two_agents_figure.png');

results = struct('stateA', stateA, 'stateB', stateB, ...
    'finalA', stateA(end, :), 'finalB', stateB(end, :));

fprintf('Plain MATLAB two-agent simulation complete.\n');
fprintf('Agent A final position: x = %.2f m, y = %.2f m, psi = %.2f deg\n', ...
    stateA(end, 1), stateA(end, 2), rad2deg(stateA(end, 3)));
fprintf('Agent B final position: x = %.2f m, y = %.2f m, psi = %.2f deg\n', ...
    stateB(end, 1), stateB(end, 2), rad2deg(stateB(end, 3)));
end
