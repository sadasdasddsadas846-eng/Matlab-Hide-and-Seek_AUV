function phase4_demo(seed)
% PHASE4_DEMO visualizes one hide-and-seek run in the planar arena.

if nargin < 1 || isempty(seed)
    seed = 1;
end

config = phase4_config();
trial = phase4_run_trial(seed, config);

figure('Color', 'w', 'Position', [100 100 900 600]);
plot(trial.stateA(:, 1), trial.stateA(:, 2), 'b', 'LineWidth', 2); hold on;
plot(trial.stateB(:, 1), trial.stateB(:, 2), 'r', 'LineWidth', 2);
plot(config.goal(1), config.goal(2), 'kx', 'MarkerSize', 10, 'LineWidth', 2);
plot(trial.stateA(1, 1), trial.stateA(1, 2), 'bo', 'MarkerSize', 7); 
plot(trial.stateB(1, 1), trial.stateB(1, 2), 'ro', 'MarkerSize', 7);
legend('Agent A', 'Agent B', 'Goal', 'Location', 'best');
axis equal;
axis([0 config.arenaSize 0 config.arenaSize]);
grid on;
xlabel('x (m)');
ylabel('y (m)');
title(sprintf('Phase 4 demo, outcome = %s', trial.outcome));
end
